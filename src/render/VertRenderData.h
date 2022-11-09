#pragma once
#include "BaseRenderData.h"
#include "LayersInfo.h"
#include "PathInfo.h"
#include "BezierGen.hpp"
#include <glm/glm.hpp>
#include <vector>

namespace R2grap{

typedef std::vector<BezierCluster> SignalPathData;
typedef std::vector<SignalPathData> MultiPathsData;

struct BezierVertData{
  std::vector<float> verts;//3-dim
  std::vector<unsigned int> tri_index;//3 int make up a triangle
};

class VerticesRenderData : public BaseRenderData{
public:
  explicit VerticesRenderData(const LayersInfo* layer);
  bool ConverToOpenglVert(unsigned int ind_path, std::vector<float>& vert_info);
  unsigned int GetVertNumByPathInd(unsigned int ind) const;

  template<typename T>
 T Normalize(const T& pos); 

  unsigned int GetPathsCount() const {return paths_count_;}
  const MultiPathsData& GetMultiPathsData() const {return multi_paths_data_;}
  const std::vector<unsigned int>& GetTriangleIndex(int ind) const {return bezier_vert_data_[ind].tri_index;}
  unsigned int GetTriangleIndexSize(unsigned int ind) const {return static_cast<unsigned int>(bezier_vert_data_[ind].tri_index.size());}
private:
  MultiPathsData multi_paths_data_;
  std::vector<BezierVertData> bezier_vert_data_;
  unsigned int paths_count_;
};
}