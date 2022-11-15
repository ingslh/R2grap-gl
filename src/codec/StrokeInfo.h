#pragma once
#include <nlohmann/json.hpp>
#include <glm/glm.hpp>

namespace R2grap{

class StrokeInfo{
public:
  StrokeInfo(const nlohmann::json& json):
    blend_mode_(json["Blend Mode"]),
    composite_(json["Composite"])
  {
    bool has_keyframe = false;
    if(json["Opacity"].is_number())
      opacity_ = json["Opacity"];
    else if(json["Opacity"].is_object())
      has_keyframe |= true;
    
    if(json["Stroke Width"].is_number())
      stroke_width_ = json["Stroke Width"];
    else if(json["Stroke Width"].is_object())
      has_keyframe |= true;

    if(json["Miter Limit"].is_number())
      miter_limit_ = json["Miter Limit"];
    else if(json["Miter Limit"].is_object())
      has_keyframe |= true;

    if(json["Color"].is_array())
      color_ = glm::vec4(json["Color"][0], json["Color"][1], json["Color"][2], json["Color"][3]);
    else if(json["Color"].is_object())
      has_keyframe |= true;

    //need to add other property

    if(!has_keyframe) return;
    std::vector<std::string> include_propname = {"Color", "Opacity", "Stroke Width", "Miter Limit"};
    auto PropertyType = [&](const std::string& proname)-> DimensionType {
      if(proname == "Color")
        return DimensionType::t_Vector;
      else if(proname == "Opacity" || proname == "Stroke Width" || proname == "Miter Limit")
        return DimensionType::t_Scalar;
      else
        return DimensionType::t_NoFind;
    };

    for(auto& el : json.items()){
      if(PropertyType(el.key()) == DimensionType::t_Vector){
        color_ = glm::vec4(el.value()["Curve1"]["lastkeyValue"][0], 
                           el.value()["Curve1"]["lastkeyValue"][1], 
                           el.value()["Curve1"]["lastkeyValue"][2],1);
      }
      else if(PropertyType(el.key()) == DimensionType::t_Scalar){
        opacity_ = el.value()["Curve1"]["lastkeyValue"];
      }
      KeyframeGen keyframegen(el.key(), el.value(),PropertyType);
      auto pair = keyframegen.GetKeyframePair();
      if(pair != nullptr)
        keyframe_data_[pair->first] = pair->second;
    }
  }

  unsigned int GetMiterLimit() const{return miter_limit_;}
  unsigned int GetStrokeWidth() const{return stroke_width_;}
  unsigned int GetOpacity() const{return opacity_;}
  const glm::vec4& GetColor() const{return color_;}
  const KeyframesMap& GetKeyframeData()const { return keyframe_data_; }

private:
  unsigned int blend_mode_;
  unsigned int composite_;
  unsigned int opacity_;
  glm::vec4 color_;
  unsigned int stroke_width_;
  unsigned int miter_limit_;

  KeyframesMap keyframe_data_;
};

}