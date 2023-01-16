//
// Created by ingslh on 2022/12/31.
//
#pragma once
#include <vector>
#include <Metal/Metal.hpp>
#include <AppKit/AppKit.hpp>
#include <MetalKit/MetalKit.hpp>
#include "PathRenderData.h"

class MetalMTKViewDelegate;
class MetalAppDelegate : public NS::ApplicationDelegate
{
public:
		~MetalAppDelegate();

		NS::Menu* createMenuBar();
		virtual void applicationWillFinishLaunching( NS::Notification* pNotification ) override;
		virtual void applicationDidFinishLaunching( NS::Notification* pNotification ) override;
		virtual bool applicationShouldTerminateAfterLastWindowClosed( NS::Application* pSender ) override;

		void setWindowSize(unsigned width, unsigned height);
		void setPathObjs(const std::vector<R2grap::RePathObj>& objs);
		void setFrameCount(unsigned count);

private:
		NS::Window* _pWindow;
		MTK::View* _pMtkView;
		MTL::Device* _pDevice;
		MetalMTKViewDelegate* _pViewDelegate = nullptr;

		unsigned window_width_ = 0;
		unsigned window_height_ = 0;
		unsigned frame_count_ = 0;
		std::vector<R2grap::RePathObj> path_objs_;
};

