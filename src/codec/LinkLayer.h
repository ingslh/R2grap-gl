#pragma once
#include "Transform.h"
#include "LayersInfo.h"
#include "JsonReader.h"
#include "RenderContent.h"

namespace R2grap{
class JsonReader;
class LinkLayer{
public:
  LinkLayer(JsonReader* reader);
  void UpdatePropertyByLink(unsigned int ind);
	void UpdateGroupsProperty(unsigned int ind);
  std::map<unsigned int, std::vector<unsigned int>> GetLinkMap()const{return layers_link_map_;}

  


private:
  JsonReader* reader_ = nullptr;
  std::map<unsigned int, std::vector<unsigned int>> layers_link_map_;
};

}