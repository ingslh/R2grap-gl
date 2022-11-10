#pragma once
#include <nlohmann/json.hpp>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <variant>

namespace R2grap{

enum TransformProp {
  k_AnchorPos = 0,
  k_Position,
  k_Scale,
  k_Rotation,
  k_Opacity,
  k_Skew,
  k_SkewAxis,
};

enum TransformType {
  t_None = 0,
  t_ShapeTrans,
  t_GroupTrans,
};

enum PropertyOrKeyvalueType {
  t_Vector = 0,
  t_Scalar,
};

template<typename T>
struct Keyframe{
  T lastkeyValue;
  float lastkeyTime;
  std::vector<glm::vec2> outPos;
  std::vector<glm::vec2> inPos;
  T keyValue;
  float keyTime;

  Keyframe(T lastkeyValue_, float lastkeyTime_, std::vector<glm::vec2> outPos_, std::vector<glm::vec2> inPos_, T keyValue_, float keyTime_) :
    lastkeyValue(lastkeyValue_), lastkeyTime(lastkeyTime_), outPos(outPos_), inPos(inPos_), keyValue(keyValue_), keyTime(keyTime_){}
};

typedef std::vector<Keyframe<glm::vec3>> VectorKeyFrames;
typedef std::vector<Keyframe<float>> ScalarKeyFrames;
typedef std::map<std::string, std::variant<VectorKeyFrames, ScalarKeyFrames>> KeyframesMap;

struct TransMat{
  unsigned int layer_index;
  float clip_start;
  float clip_end;
  std::vector<glm::mat4> trans;
  float duration;

  TransMat() : layer_index(0), clip_start(0), clip_end(0), trans(std::vector<glm::mat4>()), duration(0){}
};

typedef std::map<std::string, std::vector<std::map<unsigned int, float>>> TransformCurve;

class Transform{
public:
  explicit Transform(const nlohmann::json& transform, bool IsShapeTransform = false);
  Transform() :
  type_(TransformType::t_None),
  property_values_(std::map<std::string, std::variant<glm::vec3, float>>()),
  keyframe_data_(KeyframesMap()),
  transform_mat_(TransMat()),
  transform_curve_(TransformCurve())
  {}

  glm::vec3 GetShapeGrapOffset();
  bool IsVectorProperty(std::string);
  glm::vec3& GetPosition() { return std::get<t_Vector>(property_values_["Position"]); }
  const KeyframesMap& GetKeyframeData()const { return keyframe_data_; }
  const TransMat* GetTransMat()const {return &transform_mat_;}

protected:
  virtual void readKeyframeandProperty(const std::string& propname, const nlohmann::json& transform);
private:
  TransformType type_;
  std::map<std::string, std::variant<glm::vec3, float>> property_values_;
  KeyframesMap keyframe_data_;
  TransMat transform_mat_;
  TransformCurve transform_curve_;
};

}