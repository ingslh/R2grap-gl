#include "VertRenderData.h"
#include "AniInfoManager.h"
#include "Transform.h"

namespace R2grap{

VerticesRenderData::VerticesRenderData(const LayersInfo* data){
  auto shape_offset = data->GetShapeTransform()->GetShapeGrapOffset();
  auto shape_groups = data->GetShapeGroup();
  paths_count_ = 0;

  for(auto& group : shape_groups){
    auto paths = group->GetContents()->GetPaths();
    paths_count_ += static_cast<unsigned int>(paths.size());
    auto final_offset = group->GetTransform()->GetPosition() + shape_offset; 

    auto it = std::find(shape_groups.begin(), shape_groups.end(), group);
    auto group_index = static_cast<unsigned int>(it - shape_groups.begin());

    for(auto& path : paths){
      auto it = std::find(paths.begin(), paths.end(), path);
      auto path_index = static_cast<unsigned int>(it - paths.begin());

      BezierVertData signal_path_data;
      signal_path_data.group_ind = group_index;
      signal_path_data.path_ind = path_index;

      auto bezier_verts = path->GetBezierVertices();

      for(auto& vert : bezier_verts){
        auto tmp_vert = Normalize<glm::vec2>(vert + glm::vec2(final_offset.x, final_offset.y));
        signal_path_data.verts.emplace_back(tmp_vert.x);
        signal_path_data.verts.emplace_back(tmp_vert.y);
        signal_path_data.verts.emplace_back(0);
      }

      if(path->IsClosed())
        signal_path_data.tri_index = path->GetTriIndexList();

      if (path->HasKeyframe()) {
        auto linear_map = path->GetLinearMap();

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
      }

      if (path->IsClosed() && path->HasKeyframe()) {
        signal_path_data.linear_trig = path->GetTrigIndexMap();
      }


      bezier_vert_data_.emplace_back(signal_path_data);
    }
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

bool VerticesRenderData::GetVertices(unsigned int group_ind, unsigned int path_ind, std::vector<float>& verts){
  auto it = std::find_if(bezier_vert_data_.begin(), bezier_vert_data_.end(), [&](BezierVertData data){
    return (group_ind == data.group_ind && path_ind == data.path_ind);
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

bool VerticesRenderData::GetTriangleIndex(unsigned int group_ind, unsigned int path_ind, std::vector<unsigned int>& trigs) {
  auto it = std::find_if(bezier_vert_data_.begin(), bezier_vert_data_.end(), [&](BezierVertData data) {
    return (group_ind == data.group_ind && path_ind == data.path_ind);
  });
  if (it == bezier_vert_data_.end()) return false;
  trigs = bezier_vert_data_[it - bezier_vert_data_.begin()].tri_index;
  return true;
}


}