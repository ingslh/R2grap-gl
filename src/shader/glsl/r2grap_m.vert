#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 transform;

void main()
{
  vec4 vert = projection * view * model * transform * vec4(aPos.x, aPos.y, aPos.z, 1.0); 
  gl_Position = vec4(vert.x, -vert.y, vert.z, 1.0); 
}