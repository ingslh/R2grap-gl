#include "AniInfoManager.h"
#include "Transform.h"

namespace R2grap{
  
  void AniInfoManager:: SetLayerTransform(unsigned int ind, std::shared_ptr<Transform> transform){
    TransformPorperty property;
    property.position = glm::vec2(transform->GetPosition().x, transform->GetPosition().y);
    property.scale = glm::vec2(transform->GetScale().x, transform->GetScale().y);
    property.rotation = transform->GetRotation();
    property.opacity = transform->GetOpacity();
    layers_transform_map_[ind] = property;
  } 

  void AniInfoManager::SetBasicInfo(nlohmann::json& json) {
    width_ = json["width"].get<unsigned int>();
    height_ = json["height"].get<unsigned int>();
    frame_rate_ = json["frameRate"].get<unsigned int>();
    duration_ = json["duration"].get<unsigned int>();
    layers_num_ = json["numLayers"].get<unsigned int>();
  }

  void AniInfoManager::SetLayersLinkMap(const std::map<unsigned int, std::vector<unsigned int>>& link_map) {
    layers_link_map_ = link_map;
  }

  void AniInfoManager::AppendLayerInandOut(unsigned int ind, unsigned int in, unsigned int out) {
    layers_inout_map_[ind] = std::make_pair(in,out);
  }

  void AniInfoManager::GetLayerInandOutPos(const unsigned int ind, unsigned int& in, unsigned int& out) {
    in = layers_inout_map_[ind].first * frame_rate_;
    out = layers_inout_map_[ind].second * frame_rate_;
  }


}