#pragma once
#include <nlohmann/json.hpp>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <variant>

namespace R2grap{


enum ProperType{
  t_None = 0,

  //vector
  t_AnchorPos,//transform
  t_Position,
  t_Scale,
  t_Color,//fill/stroke

  //scalar
  t_Rotation,//transform
  k_Skew,//shape-transform
  k_SkewAxis,
  k_StrokeWidth,//stroke

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
typedef std::pair<std::string, std::variant<VectorKeyFrames, ScalarKeyFrames>> KeyframePair;
typedef std::map<std::string, std::variant<VectorKeyFrames, ScalarKeyFrames>> KeyframesMap;

class KeyframeGen{
public:

  KeyframeGen(const std::string& propname, const nlohmann::json& transform, const std::function<bool(std::string)>& valueType){
    if(valueType(propname)>0 && valueType(propname) < 5){
      
    }
  }


  const KeyframePair& GetKeyframePair()const {return keyframe_pair_;}

private:
  KeyframePair keyframe_pair_;
};

}