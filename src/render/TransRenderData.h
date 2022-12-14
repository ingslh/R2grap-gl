#pragma once
#include "Transform.h"
#include "RenderDataFactory.h"
#include "BaseRenderData.h"
#include "LayersInfo.h"
#include <glm/glm.hpp>
#include <map>

namespace R2grap{
struct TransMat;
class Transform;
class TransformRenderData : public BaseRenderData{
public:
  TransformRenderData(const LayersInfo* layer);
  TransformRenderData(const ShapeGroup* shape_group, unsigned int layer_ind, const std::vector<unsigned int>& groups_ind);
  TransMat* GetTransMat(){return transform_mat_;}
  void GenerateTransformMat();
  const TransformCurve& GetTransCurve()const {return transform_curve_;}
	const TransformCurve& GetOrigTransCurve()const {return orig_transform_curve_;}
  const TransformCurveEx& GetGroupOrigTransCurve() const { return orig_group_transform_curve_; }
  void SetTransCurve(const TransformCurve& curve){transform_curve_ = curve;}
  void SetTransCurve(const TransformCurveEx& curve) { group_transform_curve_ = curve; }

	void SetParentLayerInd(int ind){parent_layer_ind_ = ind;}
  void SetGroupsInd(std::vector<unsigned int> group_ind) { groups_ind_ = group_ind; }

  void freashGroupPositionInitVal();

protected:
	void CompTransformCurve(TransformCurve& curve, int layer_ind = -1);
		//need to pre-set parent_layer_ind and groups_inds
	void CompTransformCurve(TransformCurveEx& curve);
  bool GenerateTransformMat(const TransformCurve& transform_curve);
  bool GenerateTransformMat(const TransformCurveEx& transform_curve);
  void SetInandOutPos(unsigned int ind, float in_pos, float out_pos);

public:
  template<typename T, typename OutT>
  bool GetBezierKeyframe(const Keyframe<T>& keyframe, OutT& out, unsigned int dur, unsigned int s_t, T s_v);
  static void ConverCurveToCurveEx(const TransformCurve& curve1, TransformCurveEx& curve2);

private:
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
  TransformCurveEx group_transform_curve_;
  TransformCurveEx orig_group_transform_curve_;
  std::map<int64_t, unsigned int> opacity_map_;
  LayersInfo* layer_ = nullptr;
  ShapeGroup* group_ = nullptr;
  int parent_layer_ind_  = -1;
  std::vector<unsigned int> groups_ind_;
};

}