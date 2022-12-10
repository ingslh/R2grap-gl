#pragma once
#include <nlohmann/json.hpp>
#include <glm/glm.hpp>
#include "KeyframeGen.hpp"

namespace R2grap{

class StrokeInfo{
public:
  StrokeInfo(const nlohmann::json& json);

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