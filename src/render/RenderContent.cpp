#include "RenderContent.h"
#include "TransComp.hpp"
#include "PathRenderData.h"
#include "AniInfoManager.h"
#include "PathRenderData.h"

namespace R2grap{

RenderContent::RenderContent(LayersInfo* layer_info){
  layer_contents_path_ = SRenderDataFactory::GetIns().CreateVerticesData(layer_info);
  layer_contents_color_ = SRenderDataFactory::GetIns().CreateColorData(layer_info);
  layer_contents_trans_ = SRenderDataFactory::GetIns().CreateTransformData(layer_info); //shape transform->generate layer's transform curve

  layer_data_.index = layer_info->GetLayerInd();
  layer_data_.start_pos = layer_info->GetLayerInpos();
  layer_data_.end_pos = layer_info->GetLayerOutpos();
  layer_transform_ = layer_info->GetShapeTransform();
  shape_groups_ = layer_info->GetShapeGroup();

	bool no_group_keyframe = true;
  for(auto& group : shape_groups_){
    unsigned int group_index = std::find(shape_groups_.begin(), shape_groups_.end(), group) - shape_groups_.begin();
    std::vector<unsigned int> indexs = { group_index };
    GroupData group_data;
    RecusCalcRenderData(group, indexs, group_data, no_group_keyframe);
    layer_data_.group_data.push_back(group_data);
  }
	layer_data_.groups_no_keyframe = no_group_keyframe;
}

void RenderContent::RecusCalcRenderData(const std::shared_ptr<ShapeGroup> group, std::vector<unsigned int> indexs, GroupData& group_data, bool& no_keyframe) {
  if (group->HasChildGroups()) {
    no_keyframe &= group->GetTransform()->IsNoKeyframe();

    auto child_groups = group->GetChildGroups();
    for (auto i = 0; i < child_groups.size(); i++) {
      auto cur_inds = indexs;
      cur_inds.push_back(i);
      GroupData child_group_data;
      RecusCalcRenderData(child_groups[i], cur_inds, child_group_data, no_keyframe);
      group_data.child_groups.push_back(child_group_data);
    }
  }
  else {
    no_keyframe &= group->GetTransform()->IsNoKeyframe();

    auto color_infos = layer_contents_color_->GetColor(indexs);
    LoadColorContent(color_infos, group_data);

    auto path_num = group->GetContents()->GetPathsNum();
    for (auto path_ind = 0; path_ind < path_num; path_ind++) {
      BezierVertData vert_data;
      layer_contents_path_->GetBezierVertData( indexs, path_ind, vert_data);
      LoadPathContent(vert_data, group_data);
    }
  }
}

void RenderContent::LoadColorContent(const std::vector<ColorCacheData>& color_cache, GroupData& group) {
  for (auto& color_info : color_cache) {
    if (color_info.type == ColorDataType::t_cStroke) {
      StrokeData stroke(color_info.color, color_info.opacity, color_info.stroke_wid);
      if (color_info.h_ckf)//color keyframe
        stroke.trans_color = color_info.trans_color;
      if (color_info.h_okf)//opacity keyframe
        stroke.trans_opacity = color_info.trans_opacity;
      if (color_info.h_skf)//stroke width keyframe
        stroke.trans_stroke_wid = color_info.trans_stroke_wid;
      group.stroke = std::make_shared<StrokeData>(stroke);
    }
    else if (color_info.type == ColorDataType::t_cFill) {
      FillData fill(color_info.color, color_info.opacity);
      if (color_info.h_ckf)
        fill.trans_color = color_info.trans_color;
      if (color_info.h_okf)
        fill.trans_opacity = color_info.trans_opacity;
      group.fill = std::make_shared<FillData>(fill);
    }
  }
}

void RenderContent::LoadPathContent(const BezierVertData& vert_data, GroupData& group) {
  PathData path_data;
  path_data.closed = vert_data.closed;
  path_data.has_keyframe = vert_data.linear_verts.size();
  path_data.verts = vert_data.verts;

  if (path_data.closed) {
    path_data.tri_ind = vert_data.tri_index;
  }

  if (path_data.has_keyframe) {
    path_data.trans_verts = vert_data.linear_verts;
  }

  if (path_data.closed && path_data.has_keyframe) {
    path_data.trans_tri_ind = vert_data.linear_trig;
  }
  group.paths.emplace_back(std::make_shared<PathData>(path_data));
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

void RenderContent::UpdateTransRenderData(const std::vector<std::shared_ptr<RenderContent>>& contents, std::vector<RePathObj>& objs){
  auto link_map = AniInfoManager::GetIns().GetLayersLinkMap();
	std::vector<RePathObj> path_objs;

  //Build the layer's transition animation data(render_content->layer_data_->trans)
  for(auto i = 0; i < contents.size(); i++){
    const auto& render_content = contents[i];
    auto trans_render_data = render_content->GetTransRenderData()->GetOrigTransCurve();
    auto link_layers = link_map[i];
    TransformCurve tmp_curve;
    for(auto it = link_layers.rbegin(); it != link_layers.rend(); it++){
      auto link_trans_data = contents[*it]->GetTransRenderData()->GetOrigTransCurve();
      tmp_curve = AddTransCurve(tmp_curve, link_trans_data, false);
    }
    trans_render_data = AddTransCurve(trans_render_data, tmp_curve, true);
    render_content->GetTransRenderData()->SetTransCurve(trans_render_data);
    render_content->GetTransRenderData()->GenerateTransformMat();
		render_content->SetLayerDataTransMat(render_content->GetTransRenderData()->GetTransMat());

    //Recus layer's shapgroups to generate ervery child-group's transform curve and transform matrix;
    if (!render_content->GetLayerData().groups_no_keyframe) {
      auto groups = render_content->GetShapeGroups();
      for (unsigned int j = 0; j < groups.size(); j++) {
        std::vector<unsigned int> indexs = { j };
        TransformCurveEx layer_trans_curve_ex;
        TransformRenderData::ConverCurveToCurveEx(trans_render_data, layer_trans_curve_ex);
        RecusUpdateTransMat(groups[j], render_content->GetLayerData().index, indexs, render_content, layer_trans_curve_ex);
      }
    }
		PathRenderData::GenPathRenderObjs(render_content, path_objs);
  }
	objs = path_objs;
	AniInfoManager::GetIns().SetRenderPathObjs(path_objs);
}

void RenderContent::RecusUpdateTransMat(const std::shared_ptr<ShapeGroup> group, const unsigned int layer_ind, std::vector<unsigned int>& indexs,
  const std::shared_ptr<RenderContent> content, const TransformCurveEx& parent_curve) {
  auto group_contents_trans = SRenderDataFactory::GetIns().CreateTransformData(group.get(), layer_ind - 1, indexs);
  auto group_curve = group_contents_trans->GetGroupOrigTransCurve();

  group_curve = AddTransCurve<TransformCurveEx>(group_curve, const_cast<TransformCurveEx&>(parent_curve), true);

  if (group->HasChildGroups()) {// need to update
    auto child_groups = group->GetChildGroups();
    for (auto i = 0; i < child_groups.size(); ++i) {
      indexs.push_back(i);
      RecusUpdateTransMat(child_groups[i], layer_ind, indexs, content, group_curve);
			indexs.pop_back();
    }
  }
  else {
    group_contents_trans->SetTransCurve(group_curve);
    group_contents_trans->GenerateTransformMat();

    content->SetGroupData(indexs, group_contents_trans->GetTransMat());
  }
}

template<typename TransCurve>
const TransCurve& RenderContent::AddTransCurve(TransCurve& curve1, TransCurve& curve2, bool front_insert) {
  for (auto& el : curve2) {
    if (curve1.count(el.first) == 0) {
      curve1[el.first] = el.second;
    }
    else {
        auto trans1 = curve1[el.first];
        auto trans2 = el.second;
        if (el.first == "Position") {
          //if (trans1.size() != trans2.size() != 1) continue;
          for (auto i = 0; i < trans1.size(); i++) {
            auto map1 = trans1[i].value_map_;
            auto map2 = trans2[i].value_map_;
            if (TransComp::adjustMaps(map1, map2)) {
              TransComp::MapaddMap(map1, map2);
            }
            trans1[i].value_map_ = map1;
          }
          curve1[el.first] = trans1;
        }
        else {
          if (front_insert)
            trans1.insert(trans1.begin(), trans2.begin(), trans2.end());
          else
            trans1.insert(trans1.end(), trans2.begin(), trans2.end());
        }
        curve1[el.first] = trans1;
    }
  }
  return curve1;
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

void RenderContent::SetLayerDataTransMat(TransMat* trans_mat){
	layer_data_.start_pos = trans_mat->clip_start;
	layer_data_.end_pos = trans_mat->clip_end;
	layer_data_.trans = trans_mat->trans;
}

void RenderContent::SetGroupData(std::vector<unsigned int> indexs, TransMat* trans_mat){
	auto group_data = &layer_data_.group_data[indexs.front()];
	for (auto ind = 1; ind < indexs.size(); ind++) {
		group_data = &group_data->child_groups[indexs[ind]];
	}
	group_data->trans = trans_mat->trans;
}

}