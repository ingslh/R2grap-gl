#version 330 core
 
layout (lines_adjacency) in;
layout (line_strip, max_vertices = 256) out; //win:max support vertices num is 256

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void creatBezier(int segments){
  float delta = 1.0 / float(segments);
  int count = 0;
  for(int ind=0; ind < gl_in.length()/4; ind++){
    vec3 p0 = gl_in[4* ind + 0].gl_Position.xyz;   
    vec3 p1 = gl_in[4* ind + 1].gl_Position.xyz;   
    vec3 p2 = gl_in[4* ind + 2].gl_Position.xyz;   
    vec3 p3 = gl_in[4* ind + 3].gl_Position.xyz;  
    for (int i=0; i<=segments; ++i ){
      float t = delta * float(i); 
      vec3 p;  
      p.x = (1 - t) * (1 - t) * (1 - t) * p0.x + 3 * t * (1 - t) * (1 - t)* p1.x + 3 * t*t* (1 - t)* p2.x + t * t * t * p3.x;
      p.y = (1 - t) * (1 - t) * (1 - t) * p0.y + 3 * t * (1 - t) * (1 - t)* p1.y + 3 * t*t* (1 - t)* p2.y + t * t * t * p3.y;
      p.z = 0;
      gl_Position = projection * view * model * vec4(p, 1); 
      EmitVertex();
    }
  }
}
 
void main(){
  const int in_vert_length = gl_in.length();
  const int segments = 10;
  creatBezier(segments);
}