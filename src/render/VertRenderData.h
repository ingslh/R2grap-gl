#pragma once
#include "BaseRenderData.h"
#include "LayersInfo.h"
#include "PathInfo.h"
#include "BezierGen.hpp"
#include <glm/glm.hpp>
#include <vector>

namespace R2grap{

struct BezierVertData{

  std::vector<unsigned int> group_indexs;
  unsigned int path_ind;

  bool closed;
  std::vector<float> verts;//3-dim
  std::vector<unsigned int> tri_index;//3 int make up a triangle

  std::map<unsigned int, std::vector<float>> linear_verts;
  std::map<unsigned int, std::vector<unsigned int>> linear_trig;
};

class LayersInfo;
class ShapeGroup;
class VerticesRenderData : public BaseRenderData{
public:
  explicit VerticesRenderData(const LayersInfo* layer);
  bool GetVertices(unsigned int ind_path, std::vector<float>& vert_info);
  bool GetVertices(const std::vector<unsigned int>& indexs ,unsigned int path_ind, std::vector<float>& vert_info);
  bool GetTriangleIndex(unsigned int ind, std::vector<unsigned int>& trigs);
  bool GetTriangleIndex(const std::vector<unsigned int>& indexs, unsigned int path_ind, std::vector<unsigned int>& trigs);
  bool GetBezierVertData(const std::vector<unsigned int>& indexs, unsigned int path_ind, BezierVertData& vert_data);

  //unsigned int GetPathsCount() const { return paths_count_; }
  template<typename T>
  T Normalize(const T& pos); 

private:
  void GenerateVertCacheData(const std::vector<unsigned int>& indexs, const std::shared_ptr<ShapeGroup>& group, glm::vec3 parent_offset,std::vector<BezierVertData>& vert_data);
  void RecusCalcBezierVertData(const std::shared_ptr<ShapeGroup>& group, std::vector<unsigned int> indexs, glm::vec3 parent_offset);

private:
  std::vector<BezierVertData> bezier_vert_data_;//Index is group index

  unsigned int layer_ind_;
};

}