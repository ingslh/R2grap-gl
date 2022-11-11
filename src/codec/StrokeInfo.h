#pragma once
#include <nlohmann/json.hpp>
#include <glm/glm.hpp>

namespace R2grap{

class StrokeInfo{
public:
  StrokeInfo(const nlohmann::json& json):
    blend_mode_(json["Blend Mode"]),
    composite_(json["Composite"]),
    opacity_ (json["Opacity"]),
    color_(glm::vec4(json["Color"][0], json["Color"][1], json["Color"][2], json["Color"][3]))
  {
  }

  const glm::vec4& GetColor() const{return color_;}

private:
  unsigned int blend_mode_;
  unsigned int composite_;
  unsigned int opacity_;
  glm::vec4 color_;
};

}