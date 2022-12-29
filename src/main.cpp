#include <iostream>
#include <string>
#include "opengl/r2grapgl.h"

int main(int argc, char *argv[]){
  if(argc > 2 || argc == 1) return 0;
  auto grap = new R2grap::R2grapGl(std::string(argv[1]));
  //grap->run();
}

