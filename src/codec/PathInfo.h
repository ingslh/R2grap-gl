#pragma once
#include <glm/glm.hpp>
#include <nlohmann/json.hpp>
#include <vector>
#include "Polygon.hpp"
#include "BezierGen.hpp"

namespace R2grap{
class PathInfo{
public:
  PathInfo(const nlohmann::json& json): shape_direction_(json["Shape Direction"]), closed_(json["Closed"])
  {
    auto outpos_list = json["OutPos"];
    auto inpos_list = json["InPos"];
    auto vert_list = json["Vertices"];
    for(auto ind = 0; ind < outpos_list.size(); ind++){
      out_pos_vecs_.emplace_back(glm::vec2(outpos_list[ind][0], outpos_list[ind][1]));
      in_pos_vecs_.emplace_back(glm::vec2(inpos_list[ind][0], inpos_list[ind][1]));
      vertices_.emplace_back(glm::vec2(vert_list[ind][0], vert_list[ind][1]));
    }

    std::map<unsigned int, std::vector<glm::vec2>> keyframe_verts;
    if(json.count("Path") && json["Path"].is_object()){//exist path keyframe 
      auto curves = json["Path"];
      auto frameRate = AniInfoManager::GetIns().GetFrameRate();
      for(auto& el : curves.items()){
        auto last_verts = el.value()["lastkeyValue"];
        float last_time = round(float(el.value()["lastkeyTime"]) * 100)/100 * frameRate;
        auto cur_verts = el.value()["keyValue"];
        float cur_time = round(float(el.value()["keyTime"])*100)/100 * frameRate; 
        std::vector<glm::vec2> vec_last_verts, vec_cur_verts;
        for(auto i =0 ; i < last_verts.size(); i++){
          vec_last_verts.emplace_back(glm::vec2(last_verts[i][0], last_verts[i][1]));
          vec_cur_verts.emplace_back(glm::vec2(cur_verts[i][0], cur_verts[i][1]));
        }
        auto linear_vert = std::make_shared<LinearGenerator>(vec_last_verts, last_time, vec_cur_verts, cur_time);
        if (linear_vert) {
          auto tmp_map = linear_vert->GetLinearMap();
          linear_map_.insert(tmp_map.begin(), tmp_map.end());
        }
      }
    }

    auto bezier_verts = std::make_shared<BezierGenerator>(vertices_, out_pos_vecs_, in_pos_vecs_ ,closed_);
    bezier_verts_ = bezier_verts -> getBezierVerts();
    if(closed_){
      auto polygon = std::make_shared<PolygonArray>(bezier_verts->getBezierVerts());
      tri_index_list_ = mapbox::earcut<unsigned int>(polygon->getVertices());
    }
  }

  const std::vector<glm::vec2>& GetOutPosVec() const {return out_pos_vecs_;}
  const std::vector<glm::vec2>& GetInPosVec() const {return in_pos_vecs_;}
  const std::vector<glm::vec2>& GetVertices() const {return vertices_;}
  const std::vector<glm::vec2>& GetBezierVertices() const {return bezier_verts_;}
  const std::vector<unsigned int>& GetTriIndexList() const {return tri_index_list_;}

private:
  unsigned int shape_direction_;
  bool closed_;
  std::vector<glm::vec2> vertices_;
  std::vector<glm::vec2> out_pos_vecs_;
  std::vector<glm::vec2> in_pos_vecs_;

  std::vector<glm::vec2> bezier_verts_;
  std::vector<unsigned int> tri_index_list_;

  std::map<unsigned int, std::vector<glm::vec2>> linear_map_;
};

}