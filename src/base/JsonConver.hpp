#pragma once
#include "nlohmann/json.hpp"
#include "glm/glm.hpp"

namespace R2grap{
namespace jsonconver{

//traits
template<typename T>
struct JsonToTraits;

template<>
struct JsonToTraits<float>{
  using JsontoType = float;
  inline static JsontoType const zero = {0.f};
};

template<>
struct JsonToTraits<glm::vec2>{
  using JsontoType = glm::vec2;
  inline static JsontoType const zero = JsontoType{0};
};

template<typename T, typename JT = JsonToTraits<T>>
T json_to(const nlohmann::json json){
  using JsontoType = JT::JsontoType;
  JsontoType ret = JT::zero;
  if(typeid(T) == typeid(glm::vec2)){
    ret = glm::vec2(json[0].get<float>(), json[1].get<float>());
  }
  return ret;
}


}
}