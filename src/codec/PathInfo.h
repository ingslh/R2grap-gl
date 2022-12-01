#pragma once
#include <glm/glm.hpp>
#include <nlohmann/json.hpp>
#include <vector>
#include "Polygon.hpp"
#include "BezierGen.hpp"
#include "JsonConver.hpp"
#include "AniInfoManager.h"

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
        float last_time = round(float(el.value()["lastkeyTime"]) * 100) / 100 * frameRate;
        auto last_start_pos = el.value()["lastkeyValue"][0];
        auto last_end_pos = el.value()["lastkeyValue"][1];
        auto last_out_dir = el.value()["lastkeyOutPos"][0];
        auto last_in_dir = el.value()["lastkeyInPos"][1];
        BezierCluster last_curve_cluster{ jsonconver::json_to<vec2>(last_start_pos) , jsonconver::json_to<vec2>(last_out_dir),
                                        jsonconver::json_to<vec2>(last_in_dir) , jsonconver::json_to<vec2>(last_end_pos) };
        auto last_curve = std::make_shared<BezierGenerator>(last_curve_cluster)->getBezierVerts();
        
        float cur_time = round(float(el.value()["keyTime"]) * 100) / 100 * frameRate;
        auto cur_start_pos = el.value()["keyValue"][0]; 
        auto cur_end_pos = el.value()["keyValue"][1];
        auto cur_out_dir = el.value()["keyOutPos"][0];
        auto cur_in_dir = el.value()["keyInPos"][1];
        BezierCluster cur_curve_cluster{ jsonconver::json_to<vec2>(cur_start_pos), jsonconver::json_to<vec2>(cur_out_dir),
                                        jsonconver::json_to<vec2>(cur_in_dir), jsonconver::json_to<vec2>(cur_end_pos) };
        auto cur_curve = std::make_shared<BezierGenerator>(cur_curve_cluster)->getBezierVerts();

        auto linear_vert = std::make_shared<LinearGenerator<glm::vec2>>(last_curve, last_time, cur_curve, cur_time);
        if (linear_vert) {
          auto tmp_map = linear_vert->GetLinearMap();
          linear_map_.insert(tmp_map.begin(), tmp_map.end());
        }
      }
    }

    auto bezier_verts = std::make_shared<BezierGenerator>(vertices_, out_pos_vecs_, in_pos_vecs_ ,closed_);
    bezier_verts_ = bezier_verts -> getBezierVerts();
    if(closed_){
      auto polygon = std::make_shared<PolygonArray>(bezier_verts_);
      tri_index_list_ = mapbox::earcut<unsigned int>(polygon->getVertices());
    }

    if(closed_){
      for(auto& el : linear_map_){
        auto polygon = std::make_shared<PolygonArray>(el.second);
        auto tri_index_list = mapbox::earcut<unsigned int>(polygon->getVertices());
        tri_index_map_[el.first] = tri_index_list;
      }
    }
  }

  bool HasKeyframe() { return linear_map_.size(); }
  bool IsClosed() { return closed_; }

  const std::vector<glm::vec2>& GetOutPosVec() const {return out_pos_vecs_;}
  const std::vector<glm::vec2>& GetInPosVec() const {return in_pos_vecs_;}
  const std::vector<glm::vec2>& GetVertices() const {return vertices_;}
  const std::vector<glm::vec2>& GetBezierVertices() const {return bezier_verts_;}
  const std::vector<unsigned int>& GetTriIndexList() const {return tri_index_list_;}
  const std::map<unsigned int, std::vector<glm::vec2>>& GetLinearMap() const { return linear_map_; }
  const std::map<unsigned int, std::vector<unsigned int>>& GetTrigIndexMap() const { return tri_index_map_; }

private:
  unsigned int shape_direction_;
  bool closed_;
  std::vector<glm::vec2> vertices_;
  std::vector<glm::vec2> out_pos_vecs_;
  std::vector<glm::vec2> in_pos_vecs_;

  std::vector<glm::vec2> bezier_verts_;
  std::vector<unsigned int> tri_index_list_;

  std::map<unsigned int, std::vector<glm::vec2>> linear_map_; //first: frame index, second: array of vert
  std::map<unsigned int, std::vector<unsigned int>> tri_index_map_;//first: frame index, second: array of triangle index
};

}