//
// Created by ingslh on 2022/12/31.
//
#pragma once

#include <Metal/Metal.hpp>
#include <AppKit/AppKit.hpp>
#include <MetalKit/MetalKit.hpp>

class MetalMTKViewDelegate;
class MetalAppDelegate : public NS::ApplicationDelegate
{
public:
		~MetalAppDelegate();

		NS::Menu* createMenuBar();

		virtual void applicationWillFinishLaunching( NS::Notification* pNotification ) override;
		virtual void applicationDidFinishLaunching( NS::Notification* pNotification ) override;
		virtual bool applicationShouldTerminateAfterLastWindowClosed( NS::Application* pSender ) override;

private:
		NS::Window* _pWindow;
		MTK::View* _pMtkView;
		MTL::Device* _pDevice;
		MetalMTKViewDelegate* _pViewDelegate = nullptr;
};

