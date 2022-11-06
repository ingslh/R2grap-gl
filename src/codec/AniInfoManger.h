#pragma once
#include <nlohmann/json.hpp>

namespace R2grap{
class AniInfoManager{
public:
  static AniInfoManager& GetIns(){
    static AniInfoManager kManager;
    return kManager;
  }

  void SetBasicInfo(nlohmann::json& json){
    width_ = json["width"];
    height_ = json["height"];
    frame_rate_ = json["frameRate"];
    duration_ = json["duration"];
    layers_num_ = json["numLayers"];
  }

  unsigned int GetWidth()const {return width_;}
  unsigned int GetHeight()const {return height_;}
  unsigned int GetFrameRate()const {return frame_rate_;}
  unsigned int GetDuration()const {return duration_;}
  unsigned int GetLayersNum()const {return layers_num_;}

private:
  unsigned int width_;
  unsigned int height_;
  unsigned int frame_rate_;
  unsigned int duration_;
  unsigned int layers_num_;
};
}