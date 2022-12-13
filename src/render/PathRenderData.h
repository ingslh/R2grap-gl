#pragma once
#include <map>
#include <utility>
#include <vector>
#include <glm/glm.hpp>
#include "RenderContent.h"

namespace R2grap{
struct PathData;
struct FillData;
struct StrokeData;
struct RePathObj{
		float in_pos;
		float out_pos;

		PathData* path = nullptr;
		FillData* fill = nullptr;
		StrokeData* stroke = nullptr;

		bool keep_trans = true;
		std::vector<glm::mat4> trans;

		RePathObj(float in, float out, FillData* fill, StrokeData* stroke):
			in_pos(in), out_pos(out), fill(fill),stroke(stroke){}
};
struct GroupData;
class RenderContent;
class PathRenderData{
public:
  explicit PathRenderData(std::shared_ptr<RenderContent> content);
	static void GenPathRenderObjs(std::shared_ptr<RenderContent> content, std::vector<RePathObj>& objs);
	std::vector<RePathObj> GetObjects() const {return path_objs_;}
	
private:
  void GetChildGroupData(std::vector<std::shared_ptr<GroupData>>& in_groups, 
                         std::vector<std::shared_ptr<GroupData>>& out_groups);

private:
  std::vector<RePathObj> path_objs_;
};
}