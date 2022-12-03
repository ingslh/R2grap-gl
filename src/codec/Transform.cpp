#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include "Transform.h"
#include "KeyframeGen.hpp"
#include "AniInfoManager.h"

using namespace R2grap;

Transform::Transform(const nlohmann::json& transform, bool IsShapeTransform, int link){
  type_ = IsShapeTransform ? t_ShapeTrans : t_GroupTrans;
  std::vector<std::string> include_propname  = {"Anchor Point","Position","Scale","Rotation","Opacity"};
  if(type_ == t_GroupTrans){
    include_propname.emplace_back("Skew");
    include_propname.emplace_back("Skew Axis");
  }
  auto PropertyType = [&](const std::string& proname)-> DimensionType {
    auto it = std::find(include_propname.begin(), include_propname.end(), proname);
    if(it == include_propname.end())
      return DimensionType::t_NoFind;
    else if(it - include_propname.begin() > 2)
      return DimensionType::t_Scalar;
    else
      return DimensionType::t_Vector;
  };

  for (auto& el : transform.items()) {
    ReadProperty(el.key(), el.value());
    
    KeyframeGen keyframegen(el.key(),el.value(), PropertyType);
    auto pair = keyframegen.GetKeyframePair();
    if(pair != nullptr){
      keyframe_data_[pair->first] = pair->second;
    }
  }
	SetOriginalProerty();
}

//note:not support anchor position's keyframe,can conver to position's keyframe
void Transform::ReadProperty(const std::string& propname, const nlohmann::json& transform){
  if (propname != "Anchor Point" && propname != "Position" && propname != "Skew" && propname != "Skew Axis" &&
      propname != "Scale" && propname != "Rotation" && propname != "Opacity") 
    return;
  bool keyvalue_is_vector = IsVectorProperty(propname);
  auto cur_property = transform;
  if (cur_property.is_number()) { // not have keyframe
    property_values_[propname] = cur_property;
  }
  else if (cur_property.is_array()) {
    if(cur_property.size()==3)
      property_values_[propname] = glm::vec3(cur_property[0], cur_property[1], cur_property[2]);
    else
      property_values_[propname] = glm::vec3(cur_property[0], cur_property[1], 0);
  }
  else if(cur_property.is_object()){//have keyframe ,it's a object
    if (keyvalue_is_vector) {
      if(cur_property["Curve1"]["lastkeyValue"].size() == 3)
        property_values_[propname] = glm::vec3(cur_property["Curve1"]["lastkeyValue"][0], cur_property["Curve1"]["lastkeyValue"][1], cur_property["Curve1"]["lastkeyValue"][2]);
      else
        property_values_[propname] = glm::vec3(cur_property["Curve1"]["lastkeyValue"][0], cur_property["Curve1"]["lastkeyValue"][1], 0);
    }
    else
      property_values_[propname] = cur_property["Curve1"]["lastkeyValue"];
  }
}

//note: group's transform vector is 2D, and shape's transform vector is 3D
glm::vec3 Transform::GetShapeGrapOffset(){
  return std::get<t_Vector>(property_values_["Position"]) - std::get<t_Vector>(property_values_["Anchor Point"]);
}

bool Transform::IsVectorProperty(std::string str) {
  return (str == "Anchor Point" || str == "Position" || str == "Scale");
}
