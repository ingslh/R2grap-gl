#pragma once
#include <nlohmann/json.hpp>
#include <glm/glm.hpp>

namespace R2grap{

struct TransformPorperty{
	glm::vec2 anchor_pos;
  glm::vec2 position;
  glm::vec2 scale;
  float rotation;
  float opacity;
};

class Transform;
class AniInfoManager{
public:
  static AniInfoManager& GetIns(){
    static AniInfoManager kManager;
    return kManager;
  }

  void SetBasicInfo(nlohmann::json& json);

  void SetLayersLinkMap(const std::map<unsigned int, std::vector<unsigned int>>& link_map);
  void SetLayerTransform(unsigned int ind, std::shared_ptr<Transform> transform);
  void AppendLayerInandOut(unsigned int ind, unsigned int in, unsigned int out);

  glm::vec2 GetTransPos(unsigned int ind)const {return layers_transform_map_.at(ind).position;}
	glm::vec2 GetTransAncPos(unsigned int ind) const{return layers_transform_map_.at(ind).anchor_pos;}

  unsigned int GetWidth()const {return width_;}
  unsigned int GetHeight()const {return height_;}
  unsigned int GetFrameRate()const {return frame_rate_;}
  unsigned int GetDuration()const {return duration_;}
  unsigned int GetLayersNum()const {return layers_num_;}
  std::map<unsigned int, std::vector<unsigned int>> GetLayersLinkMap() const{return layers_link_map_;}
  void GetLayerInandOutPos(const unsigned int ind, unsigned int& in, unsigned int& out);

private:
  unsigned int width_;
  unsigned int height_;
  unsigned int frame_rate_;
  unsigned int duration_;
  unsigned int layers_num_;
  std::map<unsigned int, std::vector<unsigned int>> layers_link_map_;
  std::map<unsigned int, TransformPorperty> layers_transform_map_;
  std::map<unsigned int, std::pair<unsigned int, unsigned int>> layers_inout_map_;
};
}