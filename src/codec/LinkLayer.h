#pragma once

#include "Transform.h"
#include "LayersInfo.h"

namespace R2grap{

class LinkLayer{
public:
  LinkLayer(JsonReader* reader);
  static void UpdatePropertyByLink(JsonReader* reader, const std::shared_ptr<Transform>& transform, unsigned int link);
  static const KeyframesMap& GetKeyframeData(unsigned int layer_ind);
  


private:
  JsonReader* reader_ = nullptr;
  std::map<unsigned int, std::vector<unsigned int>> layers_link_map_;
};

}