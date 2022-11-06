#pragma once
#include <nlohmann/json.hpp>
#include <string>

namespace R2grap{
class PathInfo;
class FillInfo;
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

private:
  std::shared_ptr<GroupContents> contents_;
  unsigned int blend_mode_;
};


class LayersInfo{
public:
  LayersInfo(const nlohmann::json& layer);

  unsigned int GetLayerInd() const {return index_;}
  std::vector<std::shared_ptr<ShapeGroup>> GetShapeGroup()const{ return groups_;}

private:
  unsigned int index_;
  std::string name_;
  unsigned int blend_mode_;
  std::string link_;
  float start_time_;
  float out_point_;
  float in_point_;
  std::vector<std::shared_ptr<ShapeGroup>> groups_;
};
}