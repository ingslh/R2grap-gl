#include "ColorRenderData.h"
#include "AniInfoManager.h"
#include "LayersInfo.h"
#include "FillInfo.h"

namespace R2grap{

ColorRenderData::ColorRenderData(const LayersInfo* data){
  fills_count_ = 0;
  auto shape_groups = data->GetShapeGroup();
  for(auto i = 0; i < shape_groups.size(); i++){
    if (!shape_groups[i]->HasChildGroups()) {
      std::vector<ColorCacheData> color_cache;
      GenerateColorCacheData(i, shape_groups[i]->GetContents(), color_cache);
      multi_color_data.push_back({i, 0, color_cache });
    }
    else {
      auto child_groups = shape_groups[i]->GetChildGroups();
      for (auto j = 0; j < child_groups.size(); j++) {
        std::vector<ColorCacheData> color_cache;
        auto child_group = child_groups[j];
        GenerateColorCacheData(j, child_group->GetContents(), color_cache);
        multi_color_data.push_back({ i, j, color_cache });
      }
    }
  }
}

void ColorRenderData::GenerateColorCacheData(const unsigned int ind, const std::shared_ptr<GroupContents> content, std::vector<ColorCacheData>& color_cache_list) {
  if (content->GetFills()) {
    auto fill_color = content->GetFills()->GetColor();
    auto fill_opacity = content->GetFills()->GetOpacity();

    ColorCacheData tmp_data(ind, ColorDataType::t_cFill, fill_color, fill_opacity);
    auto keyframe_map = content->GetFills()->GetKeyframeData();
    for (auto& it : keyframe_map) {
      if (it.first == "Color") {
        tmp_data.h_ckf = true;
        auto color_keyframes = std::get<VectorKeyFrames>(it.second);
        for (auto& el : color_keyframes) {
          auto v4lastvalue = glm::vec4(el.lastkeyValue.x, el.lastkeyValue.y, el.lastkeyValue.z, 1);
          std::vector<glm::vec4> v4lastvalue_list({ v4lastvalue });
          auto v4curvalue = glm::vec4(el.keyValue.x, el.keyValue.y, el.keyValue.z, 1);
          std::vector<glm::vec4> v4curvalue_list({ v4curvalue });
          auto linear_data = std::make_shared<LinearGenerator<glm::vec4>>(v4lastvalue_list, el.lastkeyTime, v4curvalue_list, el.keyTime);
          if (!linear_data) continue;
          auto tmp_map = linear_data->GetLinearMapToSignal();
          tmp_data.trans_color.insert(tmp_map.begin(), tmp_map.end());
        }
      }
      else if (it.first == "Opacity") {
        tmp_data.h_okf = true;
        auto opacity_keyframe = std::get<ScalarKeyFrames>(it.second);
        for (auto& el : opacity_keyframe) {
          std::vector<float> lastval_list({ el.lastkeyValue }), curval_list({ el.keyValue });
          auto linear_data = std::make_shared<LinearGenerator<float>>(lastval_list, el.lastkeyTime, curval_list, el.keyTime);
          if (!linear_data) continue;
          auto tmp_map = linear_data->GetLinearMapToSignal();
          tmp_data.trans_opacity.insert(tmp_map.begin(), tmp_map.end());
        }
      }
    }
    color_cache_list.emplace_back(tmp_data);
  }
  else if (content->GetStroke()) {
    auto stroke_color = content->GetStroke()->GetColor();
    auto stroke_opacity = content->GetStroke()->GetOpacity();

    ColorCacheData tmp_data(ind, ColorDataType::t_cStroke, stroke_color, stroke_opacity);
    tmp_data.stroke_wid = content->GetStroke()->GetStrokeWidth();
    tmp_data.miter_limit = content->GetStroke()->GetMiterLimit();
    auto keyframe_map = content->GetStroke()->GetKeyframeData();
    for (auto& it : keyframe_map) {
      if (it.first == "Color") {
        tmp_data.h_ckf = true;
        auto color_keyframes = std::get<VectorKeyFrames>(it.second);
        for (auto& el : color_keyframes) {
          auto v4lastvalue = glm::vec4(el.lastkeyValue.x, el.lastkeyValue.y, el.lastkeyValue.z, 1);
          std::vector<glm::vec4> v4lastvalue_list({ v4lastvalue });
          auto v4curvalue = glm::vec4(el.keyValue.x, el.keyValue.y, el.keyValue.z, 1);
          std::vector<glm::vec4> v4curvalue_list({ v4curvalue });
          auto linear_data = std::make_shared<LinearGenerator<glm::vec4>>(v4lastvalue_list, el.lastkeyTime, v4curvalue_list, el.keyTime);
          if (!linear_data) continue;
          auto tmp_map = linear_data->GetLinearMapToSignal();
          tmp_data.trans_color.insert(tmp_map.begin(), tmp_map.end());
        }
      }
      else if (it.first == "Opacity") {
        tmp_data.h_okf = true;
        auto opacity_keyframe = std::get<ScalarKeyFrames>(it.second);
        for (auto& el : opacity_keyframe) {
          std::vector<float> lastval_list({ el.lastkeyValue }), curval_list({ el.keyValue });
          auto linear_data = std::make_shared<LinearGenerator<float>>(lastval_list, el.lastkeyTime, curval_list, el.keyTime);
          if (!linear_data) continue;
          auto tmp_map = linear_data->GetLinearMapToSignal();
          tmp_data.trans_opacity.insert(tmp_map.begin(), tmp_map.end());
        }
      }
      else if (it.first == "Stroke Width") {
        tmp_data.h_skf = true;
        auto strokewid_keyframe = std::get<ScalarKeyFrames>(it.second);
        for (auto& el : strokewid_keyframe) {
          std::vector<float> lastval_list({ el.lastkeyValue }), curval_list({ el.keyValue });
          auto linear_data = std::make_shared<LinearGenerator<float>>(lastval_list, el.lastkeyTime, curval_list, el.keyTime);
          if (!linear_data) continue;
          auto tmp_map = linear_data->GetLinearMapToSignal();
          tmp_data.trans_stroke_wid.insert(tmp_map.begin(), tmp_map.end());
        }
      }
    }
    color_cache_list.emplace_back(tmp_data);
  }
}

const std::vector<ColorCacheData>& ColorRenderData::GetColor(unsigned int parent_ind, unsigned int child_ind)const {
  auto it = std::find_if(multi_color_data.begin(), multi_color_data.end(), [&](const MulitColorData& cache) {
    return cache.first_ind == parent_ind && cache.second_ind == child_ind;
  });
  if (it != multi_color_data.end())
    return it->color_cache_data;
  else
    return std::move(std::vector<ColorCacheData>());
}

}