#pragma once
#include <nlohmann/json.hpp>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <variant>
#include "KeyframeGen.hpp"

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
  keyframe_data_(KeyframesMap())
  {}

  glm::vec3 GetShapeGrapOffset();
  bool IsVectorProperty(std::string);
  glm::vec3& GetPosition() { return std::get<t_Vector>(property_values_["Position"]); }
  const KeyframesMap& GetKeyframeData()const { return keyframe_data_; }

protected:
  void readKeyframeandProperty(const std::string& propname, const nlohmann::json& transform);
private:
  TransformType type_;
  std::map<std::string, std::variant<glm::vec3, float>> property_values_;
  KeyframesMap keyframe_data_;
  //std::map<std::string, std::variant<VectorKeyFrames, ScalarKeyFrames>>
};

}