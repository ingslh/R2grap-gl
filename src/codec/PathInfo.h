#pragma once
#include <glm/glm.hpp>
#include <nlohmann/json.hpp>
#include <vector>
#include "Polygon.hpp"
#include "BezierGen.hpp"
#include "JsonConver.hpp"

namespace R2grap{
class PathInfo{
public:
  PathInfo(const nlohmann::json& json);

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