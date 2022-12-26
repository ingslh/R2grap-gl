#pragma once
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

namespace R2grap{

class R2grap{
public:
  R2grap(const std::string& file_name);
  void run();
};


}