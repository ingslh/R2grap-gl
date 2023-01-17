#include <iostream>
#include <string>
#include <memory>
#include "opengl/r2grapgl.h"
#if(WIN32)
#include "directx/R2grapdx.h"
#elif(__APPLE__)
#include "metal/R2grapMT.h"
#endif

int main(int argc, char *argv[]){
  if(argc > 2) return 0;
  if (argc == 1) {
    R2grap::R2grapDx grapdx("designers.json");
    grapdx.run();
  }
  else if(argc == 2){
    R2grap::R2grapDx grapdx(argv[1]);
    grapdx.run();
  }
}

