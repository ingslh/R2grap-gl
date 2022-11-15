#pragma once
#include "BaseRenderData.h"
#include "LayersInfo.h"
#include <vector>
#include <glm/glm.hpp>

namespace R2grap{
  
enum ColorDataType{
  t_cFill = 0,
  t_cStroke,
};

struct ColorCacheData {
  unsigned int group_ind;
  ColorDataType type;

  glm::vec4 color;
  std::map<unsigned int, glm::vec4> trans_color;

  unsigned int opacity;
  std::map<unsigned int, unsigned int> trans_opacity;

  float stroke_wid;
  std::map<unsigned int, float> trans_stroke_wid;

  float miter_limit;
  std::map<unsigned int, float> trans_miter_limit;

  ColorCacheData(unsigned int ind, ColorDataType type, glm::vec4 color, unsigned int opacity)
  :group_ind(ind), type(type), color(color), opacity(opacity)
  {}

  ColorCacheData() :group_ind(0), type(ColorDataType::t_cFill), color(glm::vec4()), opacity(100)
  {}
};

class ColorRenderData : public R2grap::BaseRenderData{
public:
  ColorRenderData(const LayersInfo* layer);
  glm::vec4 GetColor(unsigned int ind) { return multi_fills_data_[ind]; }
private:
  unsigned int fills_count_;
  unsigned int stroke_count_;
  std::vector<glm::vec4> multi_fills_data_;

  std::map<unsigned int, std::vector<ColorCacheData>> multi_color_data_;
};

}