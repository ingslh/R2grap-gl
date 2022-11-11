#include "ColorRenderData.h"
#include "AniInfoManager.h"
#include "LayersInfo.h"
#include "FillInfo.h"

namespace R2grap{
  ColorRenderData::ColorRenderData(const LayersInfo* data){
  fills_count_ = 0;
  auto shape_groups = data->GetShapeGroup();
  for(auto& group : shape_groups){
    auto contents = group->GetContents();
    if(contents->GetFills()){
      fills_count_ += 1;
      auto fill_color = contents->GetFills()->GetColor();
      multi_fills_data_.emplace_back(fill_color);
    }else if(contents->GetStroke()){
      stroke_count_ += 1;
      auto stroke_color = contents->GetStroke()->GetColor();
    }


  }
}
}