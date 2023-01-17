#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <map>

using namespace glm;

namespace R2grap{

struct BezierCluster{
  vec2 start;
  vec2 out;
  vec2 in;
  vec2 end;
};

template<typename T>
class LinearGenerator{
public:
  LinearGenerator(const std::vector<T>& last_keyframe_vert, unsigned int last_keyframe_time, 
  const std::vector<T>& keyframe_vert, unsigned int keyframe_time){
    for(auto i = last_keyframe_time; i <= keyframe_time; i++){
      std::vector<T> tmp_verts;
      for(auto j = 0; j < last_keyframe_vert.size(); j++){
        tmp_verts.emplace_back(last_keyframe_vert[j] + float((i - last_keyframe_time))/(keyframe_time - last_keyframe_time) * (keyframe_vert[j] - last_keyframe_vert[j]));
      }
      linear_map_[i] = tmp_verts;
    }
  }

  const std::map<unsigned int, std::vector<T>>& GetLinearMap()const {return linear_map_;}
  
  const std::map<unsigned int, T> GetLinearMapToSignal()const {
    std::map<unsigned int, T> ret;
    for (const auto& el : linear_map_) {
      if (el.second.size() == 1) {
        std::pair<unsigned int, T> tmp_pair ((unsigned int)el.first, (T)el.second[0]);
        ret.insert(tmp_pair);
      }
    }
    return ret;
  }

private:
  std::map<unsigned int, std::vector<T>> linear_map_;
};


class BezierGenerator{
public:
  //generate closed curve
  BezierGenerator(const std::vector<vec2>& vert, const std::vector<vec2>& out, const std::vector<vec2>& in, bool closed){
    auto vert_nums = vert.size();
    std::vector<BezierCluster> cluster_list;
    for(size_t i = 0; i < vert_nums; i++){
      BezierCluster cluster;
      cluster.start = vert[i];
      cluster.out = out[i] + cluster.start;

      if(i == vert_nums -1 && !closed) break;

      if(i == vert_nums -1){
        cluster.end = vert[0];
        cluster.in = in[0] + cluster.end;
      }
      else{
        cluster.end = vert[i + 1];
        cluster.in = in[i + 1] + cluster.end;
      }
      cluster_list.emplace_back(cluster);
    }

    for(auto& it : cluster_list){
      if(it.in.x == 0 && it.in.y == 0 && it.out.x ==0 && it.out.y == 0)
        bezier_verts_.emplace_back(it.start);
      else{
        auto dis = (unsigned int)glm::distance(it.end, it.start);
        if (dis < 5) {
          default_segments_ = 5;
        }
        else if (dis>=5 && dis <= 10) {
          default_segments_ =  10;
        }
        else if (dis > 10) {
          default_segments_ = 15;
        }

        auto delta = 1.0/float(default_segments_);
        for(unsigned int i = 0; i <= default_segments_; i++){
          auto t = static_cast<float>(delta * float(i));
          vec2 p;
          p.x = (1 - t) * (1 - t) * (1 - t) * it.start.x + 3 * t * (1 - t) * (1 - t)* it.out.x + 3 * t*t* (1 - t)* it.in.x + t * t * t * it.end.x;
          p.y = (1 - t) * (1 - t) * (1 - t) * it.start.y + 3 * t * (1 - t) * (1 - t)* it.out.y + 3 * t*t* (1 - t)* it.in.y + t * t * t * it.end.y;
          bezier_verts_.emplace_back(p);
        }
      }
    }
  }

  BezierGenerator(const BezierCluster& cluster) {
    auto dis = (unsigned int)glm::distance(cluster.end, cluster.start);
    if (dis < 5) {
      default_segments_ = 5;
    }
    else if (dis >= 5 && dis <= 10) {
      default_segments_ = 10;
    }
    else if (dis > 10) {
      default_segments_ = 15;
    }

    auto delta = 1.0 / float(default_segments_);
    for (unsigned int i = 0; i <= default_segments_; i++) {
      auto t = static_cast<float>(delta * float(i));
      vec2 p;
      p.x = (1 - t) * (1 - t) * (1 - t) * cluster.start.x + 3 * t * (1 - t) * (1 - t)* cluster.out.x + 3 * t*t* (1 - t)* cluster.in.x + t * t * t * cluster.end.x;
      p.y = (1 - t) * (1 - t) * (1 - t) * cluster.start.y + 3 * t * (1 - t) * (1 - t)* cluster.out.y + 3 * t*t* (1 - t)* cluster.in.y + t * t * t * cluster.end.y;
      bezier_verts_.emplace_back(p);
    }
  }

  BezierGenerator(const BezierCluster& cluster, unsigned int segments, unsigned int start, float start_value){
    vec2 curPos = cluster.end, outPos = cluster.out, inPos = cluster.in, lastPos = cluster.start;
    for (unsigned int i = start; i <= segments + start; i++) {
      std::vector<float> ret;
      std::vector<float> elements;
      elements.emplace_back(curPos.x - 3 * inPos.x + 3 * outPos.x - lastPos.x);
      elements.emplace_back(3 * inPos.x - 6 * outPos.x + 3 * lastPos.x);
      elements.emplace_back(3 * outPos.x - 3 * lastPos.x);
      elements.emplace_back(lastPos.x - i);
      CubicPolynomial(0, 1, elements, ret);
      if (ret.size() == 1) {
        auto t = ret[0];
        auto p = (1 - t) * (1 - t) * (1 - t) * lastPos.y + 3 * t * (1 - t) * (1 - t)* outPos.y + 3 * t*t* (1 - t)* inPos.y + t * t * t * curPos.y;
        keyframe_curve_.emplace_back(glm::vec2(i, round((p - start_value)*100)/100));
      }
    }
  }

  BezierGenerator(const vec2& lastPos, const vec2& outPos, const vec2& inPos, const vec2& curPos, 
                  unsigned int segments, unsigned int start, float start_value){
    for (unsigned int i = start; i <= segments + start; i++) {
      std::vector<float> ret;
      std::vector<float> elements;
      elements.emplace_back(curPos.x - 3 * inPos.x + 3 * outPos.x - lastPos.x);
      elements.emplace_back(3 * inPos.x - 6 * outPos.x + 3 * lastPos.x);
      elements.emplace_back(3 * outPos.x - 3 * lastPos.x);
      elements.emplace_back(lastPos.x - i);
      CubicPolynomial(0, 1, elements, ret);
      if (ret.size() == 1) {
        auto t = ret[0];
        auto p = (1 - t) * (1 - t) * (1 - t) * lastPos.y + 3 * t * (1 - t) * (1 - t)* outPos.y + 3 * t*t* (1 - t)* inPos.y + t * t * t * curPos.y;
        keyframe_curve_.emplace_back(glm::vec2(i, round((p - start_value)*100)/100));
      }
    }
  }

  static bool MergeKeyframeCurve(const BezierGenerator& gen1, const BezierGenerator& gen2, std::map<unsigned int, std::vector<float>>& out_curve) {
    auto curve1 = const_cast<BezierGenerator&>(gen1).GetKeyframeCurve();
    auto curve2 = const_cast<BezierGenerator&>(gen2).GetKeyframeCurve();
    if (curve1.size() != curve2.size()) return false;
    for (auto i = 0; i < curve1.size(); i++) {
      if (curve1[i].x != curve2[i].x) return false;
      out_curve[static_cast<unsigned>(curve1[i].x)] = { curve1[i].y, curve2[i].y };
    }
		return true;
  }

  static bool MergeKeyframeCurve(const std::vector<glm::vec2>& curve1, const std::vector<glm::vec2>& curve2, std::map<unsigned int, std::vector<float>>& out_curve) {
    if (curve1.size() != curve2.size()) return false;
    for (auto i = 0; i < curve1.size(); i++) {
      if (curve1[i].x != curve2[i].x) return false;
      out_curve[static_cast<unsigned>(curve1[i].x)] = { curve1[i].y, curve2[i].y };
    }
    return true;
  }


public:
  const std::vector<vec2>& getKeyframeCurve() const {return keyframe_curve_;}
  const std::vector<vec2>& getBezierVerts() const {return bezier_verts_;}

  const std::map<unsigned int, float> getKeyframeCurveMap() const {
    std::map<unsigned int, float> ret;
    for (auto &el : keyframe_curve_) {
      ret[static_cast<unsigned int>(el.x)] = el.y;
    }
    return ret;
  }

  static bool MergeCurves(const std::vector<glm::vec2>& curve1, const std::vector<glm::vec2>& curve2, std::map<unsigned int, std::vector<float>>& out_curve) {
    if (curve1.size() != curve2.size()) return false;

    for (auto i = 0; i < curve1.size(); ++i) {
      if (curve1[i].x != curve2[i].x) return false;
      out_curve[static_cast<unsigned>(curve1[i].x)] = { curve1[i].y, curve2[i].y };
    }
    return true;
  }

  static bool ConverCurve(const std::vector<glm::vec2>& curve1, std::map<unsigned int, std::vector<float>>& out_curve) {
    for (auto i = 0; i < curve1.size(); ++i) {
      out_curve[static_cast<unsigned>(curve1[i].x)] = { curve1[i].y };
    }
    return true;
  }

  const std::vector<vec2>& GetKeyframeCurve() { return keyframe_curve_; }

private:
  void CubicPolynomial(float l, float r, const std::vector<float>& elements, std::vector<float>& ret) {
    auto a = elements[0], b = elements[1], c = elements[2], d = elements[3];
    auto f = [a, b, c, d](float x)->float {
      return ((a*x + b)*x + c)*x + d;
    };
    if (f(l)*f(r) > 0 && ((r - 1) < 1)) return;
    float mid = (l + r) / 2;
    if (f(mid) <= 1e-4 && f(mid) >= -1e-4) {
      ret.emplace_back(static_cast<float>(round(mid * pow(10,3)) * pow(10,-3)));
      return;
    }
    CubicPolynomial(l, mid, elements, ret), CubicPolynomial(mid, r, elements, ret);
  }

private:
  std::vector<vec2> bezier_verts_;
  std::vector<vec2> keyframe_curve_;
  unsigned int default_segments_ = 10;
};
}