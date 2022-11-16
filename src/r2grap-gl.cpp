#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <camera.h>
#include <shader_g.h>
#include "JsonReader.hpp"
#include "RenderDataFactory.h"
#include "RenderContent.h"
#include "AniInfoManager.h"

#include <iostream>
using namespace R2grap;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

// settings
const unsigned int SCR_WIDTH = 1000;
const unsigned int SCR_HEIGHT = 1000;

Camera camera(glm::vec3(0.0f, 0.0f, 0.9f));

int main()
{
  // glfw: initialize and configure
  // ------------------------------
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  // glfw window creation
  // --------------------
  GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
  if (window == NULL)
  {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);

  // glad: load all OpenGL function pointers
  // ---------------------------------------
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  // configure global opengl state
  // -----------------------------
  glEnable(GL_DEPTH_TEST);

  // build and compile shaders
  // -------------------------
  Shader shader("../src/shader/r2grap_m.vert", "../src/shader/r2grap_m.frag");

  // set up vertex data (and buffer(s)) and configure vertex attributes
  // ------------------------------------------------------------------

  JsonReader reader("../assets/test/path_test.json");
  //JsonReader reader("../assets/test.json");
  auto layers_count = reader.getLayersCount();

  std::vector<std::shared_ptr<RenderContent>> contents;
  for (auto i = 0; i < layers_count; i++) {
    auto layer_info = reader.GetLayersInfo(i).get();
    contents.emplace_back(std::make_shared<RenderContent>(layer_info));
  }

  auto paths_count = RenderContent::GetRenderPathCount(contents);

  unsigned int* VBOs = new unsigned int[paths_count];
  unsigned int* VAOs = new unsigned int[paths_count];
  unsigned int* EBOs = new unsigned int[paths_count];

  glGenBuffers(paths_count, VBOs);
  glGenBuffers(paths_count, EBOs);
  glGenVertexArrays(paths_count, VAOs);
  unsigned int index = 0;
  for (auto i = 0; i < contents.size(); i++) {
    for (auto j = 0; j < contents[i]->GetLayerData().paths_num; j++) {
      auto vert_array = contents[i]->GetLayerData().verts[j];
      auto out_vert = new float[vert_array.size()];
      memcpy(out_vert, &vert_array[0], vert_array.size() * sizeof(vert_array[0]));

      auto tri_array = contents[i]->GetLayerData().triangle_ind[j];
      auto out_tri_ind = new unsigned int[tri_array.size()];
      memcpy(out_tri_ind, &tri_array[0], tri_array.size() * sizeof(tri_array[0]));

      glBindVertexArray(VAOs[index]);
      glBindBuffer(GL_ARRAY_BUFFER, VBOs[index]);
      glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vert_array.size(), out_vert, GL_STATIC_DRAW);

      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[index]);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * tri_array.size(), out_tri_ind, GL_STATIC_DRAW);

      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
      glEnableVertexAttribArray(0);
      index++;
    }
  }

  glfwSwapInterval(1);// open the vertical synchronization
  // draw points
  shader.use();
  glm::mat4 projection = glm::perspective(45.0f, (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
  glm::mat4 view = camera.GetViewMatrix();
  glm::mat4 model = glm::mat4(1.0f);
  shader.setMat4("projection", projection);
  shader.setMat4("view", view);
  shader.setMat4("model", model);

  // draw in wireframe
  // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  static double limitFPS = 1.0 / 60.0;
  double lastTime = glfwGetTime(), timer = lastTime;
  double deltaTime = 0, nowTime = 0;
  int frames = 0, played = 0;

  auto frame_count = AniInfoManager::GetIns().GetDuration() * AniInfoManager::GetIns().GetFrameRate();

  // render loop
  // -----------
  while (!glfwWindowShouldClose(window))
  {
    // - Measure time
    nowTime = glfwGetTime();
    deltaTime += (nowTime - lastTime) / limitFPS;
    lastTime = nowTime;

    while (deltaTime >= 1.0) { // render
      glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      for (auto i = 0; i < layers_count; i++) {
        auto layer_data = contents[i]->GetLayerData();
        if (layer_data.start_pos  > static_cast<float>(played) || layer_data.end_pos < static_cast<float>(played)) continue;
        glm::mat4 trans_mat = layer_data.trans[played];
        //glm::mat4 trans_mat = glm::mat4(1.0f);
        shader.setMat4("transform", trans_mat);
        for (auto j = 0; j < layer_data.paths_num; j++) {
          shader.setVec4("color", layer_data.color[j]);
          glBindVertexArray(VAOs[RenderContent::GetPathIndex(contents, i, j)]);
          glDrawElements(GL_TRIANGLES, layer_data.verts[j].size(), GL_UNSIGNED_INT, 0);
        }
      }
      played++;
      frames++;
      deltaTime--;
      if (played > frame_count) played = 0;
    }

    // - Reset after one second
    if (glfwGetTime() - timer > 1.0) {
      timer++;
      std::cout << "FPS: " << frames << std::endl;
      frames = 0;
    }
    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
    // -------------------------------------------------------------------------------
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // optional: de-allocate all resources once they've outlived their purpose:
  // ------------------------------------------------------------------------
  glDeleteVertexArrays(paths_count, VAOs);
  glDeleteBuffers(paths_count, VBOs);
  glDeleteBuffers(paths_count, EBOs);

  glfwTerminate();
  return 0;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
  // make sure the viewport matches the new window dimensions; note that width and 
  // height will be significantly larger than specified on retina displays.
  glViewport(0, 0, width, height);
}