#pragma once
#include "BaseRenderData.h"
#include "LayersInfo.h"
#include "PathInfo.h"
#include "BezierGen.hpp"
#include <glm/glm.hpp>
#include <vector>

namespace R2grap{

struct BezierVertData{
  unsigned int p_group_ind;//parent
	unsigned int c_group_ind;//child
  unsigned int path_ind;

  bool closed;
  std::vector<float> verts;//3-dim
  std::vector<unsigned int> tri_index;//3 int make up a triangle

  std::map<unsigned int, std::vector<float>> linear_verts;
  std::map<unsigned int, std::vector<unsigned int>> linear_trig;
};

class VerticesRenderData : public BaseRenderData{
public:
  explicit VerticesRenderData(const LayersInfo* layer);
  bool GetVertices(unsigned int ind_path, std::vector<float>& vert_info);
  bool GetVertices(unsigned int group_ind, unsigned int path_ind, std::vector<float>& vert_info);
  bool GetTriangleIndex(unsigned int ind, std::vector<unsigned int>& trigs);
  bool GetTriangleIndex(unsigned int group_ind, unsigned int path_ind, std::vector<unsigned int>& trigs);
  bool GetBezierVertData(unsigned int group_ind, unsigned int path_ind, BezierVertData& vert_data);

  unsigned int GetPathsCount() const { return paths_count_; }
  template<typename T>
  T Normalize(const T& pos); 

private:
  std::vector<BezierVertData> bezier_vert_data_;//Index is group index
  unsigned int paths_count_;
};
}