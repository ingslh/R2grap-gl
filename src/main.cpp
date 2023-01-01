#include <iostream>
#include <string>
#include "opengl/r2grapgl.h"
#if(WIN32)
#include "directx/d3dApp.h"
#endif
#include "metal/MetalAppDelegate.h"

int main(int argc, char *argv[]){
	if(argc > 2 || argc == 1) return 0;

#if(WIN32)
	HINSTANCE hInstance = GetModuleHandle(NULL);
#endif

	//auto grap = new R2grap::R2grapGl(std::string(argv[1]));

	NS::AutoreleasePool* pAutoreleasePool = NS::AutoreleasePool::alloc()->init();

	MetalAppDelegate del;

	NS::Application* pSharedApplication = NS::Application::sharedApplication();
	pSharedApplication->setDelegate( &del );
	pSharedApplication->run();

	pAutoreleasePool->release();

	return 0;
}

