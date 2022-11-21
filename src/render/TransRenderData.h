#pragma once
#include "BaseRenderData.h"
#include "LayersInfo.h"
#include "Transform.h"
#include <glm/glm.hpp>
#include <map>

namespace R2grap{
class TransformRenderData : public BaseRenderData{
public:
  explicit TransformRenderData(const LayersInfo* layer);
  TransformRenderData(const Transform* transform, unsigned int ind, float inpos, float outpos);
  TransMat* GetTransMat(){return transform_mat_;}

protected:
  bool GenerateTransformMat(const TransformCurve& transform_curve, Transform* transform);
  void SetInandOutPos(unsigned int ind, float in_pos, float out_pos);

private:
  void CompTransformCurve(Transform* trans, TransformCurve& curve);

  /*struct TransMat {
    unsigned int layer_index;
    float clip_start;
    float clip_end;
    std::vector<glm::mat4> trans;
    std::vector<glm::mat4> link_trans;
    float duration;
  };*/
  TransMat* transform_mat_ = nullptr;

  //typedef std::map<std::string, std::variant<VectorKeyFrames, ScalarKeyFrames>> KeyframesMap;
  /*struct Keyframe {
    T lastkeyValue;                 //bezier:lastpos_y
    float lastkeyTime;              //bezier:lastpos_x
    std::vector<glm::vec2> outPos;  //vector's size is equal to T's dimension
    std::vector<glm::vec2> inPos;
    T keyValue;                     //bezier:curpos_y
    float keyTime;                  //bezier:curpos_x
  };*/
  KeyframesMap keyframe_mat_;
  std::map<int64_t, unsigned int> opacity_map_;  
};

}