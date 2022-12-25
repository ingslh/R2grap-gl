#pragma once
#include "Transform.h"
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

  ColorCacheData(ColorDataType type, glm::vec4 color, unsigned int opacity)
  :type(type), color(color), opacity(opacity)
  {}

  ColorCacheData() :type(ColorDataType::t_cFill), color(glm::vec4()), opacity(100)
  {}
};

struct MulitColorData {
  std::vector<unsigned int> group_indexs;
  std::vector<ColorCacheData> color_cache_data;
};

class ShapeGroup;
class LayersInfo;
class ColorRenderData : public R2grap::BaseRenderData{
public:
  ColorRenderData(const LayersInfo* layer);
  std::vector<ColorCacheData> GetColor(std::vector<unsigned int>& indexs)const;

private:
  void GenerateColorCacheData(const std::vector<unsigned int>& indexs, const std::shared_ptr<ShapeGroup> group, MulitColorData& color_data);
  void RecusCalcColorData(const std::shared_ptr<ShapeGroup>& group, const std::vector<unsigned int>& indexs);
  void ProcessColorData(const KeyframePair& key_pair, ColorCacheData& color_data);

  unsigned int fills_count_;
  unsigned int stroke_count_;

  std::vector<MulitColorData> multi_color_data_;
};

}