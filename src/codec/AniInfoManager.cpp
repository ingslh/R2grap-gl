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

}