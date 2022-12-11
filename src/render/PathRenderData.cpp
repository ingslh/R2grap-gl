#include "PathRenderData.h"

namespace R2grap{

PathRenderData::PathRenderData(std::shared_ptr<RenderContent> content){
  auto layer_data = content->GetLayerData();
  auto in_pos = layer_data.start_pos;
  auto out_pos = layer_data.end_pos;

  auto groups = layer_data.GetGroupData();
  std::vector<std::shared_ptr<GroupData>> in_groups;
  for(auto& group : groups){
    in_groups.emplace_back(std::make_shared<GroupData>(group));
  }
	std::vector<std::shared_ptr<GroupData>> out_groups;
	GetChildGroupData(in_groups,out_groups);
	for(std::size_t i = 0; i < out_groups.size(); i++){
		for(auto& c_path : out_groups[i]->paths){
			RePathObj obj(in_pos,out_pos, out_groups[i]->fill.get(), out_groups[i]->stroke.get());
			if(layer_data.groups_no_keyframe){
				if(i==0){
					obj.keep_trans = false;
					obj.trans = layer_data.trans;
				}
			}else{
				obj.keep_trans = false;
				obj.trans =out_groups[i]->trans;
			}

			obj.path = c_path.get();
			path_objs_.emplace_back(obj);
		}
	}
}

void PathRenderData::GetChildGroupData(std::vector<std::shared_ptr<GroupData>>& in_groups, 
                                       std::vector<std::shared_ptr<GroupData>>& out_groups){
  for(auto& group : in_groups){
    if(!group->child_groups.empty()){
			std::vector<std::shared_ptr<GroupData>> tmp_groups;
			for(auto& group : group->child_groups){
				tmp_groups.emplace_back(std::make_shared<GroupData>(group));
			}
			GetChildGroupData(tmp_groups, out_groups);
		}
    else
      out_groups.emplace_back(group);
  }
}

void PathRenderData::GenPathRenderObjs(std::shared_ptr<RenderContent> content, std::vector<RePathObj>& objs){
	auto path_render_data = std::make_shared<PathRenderData>(content);
	auto sig_path_objs = path_render_data->GetObjects();
	objs.insert(objs.end(), sig_path_objs.begin(), sig_path_objs.end());
}

}