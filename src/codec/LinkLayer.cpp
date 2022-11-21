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
  auto pos_offset = glm::vec3(0,0,0);
  auto cur_transform = reader_->GetLayersInfo(ind)->GetShapeTransform();
  for(auto it = links_ind.rbegin(); it != links_ind.rend(); it++){
    auto link_transform = reader_->GetLayersInfo(*it)->GetShapeTransform();
    //position
    auto temp_pos = link_transform->GetPosition() + pos_offset;
    pos_offset = temp_pos - link_transform->GetAnchorPos();
    //scale

    //rotation
    
    //opacity

  }
  cur_transform->SetPosition(cur_transform->GetPosition() + pos_offset);
}

}