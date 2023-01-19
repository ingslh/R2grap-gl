#include "opengl/r2grapgl.h"
#if(WIN32)
#include "directx/R2grapdx.h"
#elif(__APPLE__)
#include "metal/R2grapMT.h"
#endif

int main(int argc, char *argv[]){
  if(argc > 2) return 0;
  if (argc == 1) {
    R2grap::R2grapGl grap("designers.json");
    grap.run();
  }
  else if(argc == 2){
#ifdef __APPLE__
		R2grap::R2grapMT grap(argv[1]);
#elifdef WIN32
		R2grap::R2grapDx grap(argv[1]);
#endif
    grap.run();
  }
}

