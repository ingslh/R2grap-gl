#pragma once
#include "Transform.h"

namespace R2grap{

class GroupTransform : public Transform{
public:
  GroupTransform(const nlohmann::json& transform);

protected:
  void readKeyframeandProperty(const std::string& propname, const nlohmann::json& transform) override;

}; 

}