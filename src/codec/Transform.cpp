#include <glm/gtc/matrix_transform.hpp>
#include "Transform.h"
#include "AniInfoManager.h"

using namespace R2grap;

Transform::Transform(const nlohmann::json& transform, bool IsShapeTransform){
  for (auto& el : transform.items()) {
    readKeyframeandProperty(el.key(),el.value());
  }
  type_ = IsShapeTransform ? t_ShapeTrans : t_GroupTrans;
}

//note:not support anchor position's keyframe,can conver to position's keyframe
void Transform::readKeyframeandProperty(const std::string& propname, const nlohmann::json& transform){
  if (propname != "Anchor Point" && propname != "Position" && 
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
    if (keyvalue_is_vector)
      property_values_[propname] = glm::vec3(cur_property["Curve1"]["lastkeyValue"][0], cur_property["Curve1"]["lastkeyValue"][1], cur_property["Curve1"]["lastkeyValue"][2]);
    else
      property_values_[propname] = cur_property["Curve1"]["lastkeyValue"];
    VectorKeyFrames vector_keyframe;
    ScalarKeyFrames scalar_keyframe;
    auto frameRate = AniInfoManager::GetIns().GetFrameRate();
    for (auto it = cur_property.begin(); it != cur_property.end(); ++it){
      if (it.key().substr(0, 5) != "Curve") continue;
      auto lastkeyValue = it.value()["lastkeyValue"];
      float lastkeyTime = static_cast<float>(it.value()["lastkeyTime"]) * frameRate;

      auto keyValue = it.value()["keyValue"];
      float keyTime = static_cast<float>(it.value()["keyTime"]) * frameRate;

      if (keyvalue_is_vector){
        auto vector_lastkeyvalue = glm::vec3(lastkeyValue[0], lastkeyValue[1], lastkeyValue[2]);
        auto vector_keyvalue = glm::vec3(keyValue[0], keyValue[1], keyValue[2]);

        float out_x, in_x;
        glm::vec3 out_y, in_y;
        if (it.value()["OutPos"]["x"].is_number() && it.value()["OutPos"]["y"].is_number()) {
          out_x = (keyTime - lastkeyTime) * static_cast<float>(it.value()["OutPos"]["x"]) + lastkeyTime;
          out_y = (vector_keyvalue - vector_lastkeyvalue) * static_cast<float>(it.value()["OutPos"]["y"]) + vector_lastkeyvalue;
          in_x = (keyTime - lastkeyTime) * static_cast<float>(it.value()["InPos"]["x"]) + lastkeyTime;
          in_y = (vector_keyvalue - vector_lastkeyvalue) * static_cast<float>(it.value()["InPos"]["y"]) + vector_lastkeyvalue;
        }
        else {
          out_x = (keyTime - lastkeyTime) * static_cast<float>(it.value()["OutPos"]["x"][0]) + lastkeyTime;
          out_y = (vector_keyvalue - vector_lastkeyvalue) * static_cast<float>(it.value()["OutPos"]["y"][0]) + vector_lastkeyvalue;
          in_x = (keyTime - lastkeyTime) * static_cast<float>(it.value()["InPos"]["x"][0]) + lastkeyTime;
          in_y = (vector_keyvalue - vector_lastkeyvalue) * static_cast<float>(it.value()["InPos"]["y"][0]) + vector_lastkeyvalue;
        }

        std::vector<glm::vec2> outpos_list, inpos_list;
        outpos_list.emplace_back(glm::vec2(out_x, out_y.x));
        outpos_list.emplace_back(glm::vec2(out_x, out_y.y));
        inpos_list.emplace_back(glm::vec2(in_x, in_y.x));
        inpos_list.emplace_back(glm::vec2(in_x, in_y.y));

        vector_keyframe.emplace_back(Keyframe<glm::vec3>(vector_lastkeyvalue, lastkeyTime, outpos_list, inpos_list, vector_keyvalue, keyTime));
      }
      else{
        float scalar_lastkeyvalue = lastkeyValue;
        float scalar_keyvalue = keyValue;

        float out_x = (keyTime - lastkeyTime) * static_cast<float>(it.value()["OutPos"]["x"][0]) + lastkeyTime;
        auto out_y = (scalar_keyvalue - scalar_lastkeyvalue) * static_cast<float>(it.value()["OutPos"]["y"][0]) + scalar_lastkeyvalue;

        float in_x = (keyTime - lastkeyTime) * static_cast<float>(it.value()["InPos"]["x"][0]) + lastkeyTime;
        auto in_y = (scalar_keyvalue - scalar_lastkeyvalue) * static_cast<float>(it.value()["InPos"]["y"][0]) + scalar_lastkeyvalue;

        std::vector<glm::vec2> outpos_list, inpos_list;
        outpos_list.emplace_back(glm::vec2(out_x, out_y));
        inpos_list.emplace_back(glm::vec2(in_x, in_y));

        scalar_keyframe.emplace_back(Keyframe<float>(scalar_lastkeyvalue, lastkeyTime, outpos_list, inpos_list, scalar_keyvalue, keyTime));
      }
    }
    if (keyvalue_is_vector)
      keyframe_data_[propname] = vector_keyframe;
    else
      keyframe_data_[propname] = scalar_keyframe;
  }
}

//note: group's transform vector is 2D, and shape's transform vector is 3D
glm::vec3 Transform::GetShapeGrapOffset(){
  return std::get<t_Vector>(property_values_["Position"]) - std::get<t_Vector>(property_values_["Anchor Point"]);
}

bool Transform::IsVectorProperty(std::string str) {
  return (str == "Anchor Point" || str == "Position" || str == "Scale");
}
