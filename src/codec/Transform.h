#pragma once
#include <nlohmann/json.hpp>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <variant>
#include "KeyframeGen.hpp"

namespace R2grap{

enum TransformType {
  t_NoneTrans = 0,
  t_ShapeTrans,
  t_GroupTrans,
};

struct TransMat{
  unsigned int layer_index;
  float clip_start;
  float clip_end;
  std::vector<glm::mat4> trans;
  float duration;

  TransMat() : layer_index(0), clip_start(0), clip_end(0), trans(std::vector<glm::mat4>()),duration(0){}
};

//used for shape(layer) transform
struct PosRelateCurve{
		unsigned int layer_ind;
		std::map<unsigned int, std::vector<float>> value_map_;
};
//property name___link layers index__frame num __property value  
typedef std::map<std::string, std::vector<PosRelateCurve>> TransformCurve;


//used for groups transform
struct TransPropEx {
  unsigned int layer_ind;
  std::vector<unsigned int> groups_ind;
  std::map<unsigned int, std::vector<float>> value_map_;
};
typedef std::map<std::string, std::vector<TransPropEx>> TransformCurveEx;

class Transform{
public:
  explicit Transform(const nlohmann::json& transform, bool IsShapeTransform = false, int link = -1);
  Transform() :
  type_(TransformType::t_NoneTrans),
  property_values_(std::map<std::string, std::variant<glm::vec3, float>>()),
  keyframe_data_(KeyframesMap())
  {}

  glm::vec3 GetShapeGrapOffset();
  static bool IsVectorProperty(std::string);
  const KeyframesMap& GetKeyframeData()const { return keyframe_data_; }
	bool IsNoKeyframe(){return keyframe_data_.size() == 0; }

	glm::vec3& GetOrigPosition(){return std::get<t_Vector>(original_property_values_["Position"]);}
  glm::vec3& GetPosition()  { return std::get<t_Vector>(property_values_["Position"]); }
  void SetPosition(const glm::vec3& pos){property_values_["Position"] = pos;}
  glm::vec3& GetScale() {return std::get<t_Vector>(property_values_["Scale"]);}
	glm::vec3& GetOrigScale() {return std::get<t_Vector>(original_property_values_["Scale"]);}
  void SetScale(const glm::vec3& scale){property_values_["Scale"] = scale;}
  float GetRotation(){return std::get<t_Scalar>(property_values_["Rotation"]);}
  void SetRotation(const float rot){property_values_["Rotation"] = rot;}
  float GetOpacity(){return std::get<t_Scalar>(property_values_["Opacity"]);}
  void SetOpacity(const float opc){property_values_["Opacity"] = opc;}
  glm::vec3& GetAnchorPos(){return std::get<t_Vector>(property_values_["Anchor Point"]);}

  void SetKeyframeMap(const KeyframesMap& map){keyframe_data_ = map;}
  TransformType type(){return type_;}

protected:
  void ReadProperty(const std::string& propname, const nlohmann::json& transform);
	void SetOriginalProerty(){original_property_values_ = property_values_;}
private:
  TransformType type_;
  std::map<std::string, std::variant<glm::vec3, float>> property_values_;
	std::map<std::string, std::variant<glm::vec3, float>> original_property_values_;
  KeyframesMap keyframe_data_;
  //std::map<std::string, std::variant<VectorKeyFrames, ScalarKeyFrames>>
};

}