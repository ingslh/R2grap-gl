#include "opengl/r2grapgl.h"
#if(WIN32)
#include "directx/d3dApp.h"
#endif
#include "metal/R2grapMT.h"



int main(int argc, char *argv[]){
	if(argc > 2 || argc == 1) return 0;

#if(WIN32)
	HINSTANCE hInstance = GetModuleHandle(NULL);
#endif

	//R2grap::R2grapGl grapgl(std::string(argv[1]));

	R2grap::R2grapMT grap_mt(argv[1]);
	grap_mt.run();

	return 0;
}

