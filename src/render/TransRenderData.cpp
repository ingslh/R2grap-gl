#include "TransRenderData.h"
#include "BezierGen.hpp"
#include "AniInfoManager.h"
#include <glm/gtc/matrix_transform.hpp>

namespace R2grap{

TransformRenderData::TransformRenderData(const LayersInfo* layer) : keyframe_mat_(layer->GetShapeTransform()->GetKeyframeData()){
  auto transform = layer->GetShapeTransform();
  TransformCurve transform_curve;
  CompTransformCurve(transform.get(),transform_curve);

  transform_mat_ = new TransMat();
  SetInandOutPos(layer->GetLayerInd(), layer->GetLayerInpos(), layer->GetLayerOutpos());
  GenerateTransformMat(transform_curve, transform.get());
}


TransformRenderData::TransformRenderData(const Transform* transform, unsigned int ind, float inpos, float outpos){
  auto tmp_trans = const_cast<Transform*>(transform);
  keyframe_mat_ = transform->GetKeyframeData();
  transform_mat_ = new TransMat();
  SetInandOutPos(ind, inpos, outpos);
  TransformCurve transform_curve;
  CompTransformCurve(tmp_trans, transform_curve);
  GenerateTransformMat(transform_curve, tmp_trans);
}

void TransformRenderData::CompTransformCurve(Transform* trans, TransformCurve& curve){
    for (auto it = keyframe_mat_.begin(); it != keyframe_mat_.end(); it++){
    if(trans->IsVectorProperty(it->first)){ //vector
      auto vector_keyframes = std::get<t_Vector>(it->second);
      auto start_value =  vector_keyframes[0].lastkeyValue;
      auto start = vector_keyframes.front().lastkeyTime;
      std::vector<std::map<unsigned int, float>> double_curve_line;
      double_curve_line.resize(2);

      for (auto& keyframe : vector_keyframes) {
        unsigned int bezier_duration = static_cast<unsigned int>(keyframe.keyTime - keyframe.lastkeyTime);
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
      curve[it->first] = double_curve_line;
    }
    else{ //scalar
      auto scalar_keyframes = std::get<t_Scalar>(it->second);
      auto start_value = it->first == "Rotation" ? 0.0 : scalar_keyframes[0].lastkeyValue;

      unsigned int start = static_cast<unsigned int>(scalar_keyframes.front().lastkeyTime);
      std::vector<std::map<unsigned int, float>> signal_curve_line;
      signal_curve_line.resize(1);

      for (auto& keyframe : scalar_keyframes) {
        unsigned int bezier_duration = static_cast<unsigned int>(keyframe.keyTime - keyframe.lastkeyTime);
        glm::vec2 lastPos(keyframe.lastkeyTime, keyframe.lastkeyValue);
        glm::vec2 curPos(keyframe.keyTime, keyframe.keyValue);
        glm::vec2 inPos(keyframe.inPos[0]);
        glm::vec2 outPos(keyframe.outPos[0]);
        BezierGenerator generator(lastPos, outPos, inPos, curPos, bezier_duration, start, start_value);
        auto curve = generator.getKeyframeCurveMap();
        start += static_cast<unsigned int>(curve.size());

        signal_curve_line[0].merge(curve);//cpp17 support,if old cpp verison can use "signal_curve_line[0].insert(curve.begin(),curve.end());" 
      }
      curve[it->first] = signal_curve_line;
    }
  }
}

void TransformRenderData::GenerateTransformMat(const TransformCurve& transform_curve, Transform* transform){
  auto reslution = glm::vec3(AniInfoManager::GetIns().GetWidth(), AniInfoManager::GetIns().GetHeight(), 0);
  auto position = transform->GetPosition() / reslution - glm::vec3(0.5,0.5,0);
  auto frame_lenth = transform_mat_->clip_end - transform_mat_->clip_start + 1;
  for (unsigned int i = 0; i < frame_lenth; i++){
    glm::mat4 trans = glm::mat4(1.0f);
    if (transform_curve.count("Position")){
      std::vector<float> offset;
      offset.resize(2);
      for (auto j = 0; j < offset.size(); j++) {
        if(i < transform_curve.at("Position")[j].begin()->first)
          offset[j] = transform_curve.at("Position")[j].begin()->second;
        else if( i > transform_curve.at("Position")[j].rbegin()->first)
          offset[j] = transform_curve.at("Position")[j].rbegin()->second;
        else
          offset[j] = transform_curve.at("Position")[j].at(i);
      }
      trans = glm::translate(trans, glm::vec3(offset.front()/reslution.x, offset.back()/reslution.y, 0));
    }

    if (transform_curve.count("Rotation")) {
      float rot;
      if (i < transform_curve.at("Rotation").front().begin()->first)
        rot = transform_curve.at("Rotation").front().begin()->second;
      else if(i > transform_curve.at("Rotation").front().rbegin()->first)
        rot = transform_curve.at("Rotation").front().rbegin()->second;
      else
        rot = transform_curve.at("Rotation").front().at(i);
      auto t1 = glm::translate(glm::mat4(1), -glm::vec3(position.x, position.y, 0));
      auto r = glm::rotate(glm::mat4(1), glm::radians(-rot), glm::vec3(0, 0, 1.0));//ae use left-hand CS ,but opengl us right-hand CS(coordinate system)
      auto t2 = glm::translate(glm::mat4(1), glm::vec3(position.x, position.y, 0));
      trans = trans * t2 * r * t1;
    }

    if (transform_curve.count("Scale")) {
      std::vector<float> scale;
      scale.resize(2);
      for (auto j = 0; j < scale.size(); j++) {
        if (i < transform_curve.at("Scale")[j].begin()->first)
          scale[j] = transform_curve.at("Scale")[j].begin()->second;
        else if (i > transform_curve.at("Scale")[j].rbegin()->first)
          scale[j] = transform_curve.at("Scale")[j].rbegin()->second;
        else
          scale[j] = transform_curve.at("Scale")[j].at(i);
      }
      trans = glm::scale(trans, glm::vec3(scale.front() / 100, scale.back() / 100, 1.0));
    }
    transform_mat_->trans.emplace_back(trans);
  }
}

void TransformRenderData::SetInandOutPos(unsigned int ind, float in_pos, float out_pos) {
  transform_mat_->layer_index = ind;
  auto frameRate = AniInfoManager::GetIns().GetFrameRate();
  auto duration = AniInfoManager::GetIns().GetDuration();
  transform_mat_->clip_start = in_pos * frameRate < 0 ? 0 : in_pos * frameRate;
  transform_mat_->clip_end = out_pos > duration  ? duration * frameRate : out_pos * frameRate;
}


}