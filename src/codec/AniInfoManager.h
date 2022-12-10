#pragma once
#include <nlohmann/json.hpp>
#include <glm/glm.hpp>
#include "PathRenderData.h"


namespace R2grap{

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
	void SetGroupsTransform(unsigned int lay_ind, const std::vector<unsigned int>& group_inds, std::shared_ptr<Transform> transform);
  void AppendLayerInandOut(unsigned int ind, unsigned int in, unsigned int out);

	void SetRenderPathObjs(const std::vector<RePathObj>& objs){render_path_objs_ = objs;}
	const std::vector<RePathObj>& GetRenderPathObjs()const {return render_path_objs_;}

  glm::vec2 GetTransPos(unsigned int ind)const {return layers_transform_map_.at(ind).position;}
	glm::vec2 GetTransAncPos(unsigned int ind) const{return layers_transform_map_.at(ind).anchor_pos;}

  unsigned int GetWidth(){return width_;}
  unsigned int GetHeight(){return height_;}
  unsigned int GetFrameRate(){return frame_rate_;}
  unsigned int GetDuration(){return duration_;}
  unsigned int GetLayersNum(){return layers_num_;}
  std::map<unsigned int, std::vector<unsigned int>> GetLayersLinkMap(){return layers_link_map_;}
  void GetLayerInandOutPos(unsigned int ind, unsigned int& in, unsigned int& out);

private:
	struct TransformPorperty{
			glm::vec2 anchor_pos;
			glm::vec2 position;
			glm::vec2 scale;
			float rotation;
			float opacity;
	};

	struct GroupTrans{
			unsigned int layer_ind;
			std::vector<unsigned int> group_inds;
			TransformPorperty property;
	};

private:
  unsigned int width_;
  unsigned int height_;
  unsigned int frame_rate_;
  unsigned int duration_;
  unsigned int layers_num_;
  std::map<unsigned int, std::vector<unsigned int>> layers_link_map_;
  std::map<unsigned int, TransformPorperty> layers_transform_map_;
  std::map<unsigned int, std::pair<unsigned int, unsigned int>> layers_inout_map_;
	std::vector<GroupTrans> groups_transform_list_;
	std::vector<RePathObj> render_path_objs_;
};
}