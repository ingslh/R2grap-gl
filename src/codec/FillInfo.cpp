//
// Created by ingslh on 2022/12/10.
//
#include "FillInfo.h"
#include "AniInfoManager.h"

namespace R2grap{
	FillInfo::FillInfo(const nlohmann::json& json):
	blend_mode_ (json["Blend Mode"]), composite_ (json["Composite"]),fill_rule_ (json["Fill Rule"])
	{
		bool has_keyframe = false;
		if(json["Opacity"].is_number())
			opacity_ = json["Opacity"];
		else if (json["Opacity"].is_object()) {
			has_keyframe |= true;
			keyframe_property_.push_back("Opacity");
		}

		if(json["Color"].is_array())
			color_ = glm::vec4(json["Color"][0], json["Color"][1], json["Color"][2], json["Color"][3]);
		else if (json["Color"].is_object()) {
			has_keyframe |= true;
			keyframe_property_.push_back("Color");
		}


		if(!has_keyframe) return;
		auto PropertyType = [&](const std::string& proname)-> DimensionType {
				if(proname == "Color")
					return DimensionType::t_Vector;
				else if(proname == "Opacity")
					return DimensionType::t_Scalar;
				else
					return DimensionType::t_NoFind;
		};

		for(auto& el : json.items()){
			if(PropertyType(el.key()) == DimensionType::t_Vector){
				auto it = std::find(keyframe_property_.begin(), keyframe_property_.end(), el.key());
				if (it == keyframe_property_.end()) continue;
				color_ = glm::vec4(el.value()["Curve1"]["lastkeyValue"][0],
													 el.value()["Curve1"]["lastkeyValue"][1],
													 el.value()["Curve1"]["lastkeyValue"][2],1);
			}
			else if(PropertyType(el.key()) == DimensionType::t_Scalar){
				auto it = std::find(keyframe_property_.begin(), keyframe_property_.end(), el.key());
				if (it == keyframe_property_.end()) continue;
				opacity_ = el.value()["Curve1"]["lastkeyValue"];
			}
			auto frame_rate = AniInfoManager::GetIns().GetFrameRate();
			KeyframeGen keyframegen(el.key(), el.value(),PropertyType, frame_rate);
			auto pair = keyframegen.GetKeyframePair();
			if(pair != nullptr)
				keyframe_data_[pair->first] = pair->second;
		}
	}
}
