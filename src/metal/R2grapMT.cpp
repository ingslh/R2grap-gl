//
// Created by ingslh on 2023/1/1.
//

#include "R2grapMT.h"
#include "JsonReader.h"
#include "MetalAppDelegate.h"

namespace R2grap{

R2grapMT::R2grapMT(const std::string &filename) {
	JsonReader reader("../assets/" + filename);
	window_width_ = AniInfoManager::GetIns().GetWidth();
	window_height_ = AniInfoManager::GetIns().GetHeight();

	auto layers_count = reader.getLayersCount();
	std::vector<std::shared_ptr<RenderContent>> contents;
	for (auto i = 0; i < layers_count; i++) {
		auto layer_info = reader.GetLayersInfo(i).get();
		contents.emplace_back(std::make_shared<RenderContent>(layer_info));
	}
	std::vector<RePathObj> path_objs;
	RenderContent::UpdateTransRenderData(contents,path_objs);



	NS::AutoreleasePool* pAutoreleasePool = NS::AutoreleasePool::alloc()->init();
	MetalAppDelegate del;

	del.setWindowSize(window_width_, window_height_);
	del.setPathObjs(path_objs);

	NS::Application* pSharedApplication = NS::Application::sharedApplication();
	pSharedApplication->setDelegate( &del );
	pSharedApplication->run();
	pAutoreleasePool->release();
}

}
