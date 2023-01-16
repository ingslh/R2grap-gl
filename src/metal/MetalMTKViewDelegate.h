//
// Created by ingslh on 2022/12/31.
//
#pragma once

#include <cassert>
#include <Metal/Metal.hpp>
#include <AppKit/AppKit.hpp>
#include <MetalKit/MetalKit.hpp>
#include "PathRenderData.h"

class MetalRenderer;
class MetalMTKViewDelegate : public MTK::ViewDelegate
{
public:
		MetalMTKViewDelegate( MTL::Device* pDevice );
		virtual ~MetalMTKViewDelegate() override;
		virtual void drawInMTKView( MTK::View* pView ) override;

		void setRePathObjs(const std::vector<R2grap::RePathObj>& objs);
		void setScrSize(unsigned width, unsigned height);
		void setFrameCount(unsigned count);

private:
		MetalRenderer* _pRenderer;
		unsigned scr_width_ = 0;
		unsigned scr_height_ = 0;
};

