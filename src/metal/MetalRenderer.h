//
// Created by ingslh on 2022/12/31.
//

#pragma once
#include <Metal/Metal.hpp>
#include <AppKit/AppKit.hpp>
#include <MetalKit/MetalKit.hpp>
#include <glm/glm.hpp>
#include "PathRenderData.h"

namespace shader_types
{
		struct InstanceData{
				glm::mat4 instanceTransform;
				glm::vec4 instanceColor;
		};

		struct CameraData{
				glm::mat4 perspectiveTransform;
				glm::mat4 viewTransform;
				glm::mat4 modelTransform;
		};
}
static constexpr size_t kMaxFramesInFlight = 3;
class MetalRenderer {
public:
		MetalRenderer( MTL::Device* pDevice );
		~MetalRenderer();
		void buildShaders();
		void buildDepthStencilStates();
		void buildBuffers();
		void draw( MTK::View* pView );

		void setCameraData();
		void setScrSize(unsigned width, unsigned height);
		void setRePathObjs(const std::vector<R2grap::RePathObj> objs);
		void setFrameCount(unsigned count);
		void buildBuffers(const R2grap::RePathObj& obj);
		void drawPathObjs(MTK::View* pView);

private:
		MTL::Device* _pDevice;
		MTL::CommandQueue* _pCommandQueue;
		MTL::Library* _pShaderLibrary;
		MTL::RenderPipelineState* _pPSO;
		MTL::DepthStencilState* _pDepthStencilState;
		MTL::Buffer* _pVertexDataBuffer;
		MTL::Buffer* _pInstanceDataBuffer;
		MTL::Buffer* _pCameraDataBuffer;
		MTL::Buffer* _pIndexBuffer;
		float _angle;
		int _frame;
		dispatch_semaphore_t _semaphore;
		static const int kMaxFramesInFlight;

		unsigned played_ = 0;
		unsigned frame_count_;
		unsigned scr_width_ = 0;
		unsigned scr_height_ = 0;
		std::vector<R2grap::RePathObj> path_objs_;
		std::vector<MTL::Buffer*> pVertDataBufferList_;//size = objs's size
		std::map<unsigned, MTL::Buffer*> pIndexBufferList_;//size != objs's size(path maybe not closed)
		std::vector<MTL::Buffer*> pInstanceDataBufferList_;//size = objs's size
		MTL::Buffer* pCameraDataBuffer_;
};


