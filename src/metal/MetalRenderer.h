//
// Created by ingslh on 2022/12/31.
//

#pragma once
#include <Metal/Metal.hpp>
#include <AppKit/AppKit.hpp>
#include <MetalKit/MetalKit.hpp>
#include <glm/glm.hpp>

namespace shader_types
{
		struct InstanceData{
				glm::mat4 instanceTransform;
				glm::vec4 instanceColor;
		};

		struct CameraData{
				glm::mat4 perspectiveTransform;
				glm::mat4 worldTransform;
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

private:
		MTL::Device* _pDevice;
		MTL::CommandQueue* _pCommandQueue;
		MTL::Library* _pShaderLibrary;
		MTL::RenderPipelineState* _pPSO;
		MTL::DepthStencilState* _pDepthStencilState;
		MTL::Buffer* _pVertexDataBuffer;
		MTL::Buffer* _pInstanceDataBuffer[kMaxFramesInFlight];
		MTL::Buffer* _pCameraDataBuffer[kMaxFramesInFlight];
		MTL::Buffer* _pIndexBuffer;
		float _angle;
		int _frame;
		dispatch_semaphore_t _semaphore;
		static const int kMaxFramesInFlight;
};


