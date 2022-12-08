#include "VertRenderData.h"
#include "AniInfoManager.h"
#include "Transform.h"

namespace R2grap{

VerticesRenderData::VerticesRenderData(const LayersInfo* data){
  auto shape_offset = data->GetShapeTransform()->GetShapeGrapOffset();
  auto shape_groups = data->GetShapeGroup();
  layer_ind_ = data->GetLayerInd() - 1;

  for(auto i = 0; i < shape_groups.size(); i++){
    std::vector<unsigned int> indexs = { (unsigned int)i };
    RecusCalcBezierVertData(shape_groups[i], indexs, shape_offset);
  }
}

void VerticesRenderData::RecusCalcBezierVertData(const std::shared_ptr<ShapeGroup> group, std::vector<unsigned int> indexs, glm::vec3 parent_offset) {
  if (group->HasChildGroups()) {
    auto child_groups = group->GetChildGroups();
    for (auto i = 0; i < child_groups.size(); i++) {
      auto offset = group->GetTransform()->GetOrigPosition() - group->GetTransform()->GetAnchorPos() + parent_offset;
      auto cur_inds = indexs;
      cur_inds.push_back(i);
      RecusCalcBezierVertData(child_groups[i], cur_inds, offset);
    }
  }
  else {
    std::vector<BezierVertData> vert_data;
    GenerateVertCacheData(indexs, group, parent_offset, vert_data);
    bezier_vert_data_.insert(bezier_vert_data_.end(), vert_data.begin(), vert_data.end());
  }
}

void VerticesRenderData::GenerateVertCacheData(const std::vector<unsigned int>& indexs, const std::shared_ptr<ShapeGroup> group, glm::vec3 parent_offset, std::vector<BezierVertData>& vert_data) {
  auto paths = group->GetContents()->GetPaths();
  auto final_offset = group->GetTransform()->GetPosition() + parent_offset;
  for (auto i = 0; i < paths.size(); i++) {
    BezierVertData signal_path_data;
    signal_path_data.group_indexs = indexs;
    signal_path_data.path_ind = i;
    signal_path_data.closed = paths[i]->IsClosed();
    auto bezier_verts = paths[i]->GetBezierVertices();
    for (auto& vert : bezier_verts) {
      auto tmp_vert = Normalize<glm::vec2>(vert + glm::vec2(final_offset.x, final_offset.y));
      signal_path_data.verts.emplace_back(tmp_vert.x);
      signal_path_data.verts.emplace_back(tmp_vert.y);
      signal_path_data.verts.emplace_back(0);
    }

    if (paths[i]->IsClosed())
      signal_path_data.tri_index = paths[i]->GetTriIndexList();

    if (paths[i]->HasKeyframe()) {
      unsigned int inpos = 0, outpos = 0;
      AniInfoManager::GetIns().GetLayerInandOutPos(layer_ind_, inpos, outpos);
      auto linear_map = paths[i]->GetLinearMap();

      for (auto& el : linear_map) {
        std::vector<float> tmp_float_arr;
        auto dym_verts = el.second;
        for (auto& vert : dym_verts) {
          auto tmp_vert = Normalize<glm::vec2>(vert + glm::vec2(final_offset.x, final_offset.y));
          tmp_float_arr.emplace_back(tmp_vert.x);
          tmp_float_arr.emplace_back(tmp_vert.y);
          tmp_float_arr.emplace_back(0);
        }
        signal_path_data.linear_verts[el.first] = tmp_float_arr;
      }
      auto tmp_path_data = signal_path_data.linear_verts;
      for (auto i = inpos; i <= outpos; i++) {
        if (!tmp_path_data.count(i) && i < tmp_path_data.begin()->first)
          signal_path_data.linear_verts[i] = tmp_path_data.begin()->second;
        else if (!tmp_path_data.count(i) && i > tmp_path_data.rbegin()->first)
          signal_path_data.linear_verts[i] = tmp_path_data.rbegin()->second;
      }
    }

    if (paths[i]->IsClosed() && paths[i]->HasKeyframe()) {
      signal_path_data.linear_trig = paths[i]->GetTrigIndexMap();
    }
    vert_data.emplace_back(signal_path_data);
  }
}

template<typename T>
T VerticesRenderData::Normalize(const T& pos){
  auto width = AniInfoManager::GetIns().GetWidth();
  auto height = AniInfoManager::GetIns().GetHeight();
  if(typeid(T) == typeid(glm::vec2)){
    return glm::vec2((pos.x - width/2) / width, (pos.y - height/2) / height);
  }
  else
    return glm::vec3((pos.x - width/2) / width, (pos.y - height/2) / height, 0);
}

bool VerticesRenderData::GetVertices(unsigned int path_ind, std::vector<float>& verts){
  if(path_ind >= bezier_vert_data_.size()) return false;
    verts = bezier_vert_data_[path_ind].verts;
    return true;
}

bool VerticesRenderData::GetVertices(const std::vector<unsigned int>& indexs, unsigned int path_ind, std::vector<float>& verts){
  auto it = std::find_if(bezier_vert_data_.begin(), bezier_vert_data_.end(), [&](const BezierVertData& data){
    return (indexs == data.group_indexs && path_ind == data.path_ind);
  });
  if(it == bezier_vert_data_.end()) return false;
  verts = bezier_vert_data_[it - bezier_vert_data_.begin()].verts;
  return true;
}

bool VerticesRenderData::GetTriangleIndex(unsigned int ind, std::vector<unsigned int>& trigs) {
  if (ind >= bezier_vert_data_.size()) return false;

  trigs = bezier_vert_data_[ind].tri_index;
  return true;
}

bool VerticesRenderData::GetTriangleIndex(const std::vector<unsigned int>& indexs, unsigned int path_ind, std::vector<unsigned int>& trigs) {
  auto it = std::find_if(bezier_vert_data_.begin(), bezier_vert_data_.end(), [&](const BezierVertData& data) {
    return (indexs == data.group_indexs && path_ind == data.path_ind);
  });
  if (it == bezier_vert_data_.end()) return false;
  trigs = bezier_vert_data_[it - bezier_vert_data_.begin()].tri_index;
  return true;
}

bool VerticesRenderData::GetBezierVertData(const std::vector<unsigned int>& indexs, unsigned int path_ind, BezierVertData& vert_data){
  auto it = std::find_if(bezier_vert_data_.begin(), bezier_vert_data_.end(), [&](const BezierVertData& data) {
    return (indexs == data.group_indexs && path_ind == data.path_ind);
  });
  if (it == bezier_vert_data_.end()) return false;
  vert_data = bezier_vert_data_[it - bezier_vert_data_.begin()];

  return true;
}

}