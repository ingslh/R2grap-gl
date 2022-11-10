#pragma once
#include "LayersInfo.h"
#include "Transform.h"
#include "RenderDataFactory.h"
#include <glm/glm.hpp>
#include <glad/glad.h>

namespace R2grap{

struct LayerData{
  unsigned int index;
  float start_pos;
  float end_pos;
  std::vector<glm::mat4> trans;

  unsigned int paths_num;
  std::vector<std::vector<float>> verts;
  std::vector<std::vector<unsigned int>> triangle_ind;

  std::vector<glm::vec4> color;
};

class RenderContent{
public:
  RenderContent(LayersInfo* layer_info);

  static unsigned int GetRenderPathCount(const std::vector<std::shared_ptr<RenderContent>>& contents);

  static unsigned int GetPathIndex(const std::vector<std::shared_ptr<RenderContent>>& contents, unsigned int layer_ind, unsigned int path_ind);


  LayerData& GetLayerData(){return layer_data_;}
  
  
private:
  LayerData layer_data_;
};

}