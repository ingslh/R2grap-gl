#include "JsonReader.h"

namespace R2grap{

JsonReader::JsonReader(const std::string& path){
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
      AniInfoManager::GetIns().AppendLayerInandOut(layers_.back()->GetLayerInd() - 1, 
        static_cast<unsigned int>(layers_.back()->GetLayerInpos()), static_cast<unsigned int>(layers_.back()->GetLayerOutpos()));
    }
  }
  //update transform property by link layer
  LinkLayer link_set(this);
  for (auto i = 0; i < layers_.size(); i++) {
    link_set.UpdatePropertyByLink(i);
  }
  AniInfoManager::GetIns().SetLayersLinkMap(link_set.GetLinkMap());
}

unsigned int JsonReader::getLayersCount(){
  return !root_.is_null() ? static_cast<unsigned int>(root_[root_.begin().key()]["layers"].size()) : 0;
}

std::shared_ptr<LayersInfo> JsonReader::GetLayersInfo(unsigned int ind){
  if(ind < layers_.size() && !layers_.empty())
    return layers_[ind];
  else
    return nullptr;
}


}