#include "LayersInfo.h"
#include "Transform.h"
#include <memory>
using namespace R2grap;

GroupContents::GroupContents(const nlohmann::json& json){
  for(auto& el : json.items()){
    if(el.key().substr(0,4) == "Path"){
      paths_.emplace_back(std::make_shared<PathInfo>(json[el.key()]));
    }
    else if(el.key().substr(0,4) == "Fill"){
      fills_=std::make_shared<FillInfo>(json[el.key()]);
    }
    else if(el.key().substr(0,6) == "Stroke"){
      stroke_=std::make_shared<StrokeInfo>(json[el.key()]);
    }
    else if (el.key().substr(0, 11) == "Merge Paths") { 
      auto merge_mode = json[el.key()]["Mode"];
    }
  }

  if (paths_.size() > 1) { //exist multi path

  }
}

ShapeGroup::ShapeGroup(const nlohmann::json& json):
blend_mode_(json["Blend Mode"]), transform_(std::make_shared<Transform>(json["Transform"]))
{
  auto has_child_groups = [&](const nlohmann::json& content)->bool{
    return content.items().begin().key().find("Group") != std::string::npos;
  };
  if(has_child_groups(json["Contents"])){
    auto contents = json["Contents"].items();
    for(auto& el : contents){
			auto child_group = std::make_shared<ShapeGroup>(el.value());
 			child_group->SetParent(weak_from_this());
      child_groups_.emplace_back(child_group);
    }
  }else
    contents_= std::make_shared<GroupContents>(json["Contents"]);
}

LayersInfo::LayersInfo(const nlohmann::json& layer) :
index_(layer["index"]),name_(layer["name"]),blend_mode_(layer["blendingMode"]),
start_time_(layer["startTime"]),out_point_(layer["outPoint"]),in_point_(layer["inPoint"])
{
	link_ = layer["Link"].is_number() ? static_cast<int>(layer["Link"]) : -1;
  transform_ = std::make_shared<Transform>(layer["Transform"],true);
  auto contents = layer["Contents"].items();
  std::map<int, nlohmann::json> groups;
  for (auto& el : contents) {
    auto key = std::stoi(el.key().substr(strlen("Group")));
    groups[key] = el.value();
  }

  for(auto& it : groups){
    groups_.emplace_back(std::make_shared<ShapeGroup>(it.second));
  }
}