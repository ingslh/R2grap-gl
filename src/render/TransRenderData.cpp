#include "TransRenderData.h"
#include "BezierGen.hpp"
#include "AniInfoManager.h"
#include <glm/gtc/matrix_transform.hpp>

namespace R2grap{

TransformRenderData::TransformRenderData(const LayersInfo* layer) : //shape-layer transform 
keyframe_mat_(layer->GetShapeTransform()->GetKeyframeData()), layer_(const_cast<LayersInfo*>(layer)){
	auto transform = layer->GetShapeTransform();
  CompTransformCurve(transform.get(), transform_curve_, layer_->GetLayerInd());
}

TransformRenderData::TransformRenderData(const ShapeGroup* shape_group, unsigned int ind, float inpos, float outpos) :
  keyframe_mat_(shape_group->GetTransform()->GetKeyframeData()), group_(const_cast<ShapeGroup*>(shape_group)) {//group transform 
  auto transform = shape_group->GetTransform();
  transform_mat_ = new TransMat();
  SetInandOutPos(ind, inpos, outpos);
  CompTransformCurve(transform.get(), transform_curve_, ind);//gengerate origial transform curve
}

void TransformRenderData::GenerateTransformMat(){
  if (layer_) {
    auto transform = layer_->GetShapeTransform();
    transform_mat_ = new TransMat();
    SetInandOutPos(layer_->GetLayerInd(), layer_->GetLayerInpos(), layer_->GetLayerOutpos());
    GenerateTransformMat(transform_curve_, transform.get());
  }
  else if (group_) {
    auto transform = group_->GetTransform();
		if(parent_layer_ind_ != -1){

			auto base_pos = AniInfoManager::GetIns().GetTransPos(parent_layer_ind_);
			auto base_anc_pos = AniInfoManager::GetIns().GetTransAncPos(parent_layer_ind_);
      glm::vec2 offset = base_pos - base_anc_pos;
      glm::vec2 cur_scale = AniInfoManager::GetIns().GetTransScale(parent_layer_ind_);
			
			auto parent_group = group_->parent();
			while(parent_group.lock()){
				auto pos = parent_group.lock()->GetTransform()->GetPosition();
				auto anchor_pos = parent_group.lock()->GetTransform()->GetAnchorPos();
        auto scale = parent_group.lock()->GetTransform()->GetScale();
				offset += glm::vec2(pos.x, pos.y) - glm::vec2(anchor_pos.x, anchor_pos.y);
        cur_scale *= glm::vec2(scale.x, scale.y);
				parent_group = parent_group.lock()->parent();
			}
      transform->SetScale(transform->GetScale()* glm::vec3(cur_scale.x/100, cur_scale.y/100, 0));
			transform->SetPosition(transform->GetPosition()+ glm::vec3(offset.x, offset.y, 0));
		}
    GenerateTransformMat(transform_curve_, transform.get());
  }
}


void TransformRenderData::CompTransformCurve(Transform* trans, TransformCurve& curve, int ind){
  curve.clear();
  for (auto & it : keyframe_mat_){
    if(trans->IsVectorProperty(it.first)){ //vector
      auto vector_keyframes = std::get<t_Vector>(it.second);
      auto start_value =  vector_keyframes[0].lastkeyValue;
      auto start = vector_keyframes.front().lastkeyTime;
      std::vector<std::map<unsigned int, float>> double_curve_line;
      double_curve_line.resize(2);

      for (auto& keyframe : vector_keyframes) {
        auto bezier_duration = static_cast<unsigned int>(keyframe.keyTime - keyframe.lastkeyTime);
        glm::vec2 lastPos_x(keyframe.lastkeyTime, keyframe.lastkeyValue.x);
        glm::vec2 lastPos_y(keyframe.lastkeyTime, keyframe.lastkeyValue.y);
        glm::vec2 outPos_x(keyframe.outPos[0].x, keyframe.outPos[0].y);
        glm::vec2 outPos_y(keyframe.outPos[1].x, keyframe.outPos[1].y);
        glm::vec2 inPos_x(keyframe.inPos[0].x, keyframe.inPos[0].y);
        glm::vec2 inPos_y(keyframe.inPos[1].x, keyframe.inPos[1].y);
        glm::vec2 curPos_x(keyframe.keyTime, keyframe.keyValue.x);
        glm::vec2 curPos_y(keyframe.keyTime, keyframe.keyValue.y);
        BezierGenerator generator_x(lastPos_x, outPos_x, inPos_x, curPos_x, bezier_duration, static_cast<unsigned int>(start), start_value.x);
        auto curve_x = generator_x.getKeyframeCurveMap();
        BezierGenerator generator_y(lastPos_y, outPos_y, inPos_y, curPos_y, bezier_duration, static_cast<unsigned int>(start), start_value.y);
        auto curve_y = generator_y.getKeyframeCurveMap();
        start += static_cast<unsigned int>(curve_x.size()); 
        double_curve_line.front().merge(curve_x);
        double_curve_line.back().merge(curve_y);
      }
      curve[it.first] = double_curve_line;
    }
    else{ //scalar
      auto scalar_keyframes = std::get<t_Scalar>(it.second);
      auto start_value = it.first == "Rotation" ? 0.0 : scalar_keyframes[0].lastkeyValue;
			//auto start_value = scalar_keyframes[0].lastkeyValue;

      auto start = static_cast<unsigned int>(scalar_keyframes.front().lastkeyTime);
			if(it.first != "Rotation"){
				std::vector<SigDimCurve> signal_curve_line;
				signal_curve_line.resize(1);

				for (auto& keyframe : scalar_keyframes) {
					auto bezier_duration = static_cast<unsigned int>(keyframe.keyTime - keyframe.lastkeyTime);
					glm::vec2 lastPos(keyframe.lastkeyTime, keyframe.lastkeyValue);
					glm::vec2 curPos(keyframe.keyTime, keyframe.keyValue);
					glm::vec2 inPos(keyframe.inPos[0]);
					glm::vec2 outPos(keyframe.outPos[0]);
					BezierGenerator generator(lastPos, outPos, inPos, curPos, bezier_duration, start, start_value);
					auto curve = generator.getKeyframeCurveMap();
					start += static_cast<unsigned int>(curve.size());

					signal_curve_line[0].merge(curve);//cpp17 support,if old cpp verison can use "signal_curve_line[0].insert(curve.begin(),curve.end());"
				}
				curve[it.first] = signal_curve_line;
			}else{
				std::vector<RotationCurve> rot_curve_line;
        rot_curve_line.resize(1);

        for(auto& keyframe : scalar_keyframes){
          auto bezier_duration = static_cast<unsigned int>(keyframe.keyTime - keyframe.lastkeyTime);
					glm::vec2 lastPos(keyframe.lastkeyTime, keyframe.lastkeyValue);
					glm::vec2 curPos(keyframe.keyTime, keyframe.keyValue);
					glm::vec2 inPos(keyframe.inPos[0]);
					glm::vec2 outPos(keyframe.outPos[0]);
					BezierGenerator generator(lastPos, outPos, inPos, curPos, bezier_duration, start, start_value);
					auto curve = generator.getKeyframeCurveMap();
					start += static_cast<unsigned int>(curve.size());

          rot_curve_line.front().rot_value_map_.merge(curve);
          rot_curve_line.front().layer_ind = ind - 1;
        }
        curve[it.first] = rot_curve_line;
			}
    }
  }
	orig_transform_curve_ = curve;
}

void TransformRenderData::CompTransformCurve(const Transform* trans, TransformCurveEx& curve) {
  curve.clear();
  for (auto & it : keyframe_mat_) {
    if (trans->IsVectorProperty(it.first)) {
      auto vector_keyframes = std::get<t_Vector>(it.second);
      auto start_value = vector_keyframes[0].lastkeyValue;
      auto start = vector_keyframes.front().lastkeyTime;
      std::map<unsigned int, std::vector<float>> curve_line;

      for (auto& keyframe : vector_keyframes) {
        auto bezier_duration = static_cast<unsigned int>(keyframe.keyTime - keyframe.lastkeyTime);
        glm::vec2 lastPos_x(keyframe.lastkeyTime, keyframe.lastkeyValue.x);
        glm::vec2 lastPos_y(keyframe.lastkeyTime, keyframe.lastkeyValue.y);
        glm::vec2 outPos_x(keyframe.outPos[0].x, keyframe.outPos[0].y);
        glm::vec2 outPos_y(keyframe.outPos[1].x, keyframe.outPos[1].y);
        glm::vec2 inPos_x(keyframe.inPos[0].x, keyframe.inPos[0].y);
        glm::vec2 inPos_y(keyframe.inPos[1].x, keyframe.inPos[1].y);
        glm::vec2 curPos_x(keyframe.keyTime, keyframe.keyValue.x);
        glm::vec2 curPos_y(keyframe.keyTime, keyframe.keyValue.y);
        BezierGenerator generator_x(lastPos_x, outPos_x, inPos_x, curPos_x, bezier_duration, static_cast<unsigned int>(start), start_value.x);
        BezierGenerator generator_y(lastPos_y, outPos_y, inPos_y, curPos_y, bezier_duration, static_cast<unsigned int>(start), start_value.y);
        start += static_cast<unsigned int>(generator_x.getKeyframeCurve().size());
        std::map<unsigned int, std::vector<float>> tmp_curve;
        auto ret = BezierGenerator::MergeKeyframeCurve(generator_x, generator_y, tmp_curve);
        curve_line.merge(tmp_curve);
      }
      curve[it.first] = { static_cast<unsigned int>(parent_layer_ind_), groups_ind_, curve_line };
    }
    else {
      auto scalar_keyframes = std::get<t_Scalar>(it.second);
      auto start_value = it.first == "Rotation" ? 0.0 : scalar_keyframes[0].lastkeyValue;
      auto start = static_cast<unsigned int>(scalar_keyframes.front().lastkeyTime);
      std::map<unsigned int, std::vector<float>> curve_line;

      for (auto& keyframe : scalar_keyframes) {
        auto bezier_duration = static_cast<unsigned int>(keyframe.keyTime - keyframe.lastkeyTime);
        glm::vec2 lastPos(keyframe.lastkeyTime, keyframe.lastkeyValue);
        glm::vec2 curPos(keyframe.keyTime, keyframe.keyValue);
        glm::vec2 inPos(keyframe.inPos[0]);
        glm::vec2 outPos(keyframe.outPos[0]);
        BezierGenerator generator(lastPos, outPos, inPos, curPos, bezier_duration, start, start_value);
        auto curve = generator.getKeyframeCurveMap();
        start += static_cast<unsigned int>(curve.size());
        for (auto& pair : curve)
          curve_line[pair.first] = { pair.second };
      }
      curve[it.first] = { static_cast<unsigned int>(parent_layer_ind_), groups_ind_, curve_line };
    }
  }
}

bool TransformRenderData::GenerateTransformMat(const TransformCurve& transform_curve, Transform* transform){
  auto reslution = glm::vec3(AniInfoManager::GetIns().GetWidth(), AniInfoManager::GetIns().GetHeight(), 0);

  if(!transform_mat_) return false;
  transform_mat_->trans.clear();

	auto frame_lenth = transform_mat_->clip_end - transform_mat_->clip_start + 1;

  for (unsigned int i = 0; i < frame_lenth; i++){
    glm::mat4 trans = glm::mat4(1.0f);
    if (transform_curve.count("Position")){
      std::vector<float> offset;
      offset.resize(2);
      auto pos_curve = std::get<0>(transform_curve.at("Position"));
      for (auto j = 0; j < offset.size(); j++) {
        if(i < pos_curve[j].begin()->first)
          offset[j] = pos_curve[j].begin()->second;
        else if( i > pos_curve[j].rbegin()->first)
          offset[j] = pos_curve[j].rbegin()->second;
        else
          offset[j] = pos_curve[j].at(i);
      }
      trans = glm::translate(trans, glm::vec3(offset.front()/reslution.x, offset.back()/reslution.y, 0));
    }

    if (transform_curve.count("Rotation")) {
      float rot;
      auto rot_curves = std::get<1>(transform_curve.at("Rotation"));
			glm::mat4 rot_trans = glm::mat4(1.0f);
      for(auto& rot_curve : rot_curves){
        auto link_ind = rot_curve.layer_ind;
        auto pos_v2 = AniInfoManager::GetIns().GetTransPos(link_ind);
        auto pos_v3 = glm::vec3(pos_v2.x, pos_v2.y, 0) / reslution - glm::vec3(0.5, 0.5, 0);
        if (i < rot_curve.rot_value_map_.begin()->first)
          rot = rot_curve.rot_value_map_.begin()->second;
        else if(i > rot_curve.rot_value_map_.rbegin()->first)
          rot = rot_curve.rot_value_map_.rbegin()->second;
        else
          rot = rot_curve.rot_value_map_.at(i);
        
        auto t1 = glm::translate(glm::mat4(1), -glm::vec3(pos_v3.x, pos_v3.y, 0));
        auto r = glm::rotate(glm::mat4(1), glm::radians(rot), glm::vec3(0, 0, 1.0));
        auto t2 = glm::translate(glm::mat4(1), glm::vec3(pos_v3.x, pos_v3.y, 0));
        trans = trans * t2 * r * t1;
      }
    }

    if (transform_curve.count("Scale")) {
      std::vector<float> scale;
      scale.resize(2);
      auto scale_curve = std::get<0>(transform_curve.at("Scale"));
      for (auto j = 0; j < scale.size(); j++) {
        if (i < scale_curve[j].begin()->first)
          scale[j] = scale_curve[j].begin()->second;
        else if (i > scale_curve[j].rbegin()->first)
          scale[j] = scale_curve[j].rbegin()->second;
        else
          scale[j] = scale_curve[j].at(i);
      }
      auto start_pos = transform->GetPosition();
			auto start_scale = transform->GetScale();
      glm::mat4 t1, t2, s;
      unsigned int t;
			auto cur_pos = start_pos / glm::vec3(reslution.x, reslution.y, 1.0) - glm::vec3(0.5, 0.5, 0);
			t1 = glm::translate(glm::mat4(1), -glm::vec3(cur_pos));
			s = glm::scale(glm::mat4(1), glm::vec3((start_scale.x + scale.front()) / 100, (start_scale.y + scale.back()) / 100, 1.0));
			t2 = glm::translate(glm::mat4(1), glm::vec3(cur_pos));
      trans = trans * t2 * s * t1;
    }
    transform_mat_->trans.emplace_back(trans);
  }
	return true;
}

void TransformRenderData::SetInandOutPos(unsigned int ind, float in_pos, float out_pos) {
  transform_mat_->layer_index = ind;
  auto frameRate = AniInfoManager::GetIns().GetFrameRate();
  auto duration = AniInfoManager::GetIns().GetDuration();
  transform_mat_->clip_start = in_pos * frameRate < 0 ? 0 : in_pos * frameRate;
  transform_mat_->clip_end = out_pos > duration  ? duration * frameRate : out_pos * frameRate;
}


}