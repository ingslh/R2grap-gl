#include "r2grap.h"
#include <camera.h>
#include <shader_g.h>
#include <codec/JsonReader.h>

namespace R2grap{
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
// timing
float input_deltaTime = 0.0f;	// time between current frame and last frame 
  
R2grap::R2grap(const std::string& filename){

  Camera camera(glm::vec3(0.0f, 0.0f, 1.0f)); 
  JsonReader reader("../assets/" + filename);
  unsigned int SCR_WIDTH = AniInfoManager::GetIns().GetWidth();
  unsigned int SCR_HEIGHT = AniInfoManager::GetIns().GetHeight();

  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

// glfw window creation
  // --------------------
  GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "R2grap", NULL, NULL);
  if (window == nullptr){
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return;
  }
  glfwMakeContextCurrent(window);
  //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); //disable the mouse
  // glad: load all OpenGL function pointers
  // ---------------------------------------
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
    std::cout << "Failed to initialize GLAD" << std::endl;
    return;
  }

  // configure global opengl state
  // -----------------------------
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

}

}