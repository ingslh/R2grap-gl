#include "TransRenderData.h"
#include "BezierGen.hpp"
#include "AniInfoManager.h"
#include <glm/gtc/matrix_transform.hpp>

namespace R2grap{

TransformRenderData::TransformRenderData(const LayersInfo* layer) : //shape-layer transform 
keyframe_mat_(layer->GetShapeTransform()->GetKeyframeData()), layer_(const_cast<LayersInfo*>(layer)){
	auto transform = layer->GetShapeTransform();
	transform_mat_ = new TransMat();
	SetInandOutPos(layer_->GetLayerInd(), layer_->GetLayerInpos(), layer_->GetLayerOutpos());

  AniInfoManager::GetIns().SetLinkTransformMap({ (unsigned int)layer_->GetLayerInd() - 1 }, transform);

  CompTransformCurve(transform_curve_, (int)layer_->GetLayerInd() - 1);
}

TransformRenderData::TransformRenderData(const ShapeGroup* shape_group, unsigned int layer_ind, const std::vector<unsigned int>& groups_ind) :
  keyframe_mat_(shape_group->GetTransform()->GetKeyframeData()), group_(const_cast<ShapeGroup*>(shape_group)) ,
	parent_layer_ind_((int)layer_ind),groups_ind_(groups_ind){//group transform
  auto transform = shape_group->GetTransform();
  transform_mat_ = new TransMat();
	float inpos = 0, outpos = 0;
	AniInfoManager::GetIns().GetLayerInandOutPos(layer_ind, inpos, outpos, true);
  SetInandOutPos(layer_ind, inpos, outpos);

	auto tmp_indexs_list = groups_ind;
	tmp_indexs_list.insert(tmp_indexs_list.begin(), layer_ind);
	AniInfoManager::GetIns().SetLinkTransformMap(tmp_indexs_list, transform);

	freashGroupPositionInitVal();
	CompTransformCurve(group_transform_curve_);
}

void TransformRenderData::GenerateTransformMat(){
  if (layer_) {
    auto transform = layer_->GetShapeTransform();
    GenerateTransformMat(transform_curve_);
  }
  else if (group_) {
    auto transform = group_->GetTransform();
    GenerateTransformMat(group_transform_curve_);
  }
}


void TransformRenderData::CompTransformCurve(TransformCurve& curve, int ind){
  curve.clear();
  for (auto & it : keyframe_mat_){
    if(R2grap::Transform::IsVectorProperty(it.first)){ //vector
      auto vector_keyframes = std::get<t_Vector>(it.second);
      auto start_value =  vector_keyframes.front().lastkeyValue;
      auto start = vector_keyframes.front().lastkeyTime;
      std::map<unsigned int, std::vector<float>> curve_line;

      for (auto& keyframe : vector_keyframes) {
        auto bezier_duration = static_cast<unsigned int>(keyframe.keyTime - keyframe.lastkeyTime);
        std::vector<std::vector<glm::vec2>> out;
        GetBezierKeyframe<glm::vec3, std::vector<std::vector<glm::vec2>>>(keyframe, out, bezier_duration, static_cast<unsigned int>(start), start_value);
        start += static_cast<float>(out.front().size());
        std::map<unsigned int, std::vector<float>> tmp_curve;
        BezierGenerator::MergeKeyframeCurve(out.front(), out.back(), tmp_curve);
        curve_line.merge(tmp_curve);
      }
      PosRelateCurve pos_relate_curve;
      pos_relate_curve.layer_ind = ind;
      pos_relate_curve.value_map_ = curve_line;
      curve[it.first] = std::vector<PosRelateCurve>({ pos_relate_curve });
    }
    else{ //rotation
      auto scalar_keyframes = std::get<t_Scalar>(it.second);
      auto start_value = scalar_keyframes.front().lastkeyValue;
      auto start = scalar_keyframes.front().lastkeyTime;
      std::map<unsigned int, std::vector<float>> curve_line;

      for(auto& keyframe : scalar_keyframes){
        auto bezier_duration = static_cast<unsigned int>(keyframe.keyTime - keyframe.lastkeyTime);
        std::map<unsigned int, float> out;
        GetBezierKeyframe<float, std::map<unsigned int, float>>(keyframe, out, bezier_duration, static_cast<unsigned int>(start), start_value);

        start += static_cast<float>(out.size());
        for (auto& pair : out)
          curve_line[pair.first] = { pair.second };
      }
      PosRelateCurve pos_relate_curve;
      pos_relate_curve.layer_ind = ind;
      pos_relate_curve.value_map_ = curve_line;
      curve[it.first] = std::vector<PosRelateCurve>({ pos_relate_curve });
    }
  }
	orig_transform_curve_ = curve;
}

void TransformRenderData::CompTransformCurve(TransformCurveEx& curve) {
  curve.clear();
  for (auto & it : keyframe_mat_) {
    if (R2grap::Transform::IsVectorProperty(it.first)) {
      auto vector_keyframes = std::get<t_Vector>(it.second);
      auto start_value = vector_keyframes.front().lastkeyValue;
      auto start = vector_keyframes.front().lastkeyTime;
      std::map<unsigned int, std::vector<float>> curve_line;

      for (auto& keyframe : vector_keyframes) {
        auto bezier_duration = static_cast<unsigned int>(keyframe.keyTime - keyframe.lastkeyTime);
        std::vector<std::vector<glm::vec2>> out;
        GetBezierKeyframe<glm::vec3, std::vector<std::vector<glm::vec2>>>(keyframe, out, bezier_duration, static_cast<unsigned int>(start), start_value);
        start += static_cast<float>(out.front().size());
        std::map<unsigned int, std::vector<float>> tmp_curve;
        BezierGenerator::MergeKeyframeCurve(out.front(), out.back(), tmp_curve);
        curve_line.merge(tmp_curve);
      }
      curve[it.first] = { { static_cast<unsigned int>(parent_layer_ind_), groups_ind_, curve_line } };
    }
    else {
      auto scalar_keyframes = std::get<t_Scalar>(it.second);
      auto start_value = scalar_keyframes.front().lastkeyValue;
      auto start = static_cast<unsigned int>(scalar_keyframes.front().lastkeyTime);
      std::map<unsigned int, std::vector<float>> curve_line;

      for (auto& keyframe : scalar_keyframes) {
        auto bezier_duration = static_cast<unsigned int>(keyframe.keyTime - keyframe.lastkeyTime);
        std::map<unsigned int, float> out;
        GetBezierKeyframe<float, std::map<unsigned int, float>>(keyframe, out, bezier_duration, static_cast<unsigned int>(start), start_value);

        start += static_cast<unsigned int>(out.size());
        for (auto& pair : out)
          curve_line[pair.first] = { pair.second };
      }
      curve[it.first] = { { static_cast<unsigned int>(parent_layer_ind_), groups_ind_, curve_line } };
    }
  }
  orig_group_transform_curve_ = curve;
}

bool TransformRenderData::GenerateTransformMat(const TransformCurve& transform_curve){
  auto reslution = glm::vec3(AniInfoManager::GetIns().GetWidth(), AniInfoManager::GetIns().GetHeight(), 1);

  if(!transform_mat_) return false;
  transform_mat_->trans.clear();
	auto frame_lenth = static_cast<unsigned int>(transform_mat_->clip_end - transform_mat_->clip_start + 1);

  for (unsigned int i = 0; i < frame_lenth; i++){
    glm::mat4 trans = glm::mat4(1.0f);
    if (transform_curve.count("Position")){
      std::vector<float> offset;
      offset.resize(2);
      auto pos_curve = transform_curve.at("Position").begin();
      if(i < pos_curve->value_map_.begin()->first)
        offset = pos_curve->value_map_.begin()->second;
      else if( i > pos_curve->value_map_.rbegin()->first)
        offset = pos_curve->value_map_.rbegin()->second;
      else
        offset = pos_curve->value_map_.at(i);
      
      trans = glm::translate(trans, glm::vec3(offset.front()/reslution.x, offset.back()/reslution.y, 0));
    }

    if (transform_curve.count("Rotation")) {
      float rot;
      auto rot_curves = transform_curve.at("Rotation");
			glm::mat4 rot_trans = glm::mat4(1.0f);
      for(auto& rot_curve : rot_curves){
        std::vector<unsigned int> tmp_indexs = { rot_curve.layer_ind };
        glm::vec3 rot_pos = glm::vec3(AniInfoManager::GetIns().GetTransPos(tmp_indexs.front()), 0);
        float start_rot = AniInfoManager::GetIns().GetTransRotation(tmp_indexs.front());
        rot_pos = glm::vec3(rot_pos.x, rot_pos.y, 0) / reslution - glm::vec3(0.5, 0.5, 0);

        if (i < rot_curve.value_map_.begin()->first)
          rot = rot_curve.value_map_.begin()->second.front();
        else if(i > rot_curve.value_map_.rbegin()->first)
          rot = rot_curve.value_map_.rbegin()->second.front();
        else
          rot = rot_curve.value_map_.at(i).front();
        
        auto t1 = glm::translate(glm::mat4(1), -glm::vec3(rot_pos.x, rot_pos.y, 0));
        auto r = glm::rotate(glm::mat4(1), glm::radians(start_rot + rot), glm::vec3(0, 0, 1.0));
        auto t2 = glm::translate(glm::mat4(1), glm::vec3(rot_pos.x, rot_pos.y, 0));
        trans = trans * t2 * r * t1;
      }
    }

    if (transform_curve.count("Scale")) {
      std::vector<float> scale;
      scale.resize(2);
      auto scale_curves = transform_curve.at("Scale");
      for (auto& scale_curve : scale_curves) {
        std::vector<unsigned int> tmp_indexs = { scale_curve.layer_ind };
        glm::vec3 scale_pos = glm::vec3(AniInfoManager::GetIns().GetTransPos(tmp_indexs.front()), 0);
        scale_pos = glm::vec3(scale_pos.x, scale_pos.y, 0) / reslution - glm::vec3(0.5, 0.5, 0);
        auto start_scale = glm::vec3(AniInfoManager::GetIns().GetTransScale(tmp_indexs.front()), 0);

        if (i < scale_curve.value_map_.begin()->first)
          scale = scale_curve.value_map_.begin()->second;
        else if (i > scale_curve.value_map_.rbegin()->first)
          scale = scale_curve.value_map_.rbegin()->second;
        else
          scale = scale_curve.value_map_.at(i);

        auto t1 = glm::translate(glm::mat4(1), -glm::vec3(scale_pos.x, scale_pos.y, 0));
        auto s = glm::scale(glm::mat4(1), glm::vec3((start_scale.x + scale.front()) / 100, (start_scale.y + scale.back()) / 100, 1.0));
        auto t2 = glm::translate(glm::mat4(1), glm::vec3(scale_pos.x, scale_pos.y, 0));
        trans = trans * t2 * s * t1;
      }
    }

    transform_mat_->trans.emplace_back(trans);
  }
	return true;
}

bool TransformRenderData::GenerateTransformMat(const TransformCurveEx& transform_curve) {
  auto reslution = glm::vec3(AniInfoManager::GetIns().GetWidth(), AniInfoManager::GetIns().GetHeight(), 0);

  if (!transform_mat_) return false;
  transform_mat_->trans.clear();

  auto frame_lenth = transform_mat_->clip_end - transform_mat_->clip_start + 1;

  for (auto i = 0; i < (unsigned int)frame_lenth; i++) {
    glm::mat4 trans = glm::mat4(1.0f);
    if (transform_curve.count("Position")) {
      std::vector<float> offset;
      offset.resize(2);
      auto pos_curve = transform_curve.at("Position").begin();
      if (i < pos_curve->value_map_.begin()->first)
        offset = pos_curve->value_map_.begin()->second;
      else if (i > pos_curve->value_map_.rbegin()->first)
        offset = pos_curve->value_map_.rbegin()->second;
      else
        offset = pos_curve->value_map_.at(i);
      
      trans = glm::translate(trans, glm::vec3(offset.front() / reslution.x, offset.back() / reslution.y, 0));
    }

    if (transform_curve.count("Rotation")) {
      float rot;
      auto rot_curves = transform_curve.at("Rotation");
      glm::mat4 rot_trans = glm::mat4(1.0f);
      for (auto& rot_curve : rot_curves) {
        std::vector<unsigned int> tmp_indexs = rot_curve.groups_ind;
        tmp_indexs.insert(tmp_indexs.begin(), rot_curve.layer_ind);
        glm::vec3 rot_pos;
        float start_rot;
        if (AniInfoManager::GetIns().ExistLinkTrans(tmp_indexs)) {
          rot_pos = AniInfoManager::GetIns().GetLinkTransPtrbyIndexs(tmp_indexs)->GetPosition();
          start_rot = AniInfoManager::GetIns().GetLinkTransPtrbyIndexs(tmp_indexs)->GetRotation();
        }else {
          rot_pos = glm::vec3(AniInfoManager::GetIns().GetTransPos(tmp_indexs.front()), 0);
          start_rot = AniInfoManager::GetIns().GetTransRotation(tmp_indexs.front());
        }
        rot_pos =  glm::vec3(rot_pos.x, rot_pos.y, 0) / reslution - glm::vec3(0.5, 0.5, 0);
        if (i < rot_curve.value_map_.begin()->first)
          rot = rot_curve.value_map_.begin()->second.front();
        else if (i > rot_curve.value_map_.rbegin()->first)
          rot = rot_curve.value_map_.rbegin()->second.front();
        else
          rot = rot_curve.value_map_.at(i).front();

        auto t1 = glm::translate(glm::mat4(1), -glm::vec3(rot_pos.x, rot_pos.y, 0));
        auto r = glm::rotate(glm::mat4(1), glm::radians(start_rot +  rot), glm::vec3(0, 0, 1.0));
        auto t2 = glm::translate(glm::mat4(1), glm::vec3(rot_pos.x, rot_pos.y, 0));
        trans = trans * t2 * r * t1;
      }
    }

    if (transform_curve.count("Scale")) {
      std::vector<float> scale;
      auto scale_curves = transform_curve.at("Scale");
      glm::mat4 scale_trans = glm::mat4(1.0f);
      for (auto& scale_curve : scale_curves) {
        auto tmp_indexs = scale_curve.groups_ind;
        tmp_indexs.insert(tmp_indexs.begin(), scale_curve.layer_ind);
        glm::vec3 start_scale, scale_pos;
        if (AniInfoManager::GetIns().ExistLinkTrans(tmp_indexs)) {
          scale_pos = AniInfoManager::GetIns().GetLinkTransPtrbyIndexs(tmp_indexs)->GetPosition();
          start_scale = AniInfoManager::GetIns().GetLinkTransPtrbyIndexs(tmp_indexs)->GetScale();
        }else {
          scale_pos = glm::vec3(AniInfoManager::GetIns().GetTransPos(tmp_indexs.front()), 0);
          start_scale = glm::vec3(AniInfoManager::GetIns().GetTransScale(tmp_indexs.front()), 0);
        }
        scale_pos = glm::vec3(scale_pos.x, scale_pos.y, 0) / reslution - glm::vec3(0.5, 0.5, 0);
        if (i < scale_curve.value_map_.begin()->first)
          scale = scale_curve.value_map_.begin()->second;
        else if (i > scale_curve.value_map_.rbegin()->first)
          scale = scale_curve.value_map_.rbegin()->second;
        else
          scale = scale_curve.value_map_.at(i);

        auto t1 = glm::translate(glm::mat4(1), -glm::vec3(scale_pos.x, scale_pos.y, 0));
        auto s = glm::scale(glm::mat4(1), glm::vec3((start_scale.x + scale.front()) / 100, (start_scale.y + scale.back()) / 100, 1.0));
        auto t2 = glm::translate(glm::mat4(1), glm::vec3(scale_pos.x, scale_pos.y, 0));
        trans = trans * t2 * s * t1;
      }
    }
    transform_mat_->trans.emplace_back(trans);
  }
  return true;
}

void TransformRenderData::freashGroupPositionInitVal() {
  auto transform = group_->GetTransform();
  auto base_pos = AniInfoManager::GetIns().GetTransPos(parent_layer_ind_);
  auto base_anc_pos = AniInfoManager::GetIns().GetTransAncPos(parent_layer_ind_);
  glm::vec2 offset = base_pos - base_anc_pos;
  auto parent_group = group_->parent();
  while (parent_group.lock()) {
    auto pos = parent_group.lock()->GetTransform()->GetPosition();
    auto anchor_pos = parent_group.lock()->GetTransform()->GetAnchorPos();
    offset += glm::vec2(pos.x, pos.y) - glm::vec2(anchor_pos.x, anchor_pos.y);
    parent_group = parent_group.lock()->parent();
  }
  transform->SetPosition(transform->GetPosition() + glm::vec3(offset.x, offset.y, 0));
}

void TransformRenderData::SetInandOutPos(unsigned int ind, float in_pos, float out_pos) {
	if(!transform_mat_) return;
  transform_mat_->layer_index = ind;
  auto frameRate = AniInfoManager::GetIns().GetFrameRate();
  auto duration = AniInfoManager::GetIns().GetDuration();
  transform_mat_->clip_start = in_pos * frameRate < 0 ? 0 : in_pos * frameRate;
  transform_mat_->clip_end = out_pos > duration  ? duration * frameRate : out_pos * frameRate;
}

//
void TransformRenderData::ConverCurveToCurveEx(const TransformCurve& curve1, TransformCurveEx& curve2) {
  for (auto& it : curve1) {
    auto posrelat_curves = it.second;
		std::vector<TransPropEx> trans_prop_ex_list;
    for (auto& posrelat_curve : posrelat_curves) {
      TransPropEx tran_prop_ex;
      tran_prop_ex.layer_ind = posrelat_curve.layer_ind;
      tran_prop_ex.value_map_ = posrelat_curve.value_map_;
			trans_prop_ex_list.push_back(tran_prop_ex);
    }
		curve2[it.first] = trans_prop_ex_list;
  }
}

template<typename T, typename OutT>
bool TransformRenderData::GetBezierKeyframe(const Keyframe<T>& keyframe, OutT& out, unsigned int dur, unsigned int s_t, T s_v) {
  if constexpr(std::is_same<T, glm::vec3>::value) {
    glm::vec2 lastPos_x(keyframe.lastkeyTime, keyframe.lastkeyValue.x);
    glm::vec2 lastPos_y(keyframe.lastkeyTime, keyframe.lastkeyValue.y);
    glm::vec2 outPos_x(keyframe.outPos[0].x, keyframe.outPos[0].y);
    glm::vec2 outPos_y(keyframe.outPos[1].x, keyframe.outPos[1].y);
    glm::vec2 inPos_x(keyframe.inPos[0].x, keyframe.inPos[0].y);
    glm::vec2 inPos_y(keyframe.inPos[1].x, keyframe.inPos[1].y);
    glm::vec2 curPos_x(keyframe.keyTime, keyframe.keyValue.x);
    glm::vec2 curPos_y(keyframe.keyTime, keyframe.keyValue.y);
    BezierGenerator generator_x(lastPos_x, outPos_x, inPos_x, curPos_x, dur, s_t, s_v.x);
    out.emplace_back(generator_x.GetKeyframeCurve());
    BezierGenerator generator_y(lastPos_y, outPos_y, inPos_y, curPos_y, dur, s_t, s_v.y);
    out.emplace_back(generator_y.GetKeyframeCurve());
    return true;
  }
  else if constexpr(std::is_same<T, float>::value) {
    glm::vec2 lastPos(keyframe.lastkeyTime, keyframe.lastkeyValue);
    glm::vec2 curPos(keyframe.keyTime, keyframe.keyValue);
    glm::vec2 inPos(keyframe.inPos[0]);
    glm::vec2 outPos(keyframe.outPos[0]);
    BezierGenerator generator(lastPos, outPos, inPos, curPos, dur, s_t, s_v);
    out = generator.getKeyframeCurveMap();
    return true;
  }
  return false;
}

}