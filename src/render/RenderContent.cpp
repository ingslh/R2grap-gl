#include "RenderContent.h"

namespace R2grap{

RenderContent::RenderContent(LayersInfo* layer_info){
  auto layer_contents_path = SRenderDataFactory::GetIns().CreateVerticesData(layer_info);
  auto layer_contents_color = SRenderDataFactory::GetIns().CreateColorData(layer_info);
  auto layer_contents_trans = SRenderDataFactory::GetIns().CreateTransformData(layer_info);

  auto trans_mat = layer_contents_trans->GetTransMat();

  layer_data_.index = layer_info->GetLayerInd();
  layer_data_.start_pos = trans_mat->clip_start;
  layer_data_.end_pos = trans_mat->clip_end;
  layer_data_.trans = trans_mat->trans;

  auto groups = layer_info->GetShapeGroup();
  for(auto& group : groups){
    GroupData group_data;
    auto trans = group->GetTransform();
    auto group_contents_trans = SRenderDataFactory::GetIns().CreateTransformData(trans.get(), layer_data_.index, layer_data_.start_pos, layer_data_.end_pos);
    auto group_trans_mat = group_contents_trans->GetTransMat()->trans;
    group_data.trans = group_trans_mat;

    auto group_it = std::find(groups.begin(), groups.end(), group);
    unsigned int group_index = group_it - groups.begin();
    auto color_infos = layer_contents_color->GetColor(group_index);

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
      layer_contents_path->GetBezierVertData(group_index, i, vert_data);
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


  /*auto path_count = layer_contents_path->GetPathsCount();
  layer_data_.paths_num = path_count;
  for(unsigned int i = 0; i < path_count; i++){
    std::vector<float> vert;
    layer_contents_path->GetVertices(i, vert);
    layer_data_.verts.emplace_back(vert);
    std::vector<unsigned int> trig_index;
    layer_contents_path->GetTriangleIndex(i, trig_index);
    layer_data_.triangle_ind.emplace_back(trig_index);
    layer_data_.color.emplace_back(layer_contents_color->GetColor(i));
  }*/
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

/*unsigned int RenderContent::GetPathIndex(const std::vector<std::shared_ptr<RenderContent>>& contents, unsigned int layer_ind, unsigned int path_ind) {
  unsigned int index = 0;
  if (layer_ind == 0)
    return path_ind;
  else {
    for (auto i = 0; i <= layer_ind - 1; i++) {
      index += contents[i]->GetLayerData().paths_num;
    }
    return index + path_ind;
  }
}*/

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