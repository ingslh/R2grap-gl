#pragma once
#include <nlohmann/json.hpp>
#include <string>
#include <fstream>
#include <iostream>
#include "LayersInfo.h"
#include "AniInfoManager.h"
#include "LinkLayer.h"

namespace R2grap{

class LinkLayer;
class JsonReader{
public:
  JsonReader(const std::string& path);
  unsigned int getLayersCount();
  std::shared_ptr<LayersInfo> GetLayersInfo(unsigned int ind);
  unsigned int GetLayersNum(){return layers_.size();}

private:
  nlohmann::json root_;
  std::vector<std::shared_ptr<LayersInfo>> layers_;
};

}