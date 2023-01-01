//
// Created by ingslh on 2022/12/31.
//
#define NS_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#define MTK_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION
#include "MetalMTKViewDelegate.h"
#include "MetalRenderer.h"

MetalMTKViewDelegate::MetalMTKViewDelegate( MTL::Device* pDevice )
	: MTK::ViewDelegate()
	, _pRenderer( new MetalRenderer( pDevice ) )
{
}

MetalMTKViewDelegate::~MetalMTKViewDelegate()
{
	delete _pRenderer;
}

void MetalMTKViewDelegate::setRePathObjs(const std::vector<R2grap::RePathObj> &objs) {
	_pRenderer->setRePathObjs(objs);
}

void MetalMTKViewDelegate::drawInMTKView( MTK::View* pView )
{
	_pRenderer->draw( pView );
}