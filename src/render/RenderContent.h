#pragma once
#include "LayersInfo.h"
#include "Transform.h"
#include "RenderDataFactory.h"
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <map>

namespace R2grap{

struct PathData {
  bool closed;
  bool has_keyframe;
  std::vector<float> verts;//path's vertices, can have mulitity paths;
  std::vector<unsigned int> tri_ind;//for gerenate EBO array

  //key is frame index
  std::map<unsigned int, std::vector<float>> trans_verts; 
  std::map<unsigned int, std::vector<unsigned int>> trans_tri_ind;

  //traverse all frames to get the max vector
  enum PathVecContentType{
    t_Vertices = 0,
    t_TriangleIndex,
  };
  int GetMaxVectorSize(PathVecContentType type){
    auto size = type == t_Vertices ? trans_verts.size() : trans_tri_ind.size();
    if(!size) return -1;
    int max_vec_size = 0;
    if(type == PathVecContentType::t_Vertices){
      for(auto& trans_vert : trans_verts){
        max_vec_size = trans_vert.second.size() > max_vec_size ? trans_vert.second.size() : max_vec_size;
      }
    }else{
      for(auto& trans_tri : trans_tri_ind){
        max_vec_size = trans_tri.second.size() > max_vec_size ? trans_tri.second.size() : max_vec_size;
      }
    }
    return max_vec_size;
  }
};

struct FillData {
  glm::vec4 color;
  unsigned int opacity = 100;

  //key is frame index
  std::map<unsigned int, glm::vec4> trans_color;
  std::map<unsigned int, unsigned int> trans_opacity;

  FillData(glm::vec4 color, unsigned int opacity):color(color),opacity(opacity){}

  FillData():color(glm::vec4()){}
};

struct StrokeData {
  glm::vec4 color;
  unsigned int opacity;
  float stroke_wid;
  float miter_limit;

  //key is frame index
  std::map<unsigned int, glm::vec4> trans_color;
  std::map<unsigned int, unsigned int> trans_opacity;
  std::map<unsigned int, float> trans_stroke_wid;
  std::map<unsigned int, float> trans_miter_limit;

  StrokeData(glm::vec4 color, unsigned int opacity, float stroke_wid):
    color(color), opacity(opacity), stroke_wid(stroke_wid){}

  StrokeData():color(glm::vec4()){}
};

struct GroupData {
  std::vector<PathData> paths;
  std::shared_ptr<FillData> fill = nullptr;
  std::shared_ptr<StrokeData> stroke = nullptr;

  std::vector<glm::mat4> trans; //Index is frameNum

  const std::vector<PathData>& GetPathData() const {return paths;}
};

struct LayerData{
  unsigned int index;
  float start_pos;
  float end_pos;
  unsigned int link_layer_index;
  std::vector<GroupData> group_data;
  
  std::vector<glm::mat4> trans;//Index is frameNum

  //old
  unsigned int paths_num;
  std::vector<std::vector<float>> verts;
  std::vector<std::vector<unsigned int>> triangle_ind;
  std::vector<glm::vec4> color;
};

class RenderContent{
public:
  RenderContent(LayersInfo* layer_info);

  static unsigned int GetRenderPathCount(const std::vector<std::shared_ptr<RenderContent>>& contents);

  //static unsigned int GetPathIndex(const std::vector<std::shared_ptr<RenderContent>>& contents, unsigned int layer_ind, unsigned int path_ind);

  static unsigned int GetPathIndex(const std::vector<std::shared_ptr<RenderContent>>& contents, unsigned int layer_ind, unsigned int group_ind, unsigned int path_ind);

  const LayerData& GetLayerData()const {return layer_data_;}

private:
  const std::vector<GroupData>& GetGroupData()const {return layer_data_.group_data;}
  
  
private:
  LayerData layer_data_;
};

}