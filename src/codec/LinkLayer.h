#pragma once
#include "Transform.h"
#include "LayersInfo.h"
#include "JsonReader.h"

namespace R2grap{
class JsonReader;
class LinkLayer{
public:
  LinkLayer(JsonReader* reader);
  void UpdatePropertyByLink(unsigned int ind);

  


private:
  JsonReader* reader_ = nullptr;
  std::map<unsigned int, std::vector<unsigned int>> layers_link_map_;
};

}