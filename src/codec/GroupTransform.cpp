#include "GroupTransform.h"

namespace R2grap{

GroupTransform::GroupTransform(const nlohmann::json& transform):Transform(){
  
}

void GroupTransform::readKeyframeandProperty(const std::string& propname, const nlohmann::json& transform){
  if(propname != "Anchor Point" && propname != "Position" && propname != "Scale" &&
     propname != "Rotation" && propname != "Opacity" && propname != "Skew" && propname != "Skew Axis")
    return;

  bool keyvalue_is_vector = IsVectorProperty(propname);
}

}