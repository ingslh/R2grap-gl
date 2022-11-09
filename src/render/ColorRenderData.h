#pragma once
#include "BaseRenderData.h"
#include "LayersInfo.h"
#include <vector>
#include <glm/glm.hpp>

namespace R2grap{
  
class ColorRenderData : public R2grap::BaseRenderData{
public:
  ColorRenderData(const LayersInfo* layer);
  glm::vec4 GetColor(unsigned int ind) { return multi_fills_data_[ind]; }
private:
  unsigned int fills_count_;
  std::vector<glm::vec4> multi_fills_data_;
};

}