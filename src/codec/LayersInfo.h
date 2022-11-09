#pragma once
#include <nlohmann/json.hpp>
#include <string>
#include "Transform.h"
#include "FillInfo.h"
#include "PathInfo.h"

namespace R2grap{
class GroupContents{
public:
  GroupContents(const nlohmann::json& json);
  const std::vector<std::shared_ptr<PathInfo>>& GetPaths()const {return paths_;}
  const std::vector<std::shared_ptr<FillInfo>>& GetFills()const { return fills_; }

private:
  std::vector<std::shared_ptr<PathInfo>> paths_;
  std::vector<std::shared_ptr<FillInfo>> fills_;
  bool existMergePaths_;
};

class ShapeGroup{
public:
  ShapeGroup(const nlohmann::json& json);
  std::shared_ptr<GroupContents> GetContents()const {return contents_;}
  std::shared_ptr<Transform> GetTransform()const {return transform_;}

private:
  std::shared_ptr<GroupContents> contents_;
  std::shared_ptr<Transform> transform_;
  unsigned int blend_mode_;
};


class LayersInfo{
public:
  LayersInfo(const nlohmann::json& layer);

  unsigned int GetLayerInd() const {return index_;}
  float GetLayerInpos() const {return in_point_;}
  float GetLayerOutpos() const {return out_point_;}
  const std::vector<std::shared_ptr<ShapeGroup>>& GetShapeGroup()const{ return groups_;}
  const std::shared_ptr<Transform> GetShapeTransform()const {return transform_;}


private:
  unsigned int index_;
  std::string name_;
  unsigned int blend_mode_;
  std::string link_;
  float start_time_;
  float out_point_;
  float in_point_;
  std::vector<std::shared_ptr<ShapeGroup>> groups_;
  std::shared_ptr<Transform> transform_;
};
}