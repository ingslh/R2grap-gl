#pragma once
#include <nlohmann/json.hpp>
#include <string>
#include <fstream>
#include <iostream>
#include "LayersInfo.h"
#include "AniInfoManager.h"
#include "LinkLayer.h"

namespace R2grap{

class JsonReader{
public:

  JsonReader(const std::string& path){
    std::ifstream json_stream;
    json_stream.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    try{
      json_stream.open(path);
      root_ = nlohmann::json::parse(json_stream);
      json_stream.close();
    }
    catch (std::ifstream::failure& e){
      std::cout << "ERROR::JSON::FILE_NOT_SUCCESFULLY_READ: " << e.what() << std::endl;
    }
    AniInfoManager::GetIns().SetBasicInfo(root_[root_.begin().key()]);

    if (!root_.is_null()) {//get layers info
      auto layers = root_[root_.begin().key()]["layers"];
      for (auto& el_c : layers.items()) {
        layers_.emplace_back(std::make_shared<LayersInfo>(el_c.value()));
      }
    }

    //update transform property by link layer
    //LinkLayer link_set(this);
  }


  unsigned int getLayersCount(){
    return !root_.is_null() ? static_cast<unsigned int>(root_[root_.begin().key()]["layers"].size()) : 0;
  }

  std::shared_ptr<LayersInfo> GetLayersInfo(unsigned int ind){
    if(ind < layers_.size() && layers_.size())
      return layers_[ind];
    else
      return std::shared_ptr<LayersInfo>(NULL);
  }

  unsigned int GetLayersNum(){return layers_.size();}

private:
  nlohmann::json root_;
  std::vector<std::shared_ptr<LayersInfo>> layers_;
};

}