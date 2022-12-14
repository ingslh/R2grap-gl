#pragma once
#include <memory>
#include <nlohmann/json.hpp>
#include <string>
#include <utility>
#include <vector>
#include <glm/glm.hpp>
#include <variant>


namespace R2grap{

enum DimensionType {
    t_NoFind = -1,
    t_Vector,
    t_Scalar,
};

template<typename T>
struct Keyframe{
  T lastkeyValue;                 //bezier:lastpos_y
  float lastkeyTime;              //bezier:lastpos_x
  std::vector<glm::vec2> outPos;  //vector's size is equal to T's dimension
  std::vector<glm::vec2> inPos;
  T keyValue;                     //bezier:curpos_y
  float keyTime;                  //bezier:curpos_x

  Keyframe(T lastkeyValue_, float lastkeyTime_, std::vector<glm::vec2> outPos_, std::vector<glm::vec2> inPos_, T keyValue_, float keyTime_) :
    lastkeyValue(lastkeyValue_), lastkeyTime(lastkeyTime_), outPos(std::move(outPos_)), inPos(std::move(inPos_)), keyValue(keyValue_), keyTime(keyTime_){}
};

typedef std::vector<Keyframe<glm::vec3>> VectorKeyFrames;
typedef std::vector<Keyframe<float>> ScalarKeyFrames;
typedef std::pair<std::string, std::variant<VectorKeyFrames, ScalarKeyFrames>> KeyframePair;
typedef std::map<std::string, std::variant<VectorKeyFrames, ScalarKeyFrames>> KeyframesMap;

class KeyframeGen{
public:
  KeyframeGen(const std::string& propname, const nlohmann::json& transform, const std::function<DimensionType(std::string)>& valueType, unsigned int frame_rate){
    if(valueType(propname) == t_NoFind) return;
    
    bool is_vector = (valueType(propname) == t_Vector);
    auto cur_property = transform;
    
    if(cur_property.is_object()){
      VectorKeyFrames vector_keyframe;
      ScalarKeyFrames scalar_keyframe;
      for(auto it = cur_property.begin(); it != cur_property.end(); ++it){
        if (it.key().substr(0, 5) != "Curve") continue;
        auto lastkeyValue = it.value()["lastkeyValue"];
        float lastkeyTime = static_cast<float>(it.value()["lastkeyTime"]) * frame_rate;

        auto keyValue = it.value()["keyValue"];
        float keyTime = static_cast<float>(it.value()["keyTime"]) * frame_rate;

        if(is_vector){
          auto vector_lastkeyvalue = lastkeyValue.size() == 3 ? glm::vec3(lastkeyValue[0], lastkeyValue[1], lastkeyValue[2]) : 
            glm::vec3(lastkeyValue[0], lastkeyValue[1], 0);
          auto vector_keyvalue = keyValue.size() == 3 ? glm::vec3(keyValue[0], keyValue[1], keyValue[2]) : 
            glm::vec3(keyValue[0], keyValue[1], 0);

          float out_x, in_x;
          glm::vec3 out_y, in_y;
          if (it.value()["OutPos"]["x"].is_number() && it.value()["OutPos"]["y"].is_number()) {
            out_x = (keyTime - lastkeyTime) * static_cast<float>(it.value()["OutPos"]["x"]) + lastkeyTime;
            out_y = (vector_keyvalue - vector_lastkeyvalue) * static_cast<float>(it.value()["OutPos"]["y"]) + vector_lastkeyvalue;
            in_x = (keyTime - lastkeyTime) * static_cast<float>(it.value()["InPos"]["x"]) + lastkeyTime;
            in_y = (vector_keyvalue - vector_lastkeyvalue) * static_cast<float>(it.value()["InPos"]["y"]) + vector_lastkeyvalue;
          }
          else { //array
            out_x = (keyTime - lastkeyTime) * static_cast<float>(it.value()["OutPos"]["x"][0]) + lastkeyTime;
            out_y = (vector_keyvalue - vector_lastkeyvalue) * static_cast<float>(it.value()["OutPos"]["y"][0]) + vector_lastkeyvalue;
            in_x = (keyTime - lastkeyTime) * static_cast<float>(it.value()["InPos"]["x"][0]) + lastkeyTime;
            in_y = (vector_keyvalue - vector_lastkeyvalue) * static_cast<float>(it.value()["InPos"]["y"][0]) + vector_lastkeyvalue;
          }

          std::vector<glm::vec2> outpos_list, inpos_list;
          outpos_list.emplace_back(glm::vec2(out_x, out_y.x));
          outpos_list.emplace_back(glm::vec2(out_x, out_y.y));
          inpos_list.emplace_back(glm::vec2(in_x, in_y.x));
          inpos_list.emplace_back(glm::vec2(in_x, in_y.y));

          vector_keyframe.emplace_back(Keyframe<glm::vec3>(vector_lastkeyvalue, lastkeyTime, outpos_list, inpos_list, vector_keyvalue, keyTime));
        }
        else{
          float scalar_lastkeyvalue = lastkeyValue;
          float scalar_keyvalue = keyValue;

          float out_x = (keyTime - lastkeyTime) * static_cast<float>(it.value()["OutPos"]["x"][0]) + lastkeyTime;
          auto out_y = (scalar_keyvalue - scalar_lastkeyvalue) * static_cast<float>(it.value()["OutPos"]["y"][0]) + scalar_lastkeyvalue;

          float in_x = (keyTime - lastkeyTime) * static_cast<float>(it.value()["InPos"]["x"][0]) + lastkeyTime;
          auto in_y = (scalar_keyvalue - scalar_lastkeyvalue) * static_cast<float>(it.value()["InPos"]["y"][0]) + scalar_lastkeyvalue;

          std::vector<glm::vec2> outpos_list, inpos_list;
          outpos_list.emplace_back(glm::vec2(out_x, out_y));
          inpos_list.emplace_back(glm::vec2(in_x, in_y));

          scalar_keyframe.emplace_back(Keyframe<float>(scalar_lastkeyvalue, lastkeyTime, outpos_list, inpos_list, scalar_keyvalue, keyTime));
        }
      }
      if(is_vector)
        keyframe_pair_ = std::make_shared<KeyframePair>(propname, vector_keyframe);
      else
        keyframe_pair_ = std::make_shared<KeyframePair>(propname, scalar_keyframe);
    }
  }


  std::shared_ptr<KeyframePair> GetKeyframePair()const {return keyframe_pair_;}

private:
  std::shared_ptr<KeyframePair> keyframe_pair_ = nullptr;
};

}