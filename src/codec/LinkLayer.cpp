#include "LinkLayer.h"

namespace R2grap{

LinkLayer::LinkLayer(JsonReader* reader): reader_(reader){
  auto layers_num = reader_->GetLayersNum();
  for(auto i =0; i< layers_num; i++){
    auto layer = reader_->GetLayersInfo(i);
    std::vector<unsigned int> ite_links;
    while(layer->GetLinkInd() != -1){
      auto link_ind = layer->GetLayerInd() - 1;
      ite_links.emplace_back(link_ind);
      layer = reader_->GetLayersInfo(link_ind);
    }
    layers_link_map_[i] = ite_links;
  }
}

void LinkLayer::UpdatePropertyByLink(JsonReader* reader, const std::shared_ptr<Transform>& transform, unsigned int ind){
  auto links_ind = layers_link_map_[ind];
  for(auto link_ind : links_ind){
    auto link_transform = reader_->GetLayersInfo(link_ind)->GetShapeTransform();
    auto pos_offset = link_transform->GetShapeGrapOffset();

    auto rotation = link_transform->GetRotation();


  }
}

}