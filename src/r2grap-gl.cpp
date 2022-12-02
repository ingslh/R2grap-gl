#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <camera.h>
#include <shader_g.h>
#include "JsonReader.h"
#include "RenderDataFactory.h"
#include "RenderContent.h"
#include "AniInfoManager.h"

#include <iostream>
//#include <chrono>   
using namespace R2grap;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

// settings
const unsigned int SCR_WIDTH = 1000;
const unsigned int SCR_HEIGHT = 900;

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
  GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "R2grap", NULL, NULL);
  if (window == nullptr)
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

  JsonReader reader("../assets/thinking.json");
  auto layers_count = reader.getLayersCount();

  std::vector<std::shared_ptr<RenderContent>> contents;
  for (auto i = 0; i < layers_count; i++) {
    auto layer_info = reader.GetLayersInfo(i).get();
    contents.emplace_back(std::make_shared<RenderContent>(layer_info));
  }
	RenderContent::UpdateTransRenderData(contents);

  auto paths_count = RenderContent::GetRenderPathCount(contents);

  unsigned int* VBOs = new unsigned int[paths_count];
  unsigned int* VAOs = new unsigned int[paths_count];
  unsigned int* EBOs = new unsigned int[paths_count];
  glGenBuffers(paths_count, VBOs);
  glGenBuffers(paths_count, EBOs);
  glGenVertexArrays(paths_count, VAOs);

  //progress vertsices for VXO
  unsigned int index = 0;
  for (auto content_ind = 0; content_ind < contents.size(); content_ind++) {
    auto group_data = contents[content_ind]->GetLayerData().group_data;
    for (auto group_ind = 0; group_ind < group_data.size(); group_ind++) {
      auto path_data = group_data[group_ind].GetPathData();
      for(auto path_ind = 0; path_ind < path_data.size(); path_ind++){
        if(!path_data[path_ind].has_keyframe){
          auto vert_array = path_data[path_ind].verts;
          auto out_vert = new float[vert_array.size()];
          memcpy(out_vert, &vert_array[0], sizeof(float) * vert_array.size());

          glBindVertexArray(VAOs[index]);
          glBindBuffer(GL_ARRAY_BUFFER, VBOs[index]);
          glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vert_array.size(), out_vert, GL_STATIC_DRAW);

          if(path_data[path_ind].closed){
            auto tri_array = path_data[path_ind].tri_ind;
            auto out_tri = new unsigned int[tri_array.size()];
            memcpy(out_tri, &tri_array[0], tri_array.size() * sizeof(tri_array[0]));

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[index]);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * tri_array.size(), out_tri, GL_STATIC_DRAW);
          }
        }
        else{
          auto max_verts_size = path_data[path_ind].GetMaxVectorSize(PathData::PathVecContentType::t_Vertices);
          glBindVertexArray(VAOs[index]);
          glBindBuffer(GL_ARRAY_BUFFER, VBOs[index]);
          glBufferData(GL_ARRAY_BUFFER, sizeof(float) * max_verts_size, NULL, GL_DYNAMIC_DRAW);

          if(path_data[path_ind].closed){
            auto max_tri_size = path_data[path_ind].GetMaxVectorSize(PathData::PathVecContentType::t_TriangleIndex);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[index]);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*max_tri_size, NULL, GL_DYNAMIC_DRAW);
          }
        }
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
        glEnableVertexAttribArray(0);
				index++;
      }
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


	glEnable(GL_LINE_SMOOTH);
	GLfloat sizes[2];
	GLfloat granularity;
	glGetFloatv(GL_LINE_WIDTH_RANGE,sizes);
	GLfloat minLineWidth = sizes[0];
	GLfloat maxLineWidth = sizes[1];
	glGetFloatv(GL_LINE_WIDTH_GRANULARITY, &granularity);
	GLfloat width;
	glGetFloatv(GL_LINE_WIDTH,&width);


  static double limitFPS = 1.0 / AniInfoManager::GetIns().GetFrameRate();
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
      for (auto layer_ind = 0; layer_ind < layers_count; layer_ind++) {
        auto layer_data = contents[layer_ind]->GetLayerData();
        if (layer_data.start_pos  > static_cast<float>(played) || layer_data.end_pos < static_cast<float>(played)) continue;
        
        //glm::mat4 trans_mat = layer_data.trans[played];
        //shader.setMat4("transform", trans_mat);
        
        auto group_data = layer_data.group_data;
        for(auto group_ind = 0; group_ind < group_data.size(); group_ind++){
          auto group_trans_mat = group_data[group_ind].trans[played];
          shader.setMat4("transform", group_trans_mat);

          if(group_data[group_ind].fill){
            if(!group_data[group_ind].fill->trans_color.size())
              shader.setVec4("color", group_data[group_ind].fill->color);
            else
              shader.setVec4("color", group_data[group_ind].fill->trans_color[played]);
          }
          //need to add stroke
          if(group_data[group_ind].stroke){
            if(!group_data[group_ind].stroke->trans_color.size())
              shader.setVec4("color", group_data[group_ind].stroke->color);
            else  
              shader.setVec4("color",group_data[group_ind].stroke->trans_color[played]);
          }
          auto paths_data = group_data[group_ind].paths;
          for(auto path_ind = 0; path_ind < paths_data.size(); path_ind++){
            auto path = paths_data[path_ind];
            auto vxo_ind = RenderContent::GetPathIndex(contents, layer_ind, group_ind, path_ind);
            glBindVertexArray(VAOs[vxo_ind]);
            if(!path.has_keyframe){
              if(path.closed){
								glDrawElements(GL_TRIANGLES, path.verts.size(), GL_UNSIGNED_INT, 0);
							}
              else{
								glDrawArrays(GL_LINE_STRIP, 0, path.verts.size() / 3);
							}
							glBindVertexArray(0);
            }else{
              auto vert_vec = path.trans_verts[played];
              if (!vert_vec.size()) continue;
              auto out_vert = new float[vert_vec.size()];
              memcpy(out_vert, &vert_vec[0], sizeof(float) * vert_vec.size());
              glBindBuffer(GL_ARRAY_BUFFER, VBOs[vxo_ind]);
              glBufferSubData(GL_ARRAY_BUFFER, 0 , sizeof(float) * vert_vec.size(), out_vert);
              if(path.closed){
                auto trig_vec = path.trans_tri_ind[played];
                auto out_trig = new unsigned int[trig_vec.size()];
                memcpy(out_trig, &trig_vec[0], sizeof(unsigned int) * trig_vec.size());
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[index]);
                glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(unsigned int) * trig_vec.size(), out_trig);
                glDrawElements(GL_TRIANGLES, path.trans_verts[played].size(), GL_UNSIGNED_INT, 0);
                delete[] out_trig;
              }else
                glDrawArrays(GL_LINE_STRIP, 0, path.trans_verts[played].size() / 3);
              delete[] out_vert;
							glBindVertexArray(0);
            }
          }
        }
      }
      played = played > frame_count ? 0 : ++played;
      frames++;
      deltaTime--;
			glfwSwapBuffers(window);
    }



    // - Reset after one second
    if (glfwGetTime() - timer > 1.0) {
      timer++;
      std::cout << "FPS: " << frames << std::endl;
      frames = 0;
    }
    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
    // -------------------------------------------------------------------------------
    //glfwSwapBuffers(window);
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