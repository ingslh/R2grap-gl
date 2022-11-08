#pragma once
#include "BaseRenderData.h"
#include "LayersInfo.h"
#include "Transform.h"
#include <glm/glm.hpp>
#include <map>

namespace R2grap{
class TransformRenderData : public BaseRenderData{
public:
  TransformRenderData(const LayersInfo* layer);
  TransMat* GetTransMat(){return transform_mat_;}

protected:
  void GenerateTransformMat(const TransformCurve& transform_curve, std::shared_ptr<Transform> transform);
  void SetInandOutPos(unsigned int ind, float in_pos, float out_pos);

private:
  TransMat* transform_mat_;
  KeyframesMap keyframe_mat_;
  std::map<int64_t, unsigned int> opacity_map_;  
};
typedef std::shared_ptr<TransformRenderData> TransformRenderDataPtr;
}