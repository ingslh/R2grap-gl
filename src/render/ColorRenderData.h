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

struct MulitColorData {
  int first_ind;  //parent index
  int second_ind; //child index
  std::vector<ColorCacheData> color_cache_data;
};

class ColorRenderData : public R2grap::BaseRenderData{
public:
  ColorRenderData(const LayersInfo* layer);
  const std::vector<ColorCacheData>& GetColor(unsigned int parent_ind, unsigned int child_ind)const;
private:
  void GenerateColorCacheData(const unsigned int ind, const std::shared_ptr<GroupContents> content, std::vector<ColorCacheData>& color_cache_list);

  unsigned int fills_count_;
  unsigned int stroke_count_;

  std::map<unsigned int, std::vector<ColorCacheData>> multi_color_data_; //first: group index; second: fill or stroke

  std::vector<MulitColorData> multi_color_data;
};

}