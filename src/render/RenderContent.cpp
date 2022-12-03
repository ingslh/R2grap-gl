#include "RenderContent.h"
#include "TransComp.hpp"

namespace R2grap{

RenderContent::RenderContent(LayersInfo* layer_info){
  layer_contents_path_ = SRenderDataFactory::GetIns().CreateVerticesData(layer_info);
  layer_contents_color_ = SRenderDataFactory::GetIns().CreateColorData(layer_info);
  layer_contents_trans_ = SRenderDataFactory::GetIns().CreateTransformData(layer_info); //shape transform

  layer_data_.index = layer_info->GetLayerInd();
  layer_data_.start_pos = layer_info->GetLayerInpos();
  layer_data_.end_pos = layer_info->GetLayerOutpos();

  shape_groups_ = layer_info->GetShapeGroup();
	bool no_group_keyframe = true;
  for(auto& group : shape_groups_){
    GroupData group_data;
		no_group_keyframe &= group->GetTransform()->IsNoKeyframe();

    /*auto trans = group->GetTransform();
    //group transform
    auto group_contents_trans = SRenderDataFactory::GetIns().CreateTransformData(trans.get(), layer_data_.index, layer_data_.start_pos, layer_data_.end_pos, layer_contents_trans_);
    auto group_trans_mat = group_contents_trans->GetTransMat()->trans;
    group_data.trans = group_trans_mat;*/

    unsigned int group_index = std::find(shape_groups_.begin(), shape_groups_.end(), group) - shape_groups_.begin();
    auto color_infos = layer_contents_color_->GetColor(group_index);

    for(auto& color_info : color_infos){
      if(color_info.type == ColorDataType::t_cStroke){
        StrokeData stroke(color_info.color, color_info.opacity, color_info.stroke_wid);
        if(color_info.h_ckf)
          stroke.trans_color = color_info.trans_color;
        if(color_info.h_okf)
          stroke.trans_opacity = color_info.trans_opacity;
        if(color_info.h_skf)
          stroke.trans_stroke_wid = color_info.trans_stroke_wid;
        group_data.stroke = std::make_shared<StrokeData>(stroke);
      }
      else if(color_info.type == ColorDataType::t_cFill){
        FillData fill(color_info.color, color_info.opacity);
        if(color_info.h_ckf)
          fill.trans_color = color_info.trans_color;
        if(color_info.h_okf)
          fill.trans_opacity = color_info.trans_opacity;
        group_data.fill = std::make_shared<FillData>(fill);
      }
    }

    auto path_num = group->GetContents()->GetPathsNum();
    for(auto i = 0; i < path_num; i++){
      BezierVertData vert_data;
      layer_contents_path_->GetBezierVertData(group_index, i, vert_data);
      PathData path_data;
      path_data.closed = vert_data.closed;
      path_data.has_keyframe = vert_data.linear_verts.size();
      path_data.verts = vert_data.verts;

      if(path_data.closed){
        path_data.tri_ind = vert_data.tri_index;
      }

      if(path_data.has_keyframe){
        path_data.trans_verts = vert_data.linear_verts;
      }

      if(path_data.closed && path_data.has_keyframe){
        path_data.trans_tri_ind = vert_data.linear_trig;
      }
      group_data.paths.emplace_back(path_data);
    }
    layer_data_.group_data.emplace_back(group_data);
  }
	layer_data_.groups_no_keyframe = no_group_keyframe;
}

unsigned int RenderContent::GetRenderPathCount(const std::vector<std::shared_ptr<RenderContent>>& contents) {
  unsigned int count = 0;
  for (const auto& content : contents) {
    auto group_data = content->GetGroupData();
    for(auto& data : group_data){
      count += data.paths.size();
    }
  }
  return count;
}

void RenderContent::UpdateTransRenderData(const std::vector<std::shared_ptr<RenderContent>>& contents){
  auto add_trans_curve = [&](TransformCurve& curve1, TransformCurve& curve2 ,bool front_insert)->TransformCurve{
    for(auto& el : curve2){
      if(curve1.count(el.first) == 0){
        curve1[el.first] = el.second;
      }else{
        if(el.first == "Rotation"){
          auto trans1 = std::get<1>(curve1[el.first]);
          auto trans2 = std::get<1>(el.second);
          if(front_insert){
            trans1.insert(trans1.begin(),trans2.begin(),trans2.end());
          }else{
            trans1.insert(trans1.end(),trans2.begin(),trans2.end());
          }
          curve1[el.first] = trans1;
        }else{
          auto trans1 = std::get<0>(curve1[el.first]);
          auto trans2 = std::get<0>(el.second);
          if(trans1.size() != trans2.size()) continue;
          for(auto i = 0; i < trans1.size(); i++){
            auto map1 = trans1[i];
            auto map2 = trans2[i];
            if(TransComp::adjustMaps(map1,map2)){
              if(el.first == "Position")
                TransComp::MapaddMap(map1,map2);
              else if(el.first == "Scale"){
                TransComp::MapmultiplyMap(map1,map2);
                TransComp::MapdivideNum(map1,float(100));
              }
            }
            trans1[i] = map1;
          }
          curve1[el.first] = trans1;
        }
      }
    }
    return curve1;
  };
  
  auto link_map = AniInfoManager::GetIns().GetLayersLinkMap();
  for(auto i = 0; i < contents.size(); i++){
    auto render_content = contents[i];
    auto trans_render_data = render_content->GetTransRenderData()->GetOrigTransCurve();
    auto link_layers = link_map[i];
    TransformCurve tmp_curve;
    for(auto it = link_layers.rbegin(); it != link_layers.rend(); it++){
      auto link_trans_data = contents[*it]->GetTransRenderData()->GetOrigTransCurve();
      tmp_curve = add_trans_curve(tmp_curve, link_trans_data, false); 
    }
    trans_render_data = add_trans_curve(trans_render_data, tmp_curve, true);

    render_content->GetTransRenderData()->SetTransCurve(trans_render_data);
    render_content->GetTransRenderData()->GenerateTransformMat();
    render_content->SetLayerData(render_content->GetTransRenderData()->GetTransMat());

		if(render_content->GetLayerData().groups_no_keyframe) continue;
    auto groups = render_content->GetShapeGroups();
    for (auto j = 0; j < groups.size(); j++) {
      auto group = groups[j];
      auto layer_ind = render_content->GetLayerData().index;
      auto start_pos = render_content->GetLayerData().start_pos;
      auto end_pos = render_content->GetLayerData().end_pos;

      auto group_contents_trans = SRenderDataFactory::GetIns().CreateTransformData(group.get(), layer_ind, start_pos, end_pos);
      auto group_curve = group_contents_trans->GetOrigTransCurve();
      group_contents_trans->SetParentLayerInd(layer_ind - 1);
			group_curve = add_trans_curve(group_curve, trans_render_data, true);
      group_contents_trans->SetTransCurve(group_curve);

      group_contents_trans->GenerateTransformMat();
      render_content->SetGroupData(j, group_contents_trans->GetTransMat());
    }
  }
}

unsigned int RenderContent::GetPathIndex(const std::vector<std::shared_ptr<RenderContent>>& contents, unsigned int layer_ind, unsigned int group_ind, unsigned int path_ind){
  auto get_layerpaths_num = [&](unsigned int ind)->int {
    if(ind > contents.size() - 1) return -1;
    int paths_num = 0;
		auto layer_data = contents[ind]->GetLayerData();
		auto groups = layer_data.group_data;
		for(auto & group : groups){
        paths_num += group.paths.size();
		}
    return paths_num;
  };
  
  unsigned int ret = 0;
  if(layer_ind){
    for(auto i = 0; i <= layer_ind - 1; i++){
      ret += get_layerpaths_num(i);
    }
  }

  if(group_ind){
    for(auto j = 0; j <= group_ind - 1; j++){
      ret += contents[layer_ind]->GetLayerData().group_data[j].paths.size();
    }
  }

  return ret + path_ind;
}

}