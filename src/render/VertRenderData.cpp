#include "VertRenderData.h"
#include "AniInfoManger.h"
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

    for(auto& path : paths){
      multi_paths_data_.clear();
      BezierVertData signal_path_data;

      auto bezier_verts = path->GetBezierVertices();
      auto bezier_verts_count = bezier_verts.size();

      for(auto i = 0; i < bezier_verts_count; i++){
        auto tmp_vert = Normalize<glm::vec2>(bezier_verts[i] + glm::vec2(final_offset.x, final_offset.y));
        signal_path_data.verts.emplace_back(tmp_vert.x);
        signal_path_data.verts.emplace_back(tmp_vert.y);
        signal_path_data.verts.emplace_back(0);
      }
      signal_path_data.tri_index = path->GetTriIndexList();
      bezier_vert_data_.emplace_back(signal_path_data);
    }
  }
}


}