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
    if(closed_){
      auto bezier_verts = std::make_shared<BezierGenerator>(vertices_, out_pos_vecs_, in_pos_vecs_);
      bezier_verts_ = bezier_verts -> getBezierVerts();
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
};

}