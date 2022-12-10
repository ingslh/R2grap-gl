#pragma once
#include <nlohmann/json.hpp>
#include <string>
#include "Transform.h"
#include "FillInfo.h"
#include "PathInfo.h"
#include "StrokeInfo.h"

namespace R2grap{
class Transform;
class GroupContents{
public:
  GroupContents(const nlohmann::json& json);
  const std::vector<std::shared_ptr<PathInfo>>& GetPaths()const {return paths_;}
  const std::shared_ptr<FillInfo> GetFills()const { return fills_; }
  const std::shared_ptr<StrokeInfo> GetStroke()const {return stroke_;}
  unsigned int GetPathsNum() const { return paths_.size(); }

private:
  std::vector<std::shared_ptr<PathInfo>> paths_;
  std::shared_ptr<FillInfo> fills_ = nullptr;
  std::shared_ptr<StrokeInfo> stroke_ = nullptr;
  bool existMergePaths_;
};

class ShapeGroup{
public:
  ShapeGroup(const nlohmann::json& json);
  std::shared_ptr<GroupContents> GetContents()const {return contents_;}
  std::shared_ptr<Transform> GetTransform()const {return transform_;}
  bool HasChildGroups(){return child_groups_.size();}
  const std::vector<std::shared_ptr<ShapeGroup>>& GetChildGroups() { return child_groups_; }
	const std::shared_ptr<ShapeGroup> parent()const {return parent_group_;}

private:
		void SetParent(const std::shared_ptr<ShapeGroup> parent){ parent_group_ = parent;}

private:
	std::shared_ptr<ShapeGroup> parent_group_ = nullptr;
  std::vector<std::shared_ptr<ShapeGroup>> child_groups_;
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
  int GetLinkInd() const {return link_;}


private:
  unsigned int index_; // note : form 1 to count, not 0
  std::string name_;
  unsigned int blend_mode_;
  int link_;
  float start_time_;
  float out_point_;
  float in_point_;
  std::vector<std::shared_ptr<ShapeGroup>> groups_;
  std::shared_ptr<Transform> transform_;
};
}