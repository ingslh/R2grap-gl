#include "ColorRenderData.h"
#include "AniInfoManager.h"
#include "LayersInfo.h"
#include "FillInfo.h"

namespace R2grap{

ColorRenderData::ColorRenderData(const LayersInfo* data){
  fills_count_ = 0;
  auto shape_groups = data->GetShapeGroup();
  for(auto i = 0; i < shape_groups.size(); i++){
    std::vector<unsigned int> indexs = { (unsigned int)i };
    RecusCalcColorData(shape_groups[i], indexs);
  }
}

void ColorRenderData::RecusCalcColorData(const std::shared_ptr<ShapeGroup>& group, const std::vector<unsigned int>& indexs) {
  if (group->HasChildGroups()) {
    auto child_groups = group->GetChildGroups();
    for (auto i = 0; i < child_groups.size(); i++) {
      auto cur_inds = indexs;
      cur_inds.push_back(i);
      RecusCalcColorData(child_groups[i], cur_inds);
    }
  }
  else {
    MulitColorData color_data;
    GenerateColorCacheData(indexs, group, color_data);
    multi_color_data_.emplace_back(color_data);
  }
}

void ColorRenderData::GenerateColorCacheData(const std::vector<unsigned int>& indexs, const std::shared_ptr<ShapeGroup> group, MulitColorData& color_data) {
  auto content = group->GetContents();
  std::vector<ColorCacheData> color_cache_data;
  if (content->GetFills()) {
    auto fill_color = content->GetFills()->GetColor();
    auto fill_opacity = content->GetFills()->GetOpacity();

    ColorCacheData tmp_data(ColorDataType::t_cFill, fill_color, fill_opacity);
    auto keyframe_map = content->GetFills()->GetKeyframeData();
    for (auto& it : keyframe_map) {
      ProcessColorData(it, tmp_data);
    }
    color_cache_data.emplace_back(tmp_data);
  }
  else if (content->GetStroke()) {
    auto stroke_color = content->GetStroke()->GetColor();
    auto stroke_opacity = content->GetStroke()->GetOpacity();

    ColorCacheData tmp_data( ColorDataType::t_cStroke, stroke_color, stroke_opacity);
    tmp_data.stroke_wid = (float)content->GetStroke()->GetStrokeWidth();
    tmp_data.miter_limit = (float)content->GetStroke()->GetMiterLimit();
    auto keyframe_map = content->GetStroke()->GetKeyframeData();
    for (auto& it : keyframe_map) {
      ProcessColorData(it, tmp_data);
    }
    color_cache_data.emplace_back(tmp_data);
  }
  color_data = { indexs, color_cache_data };
}

void ColorRenderData::ProcessColorData(const KeyframePair& key_pair, ColorCacheData& color_data) {
  if (key_pair.first == "Color") {
    color_data.h_ckf = true;
    auto color_keyframes = std::get<VectorKeyFrames>(key_pair.second);
    for (auto& el : color_keyframes) {
      auto v4lastvalue = glm::vec4(el.lastkeyValue.x, el.lastkeyValue.y, el.lastkeyValue.z, 1);
      std::vector<glm::vec4> v4lastvalue_list({ v4lastvalue });
      auto v4curvalue = glm::vec4(el.keyValue.x, el.keyValue.y, el.keyValue.z, 1);
      std::vector<glm::vec4> v4curvalue_list({ v4curvalue });
      auto linear_data = std::make_shared<LinearGenerator<glm::vec4>>(v4lastvalue_list, el.lastkeyTime, v4curvalue_list, el.keyTime);
      if (!linear_data) continue;
      color_data.trans_color.insert(linear_data->GetLinearMapToSignal().begin(), linear_data->GetLinearMapToSignal().end());
    }
  }
  else if (key_pair.first == "Opacity") {
    color_data.h_okf = true;
    auto opacity_keyframe = std::get<ScalarKeyFrames>(key_pair.second);
    for (auto& el : opacity_keyframe) {
      std::vector<float> lastval_list({ el.lastkeyValue }), curval_list({ el.keyValue });
      auto linear_data = std::make_shared<LinearGenerator<float>>(lastval_list, el.lastkeyTime, curval_list, el.keyTime);
      if (!linear_data) continue;
      color_data.trans_opacity.insert(linear_data->GetLinearMapToSignal().begin(), linear_data->GetLinearMapToSignal().end());
    }
  }
  else if (key_pair.first == "Stroke Width") {
    color_data.h_skf = true;
    auto strokewid_keyframe = std::get<ScalarKeyFrames>(key_pair.second);
    for (auto& el : strokewid_keyframe) {
      std::vector<float> lastval_list({ el.lastkeyValue }), curval_list({ el.keyValue });
      auto linear_data = std::make_shared<LinearGenerator<float>>(lastval_list, el.lastkeyTime, curval_list, el.keyTime);
      if (!linear_data) continue;
      auto tmp_map = linear_data->GetLinearMapToSignal();
      color_data.trans_stroke_wid.insert(tmp_map.begin(), tmp_map.end());
    }
  }
}

std::vector<ColorCacheData> ColorRenderData::GetColor(std::vector<unsigned int>& indexs)const {
  auto it = std::find_if(multi_color_data_.begin(), multi_color_data_.end(), [&](const MulitColorData& cache) {
    return cache.group_indexs == indexs;
  });
  if (it != multi_color_data_.end())
    return it->color_cache_data;
  else
    return {};
}

}