#include "opengl/r2grapgl.h"
#if(_WIN32)
#include "directx/d3dApp.h"
#endif
#include "metal/R2grapMT.h"


int main(int argc, char *argv[]){
	if(argc > 2 || argc == 1) return 0;

#if(WIN32)
	HINSTANCE hInstance = GetModuleHandle(NULL);
#endif

	//auto grapl = new R2grap::R2grapGl(std::string(argv[1]));

	auto grap_mt = new R2grap::R2grapMT(argv[1]);
	grap_mt->run();

	return 0;
}

