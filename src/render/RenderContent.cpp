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

  auto path_count = layer_contents_path->GetPathsCount();
  layer_data_.paths_num = path_count;
  for(unsigned int i = 0; i < path_count; i++){
    std::vector<float> vert;
    layer_contents_path->ConverToOpenglVert(i, vert);
    layer_data_.verts.emplace_back(vert);
    layer_data_.triangle_ind.emplace_back(layer_contents_path->GetTriangleIndex(i));
    layer_data_.color.emplace_back(layer_contents_color->GetColor(i));
  }
}

unsigned int RenderContent::GetRenderPathCount(const std::vector<std::shared_ptr<RenderContent>>& contents) {
  unsigned int count = 0;
  for (auto content : contents) {
    count += content->GetLayerData().paths_num;
  }
  return count;
}

unsigned int RenderContent::GetPathIndex(const std::vector<std::shared_ptr<RenderContent>>& contents, unsigned int layer_ind, unsigned int path_ind) {
  unsigned int index = 0;
  if (layer_ind == 0)
    return path_ind;
  else {
    for (auto i = 0; i <= layer_ind - 1; i++) {
      index += contents[i]->GetLayerData().paths_num;
    }
    return index + path_ind;
  }
}

}