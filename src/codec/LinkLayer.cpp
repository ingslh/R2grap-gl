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
  }
}

void LinkLayer::UpdatePropertyByLink(JsonReader* reader, const std::shared_ptr<Transform>& transform, unsigned int link){
  if(transform->type() != TransformType::t_ShapeTrans || link < 1) return;

  auto link_layer = reader->GetLayersInfo(link - 1);
  auto link_transform = link_layer->GetShapeTransform();

  //set new property
  auto pos_offset = link_transform->GetShapeGrapOffset();
  transform->SetPosition(transform->GetPosition() + pos_offset);
  transform->SetScale(link_transform->GetScale());
  transform->SetRotation(link_transform->GetRotation());
}

}