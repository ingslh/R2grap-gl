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
  void GenerateTransformMat();
  const TransformCurve& GetTransCurve()const {return transform_curve_;}
	const TransformCurve& GetOrigTransCurve()const {return orig_transform_curve_;}
  void SetTransCurve(const TransformCurve& curve){transform_curve_ = curve;}

protected:
  bool GenerateTransformMat(const TransformCurve& transform_curve, Transform* transform);
  void SetInandOutPos(unsigned int ind, float in_pos, float out_pos);

private:
  void CompTransformCurve(Transform* trans, TransformCurve& curve, int layer_ind = -1);

  /*struct TransMat {
    unsigned int layer_index;
    float clip_start;
    float clip_end;
    std::vector<glm::mat4> trans;
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
  TransformCurve transform_curve_;//the temp data, need to merge with parent's curve
	TransformCurve orig_transform_curve_;
  std::map<int64_t, unsigned int> opacity_map_;
  LayersInfo* layer_ = nullptr;
};

}