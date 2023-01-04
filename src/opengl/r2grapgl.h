#pragma once
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>

class Shader;
namespace R2grap{
struct RePathObj;
class R2grapGl{
public:
  R2grapGl(const std::string& file_name);
  void run();

private:
	GLFWwindow* window_ = nullptr;
	Shader* shader_ = nullptr;
	std::vector<RePathObj> objs_;
	unsigned int* VBOs = nullptr;
  unsigned int* VAOs = nullptr;
	unsigned int* EBOs = nullptr;
	unsigned int window_width_;
	unsigned int window_height_;
};


}