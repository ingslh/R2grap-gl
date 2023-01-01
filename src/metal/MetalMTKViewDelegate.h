//
// Created by ingslh on 2022/12/31.
//
#pragma once

#include <cassert>
#include <Metal/Metal.hpp>
#include <AppKit/AppKit.hpp>
#include <MetalKit/MetalKit.hpp>


class MetalRenderer;
class MetalMTKViewDelegate : public MTK::ViewDelegate
{
public:
		MetalMTKViewDelegate( MTL::Device* pDevice );
		virtual ~MetalMTKViewDelegate() override;
		virtual void drawInMTKView( MTK::View* pView ) override;

private:
		MetalRenderer* _pRenderer;
};

