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
  bool h_ckf = false;
  std::map<unsigned int, glm::vec4> trans_color;

  unsigned int opacity;
  bool h_okf = false;
  std::map<unsigned int, unsigned int> trans_opacity;

  float stroke_wid;
  bool h_skf = false;
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
  const std::vector<ColorCacheData>& GetColor(unsigned int ind)const { return multi_color_data_.at(ind); }
private:
  unsigned int fills_count_;
  unsigned int stroke_count_;
  std::vector<glm::vec4> multi_fills_data_;

  std::map<unsigned int, std::vector<ColorCacheData>> multi_color_data_; //first: group index; second: fill or stroke
};

}