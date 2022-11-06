#include "LayersInfo.h"
using namespace R2grap;

GroupContents::GroupContents(const nlohmann::json& json){
  for(auto& el : json.items()){
    if(el.key().substr(0,4) == "Path"){
      paths_.emplace_back(std::make_shared<PathInfo>(json[el.key()]));
    }
    else if(el.key().substr(0,4) == "Fill"){
      fills_.emplace_back(std::make_shared<FillInfo>(json[el.key()]));
    }
    else if (el.key().substr(0, 11) == "Merge Paths") { 
      auto merge_mode = json[el.key()]["Mode"];
    }
  }

  if (paths_.size() > 1) { //exist multi path

  }
}

ShapeGroup::ShapeGroup(const nlohmann::json& json){
  blend_mode_ = json["Blend Mode"];
  //transform = std::make_shared<Transform>(json["Transform"]);
  contents_ = std::make_shared<GroupContents>(json["Contents"]);
}

LayersInfo::LayersInfo(const nlohmann::json& layer) :
index_(layer["index"]),name_(layer["name"]),blend_mode_(layer["blendingMode"]),
link_(layer["Parent&Link"]),start_time_(layer["startTime"]),out_point_(layer["outPoint"]),in_point_(layer["inPoint"])
{
  /*
  transform = std::make_shared<Transform>(layer["Transform"],true);
  transform->SetInandOutPos(index, inPoint, outPoint);
  transform->GenerateTransformMat();*/

  auto contents = layer["Contents"].items();
  for(auto& el : contents){
    groups_.emplace_back(std::make_shared<ShapeGroup>(el.value()));
  }
}