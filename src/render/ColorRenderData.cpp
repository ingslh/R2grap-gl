#include "ColorRenderData.h"
#include "AniInfoManager.h"
#include "LayersInfo.h"
#include "FillInfo.h"

namespace R2grap{

ColorRenderData::ColorRenderData(const LayersInfo* data){
  fills_count_ = 0;
  auto shape_groups = data->GetShapeGroup();

  for(auto i = 0; i < shape_groups.size(); i++){
    auto contents = shape_groups[i]->GetContents();
    std::vector<ColorCacheData> color_cache_array;
    if(contents->GetFills()){
      auto fill_color = contents->GetFills()->GetColor();
      auto fill_opacity = contents->GetFills()->GetOpacity();

      ColorCacheData tmp_data(i, ColorDataType::t_cFill, fill_color, fill_opacity);
      auto keyframe_map = contents->GetFills()->GetKeyframeData();
      for(auto& it : keyframe_map){
        if(it.first == "Color"){
          auto color_keyframes = std::get<VectorKeyFrames>(it.second);
          for(auto& el : color_keyframes){
            auto v4lastvalue = glm::vec4(el.lastkeyValue.x, el.lastkeyValue.y, el.lastkeyValue.z, 1);
            std::vector<glm::vec4> v4lastvalue_list({ v4lastvalue });
            auto v4curvalue = glm::vec4(el.keyValue.x, el.keyValue.y, el.keyValue.z, 1);
            std::vector<glm::vec4> v4curvalue_list({ v4curvalue });
            auto linear_data = std::make_shared<LinearGenerator<glm::vec4>>(v4lastvalue_list, el.lastkeyTime, v4curvalue_list, el.keyTime);
            if(!linear_data) continue;
            auto tmp_map = linear_data->GetLinearMapToSignal();
            tmp_data.trans_color.insert(tmp_map.begin(), tmp_map.end());
          }
        }else if(it.first == "Opacity"){
          auto opacity_keyframe = std::get<ScalarKeyFrames>(it.second);
          for(auto& el:opacity_keyframe){
            std::vector<float> lastval_list({ el.lastkeyValue }), curval_list({ el.keyValue });
            auto linear_data = std::make_shared<LinearGenerator<float>>(lastval_list, el.lastkeyTime, curval_list, el.keyTime);
            if(!linear_data) continue;
            auto tmp_map = linear_data->GetLinearMapToSignal();
            tmp_data.trans_opacity.insert(tmp_map.begin(), tmp_map.end());
          }
        }
      }
      color_cache_array.emplace_back(tmp_data); 

    }else if(contents->GetStroke()){
      auto stroke_color = contents->GetStroke()->GetColor();
      auto stroke_opacity = contents->GetStroke()->GetOpacity();

      ColorCacheData tmp_data(i, ColorDataType::t_cStroke, stroke_color, stroke_opacity);
      tmp_data.stroke_wid = contents->GetStroke()->GetStrokeWidth();
      tmp_data.miter_limit = contents->GetStroke()->GetMiterLimit();
      auto keyframe_map = contents->GetStroke()->GetKeyframeData();
      for(auto& it: keyframe_map){
        if(it.first == "Color"){
          auto color_keyframes = std::get<VectorKeyFrames>(it.second);
          for(auto& el : color_keyframes){
            auto v4lastvalue = glm::vec4(el.lastkeyValue.x, el.lastkeyValue.y, el.lastkeyValue.z, 1);
            std::vector<glm::vec4> v4lastvalue_list({ v4lastvalue });
            auto v4curvalue = glm::vec4(el.keyValue.x, el.keyValue.y, el.keyValue.z, 1);
            std::vector<glm::vec4> v4curvalue_list({ v4curvalue });
            auto linear_data = std::make_shared<LinearGenerator<glm::vec4>>(v4lastvalue_list, el.lastkeyTime, v4curvalue_list, el.keyTime);
            if(!linear_data) continue;
            auto tmp_map = linear_data->GetLinearMapToSignal();
            tmp_data.trans_color.insert(tmp_map.begin(), tmp_map.end());
          }
        }else if(it.first == "Opacity"){
          auto opacity_keyframe = std::get<ScalarKeyFrames>(it.second);
          for(auto& el:opacity_keyframe){
            std::vector<float> lastval_list({ el.lastkeyValue }), curval_list({ el.keyValue });
            auto linear_data = std::make_shared<LinearGenerator<float>>(lastval_list, el.lastkeyTime, curval_list, el.keyTime);
            if(!linear_data) continue;
            auto tmp_map = linear_data->GetLinearMapToSignal();
            tmp_data.trans_opacity.insert(tmp_map.begin(), tmp_map.end());
          }
        }else if(it.first == "Stroke Width"){
          auto strokewid_keyframe = std::get<ScalarKeyFrames>(it.second);
          for(auto& el: strokewid_keyframe){
            std::vector<float> lastval_list({ el.lastkeyValue }), curval_list({ el.keyValue });
            auto linear_data = std::make_shared<LinearGenerator<float>>(lastval_list, el.lastkeyTime, curval_list, el.keyTime);
            if(!linear_data) continue;
            auto tmp_map = linear_data->GetLinearMapToSignal();
            tmp_data.trans_stroke_wid.insert(tmp_map.begin(), tmp_map.end());
          }
        }
      }
      color_cache_array.emplace_back(tmp_data); 
    }
    multi_color_data_[i] = color_cache_array;
  }






  /*for(auto& group : shape_groups){
    auto contents = group->GetContents();
    if(contents->GetFills()){
      fills_count_ += 1;
      auto fill_color = contents->GetFills()->GetColor();
      multi_fills_data_.emplace_back(fill_color);
    }else if(contents->GetStroke()){
      stroke_count_ += 1;
      auto stroke_color = contents->GetStroke()->GetColor();
    }
  }*/
}

}