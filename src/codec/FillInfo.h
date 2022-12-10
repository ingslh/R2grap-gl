#pragma once
#include "KeyframeGen.hpp"
#include <nlohmann/json.hpp>
#include <glm/glm.hpp>

namespace R2grap{
class FillInfo{
public:
  FillInfo(const nlohmann::json& json);

  unsigned int GetOpacity() const{return opacity_;}
  const glm::vec4& GetColor() const{return color_;} 
  const KeyframesMap& GetKeyframeData()const { return keyframe_data_; }

private:
  unsigned int blend_mode_;
  unsigned int composite_;
  unsigned int fill_rule_;
  glm::vec4 color_;
  unsigned int opacity_;

  KeyframesMap keyframe_data_;
  std::vector<std::string> keyframe_property_;
};
}