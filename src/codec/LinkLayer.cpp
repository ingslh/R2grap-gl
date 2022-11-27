#include "LinkLayer.h"

namespace R2grap{

LinkLayer::LinkLayer(JsonReader* reader): reader_(reader){
  auto layers_num = reader_->GetLayersNum();
  for(auto i =0; i< layers_num; i++){
    auto layer = reader_->GetLayersInfo(i);
    std::vector<unsigned int> ite_links;
    while(layer->GetLinkInd() != -1){
      auto link_ind = layer->GetLinkInd() - 1;
      ite_links.emplace_back(link_ind);
      layer = reader_->GetLayersInfo(link_ind);
    }
    layers_link_map_[i] = ite_links;
  }
}

//link parent's property(position、scale、rotation)
void LinkLayer::UpdatePropertyByLink(unsigned int ind){
  auto links_ind = layers_link_map_[ind];
  auto pos_offset = glm::vec3(0,0,0), scale = glm::vec3(1,1,1);
  float opacity = 1, rotation = 0 ;
  auto cur_transform = reader_->GetLayersInfo(ind)->GetShapeTransform();
  for(auto it = links_ind.rbegin(); it != links_ind.rend(); it++){
    auto link_transform = reader_->GetLayersInfo(*it)->GetShapeTransform();
    //position
    auto temp_pos = link_transform->GetOrigPosition() + pos_offset;
    pos_offset = temp_pos - link_transform->GetAnchorPos();
    //scale
    scale *= link_transform->GetScale()/glm::vec3(100);
    //rotation
    rotation += link_transform->GetRotation(); 
  }
  cur_transform->SetPosition(cur_transform->GetOrigPosition() + pos_offset);
  cur_transform->SetScale(scale * glm::vec3(100));
  cur_transform->SetRotation(rotation);
  AniInfoManager::GetIns().SetLayerTransform(ind, cur_transform);
}

}