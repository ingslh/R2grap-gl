#pragma once
#include <nlohmann/json.hpp>
#include <glm/glm.hpp>

namespace R2grap{
class FillInfo{
public:
  FillInfo(const nlohmann::json& json) :
    blend_mode_ (json["Blend Mode"]),
    composite_ (json["Composite"]),
    fill_rule_ (json["Fill Rule"]),
    opacity_ (json["Opacity"]),
    color_(glm::vec4(json["Color"][0], json["Color"][1], json["Color"][2], json["Color"][3]))
  {
  }

  const glm::vec4& GetColor() const{return color_;} 

private:
  unsigned int blend_mode_;
  unsigned int composite_;
  unsigned int fill_rule_;
  glm::vec4 color_;
  unsigned int opacity_;

};
}