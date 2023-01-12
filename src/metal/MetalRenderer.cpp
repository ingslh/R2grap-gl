//
// Created by ingslh on 2022/12/31.
//
#include "MetalRenderer.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <camera.h>
#include <simd/simd.h>

const int MetalRenderer::kMaxFramesInFlight = 1;
static constexpr size_t kNumInstances = 32;

MetalRenderer::MetalRenderer( MTL::Device* pDevice )
	: _pDevice( pDevice->retain() )
	, _angle ( 0.f )
	, _frame( 0 )
{
	_pCommandQueue = _pDevice->newCommandQueue();
	buildShaders();
	buildDepthStencilStates();

	//_semaphore = dispatch_semaphore_create( 1 );
}

MetalRenderer::~MetalRenderer()
{
	_pShaderLibrary->release();
	_pDepthStencilState->release();
	_pVertexDataBuffer->release();
	_pInstanceDataBuffer->release();
	_pCameraDataBuffer->release();

	_pIndexBuffer->release();
	_pPSO->release();
	_pCommandQueue->release();
	_pDevice->release();
}

void MetalRenderer::setRePathObjs(const std::vector<R2grap::RePathObj> objs) {
	path_objs_ = objs;
	//for(auto& obj : path_objs_)
	//	buildBuffers(obj);
	//for(auto i = 0 ; i < 1; i++)
	//	buildBuffers(path_objs_[i]);
	buildBuffers(path_objs_[0]);
	setCameraData();
}

void MetalRenderer::setScrSize(unsigned width, unsigned height){
	scr_width_ = width;
	scr_height_ = height;
}

void MetalRenderer::setFrameCount(unsigned count){
	frame_count_ = count;
}

void MetalRenderer::buildShaders()
{
	using NS::StringEncoding::UTF8StringEncoding;

	const char* shaderSrc = R"(
        #include <metal_stdlib>
        using namespace metal;

        struct v2f
        {
            float4 position [[position]];
            half3 color;
        };

        struct VertexData
        {
            float3 position;
        };

        struct InstanceData
        {
            float4x4 instanceTransform;
            float4 instanceColor;
        };

        struct CameraData
        {
            float4x4 perspectiveTransform;
						float4x4 viewTransform;
            float4x4 modelTransform;
        };

        v2f vertex vertexMain( device const VertexData* vertexData [[buffer(0)]],
                               device const InstanceData* instanceData [[buffer(1)]],
                               device const CameraData& cameraData [[buffer(2)]],
                               uint vertexId [[vertex_id]],
                               uint instanceId [[instance_id]] )
        {
            v2f o;
            float4 pos = float4( vertexData[ vertexId ].position, 1.0 );
						pos = instanceData[ instanceId ].instanceTransform * pos;
            pos = cameraData.perspectiveTransform * cameraData.viewTransform *cameraData.modelTransform * pos;
						o.position = pos;
            o.color = half3( instanceData[ instanceId ].instanceColor.rgb );
            return o;
        }

        half4 fragment fragmentMain( v2f in [[stage_in]] )
        {
            return half4( in.color, 1.0 );
        }
    )";

	NS::Error* pError = nullptr;
	MTL::Library* pLibrary = _pDevice->newLibrary( NS::String::string(shaderSrc, UTF8StringEncoding), nullptr, &pError );
	if ( !pLibrary )
	{
		__builtin_printf( "%s", pError->localizedDescription()->utf8String() );
		assert( false );
	}

	MTL::Function* pVertexFn = pLibrary->newFunction( NS::String::string("vertexMain", UTF8StringEncoding) );
	MTL::Function* pFragFn = pLibrary->newFunction( NS::String::string("fragmentMain", UTF8StringEncoding) );

	MTL::RenderPipelineDescriptor* pDesc = MTL::RenderPipelineDescriptor::alloc()->init();
	pDesc->setVertexFunction( pVertexFn );
	pDesc->setFragmentFunction( pFragFn );
	pDesc->colorAttachments()->object(0)->setPixelFormat( MTL::PixelFormat::PixelFormatBGRA8Unorm_sRGB );
	pDesc->setDepthAttachmentPixelFormat( MTL::PixelFormat::PixelFormatDepth16Unorm );

	_pPSO = _pDevice->newRenderPipelineState( pDesc, &pError );
	if ( !_pPSO )
	{
		__builtin_printf( "%s", pError->localizedDescription()->utf8String() );
		assert( false );
	}

	pVertexFn->release();
	pFragFn->release();
	pDesc->release();
	_pShaderLibrary = pLibrary;
}

void MetalRenderer::buildDepthStencilStates()
{
	MTL::DepthStencilDescriptor* pDsDesc = MTL::DepthStencilDescriptor::alloc()->init();
	pDsDesc->setDepthCompareFunction( MTL::CompareFunction::CompareFunctionLess );
	pDsDesc->setDepthWriteEnabled( true );

	_pDepthStencilState = _pDevice->newDepthStencilState( pDsDesc );

	pDsDesc->release();
}

void MetalRenderer::buildBuffers(const R2grap::RePathObj &obj) {
	auto path_data = obj.path;
	if(!path_data->has_keyframe){
		auto vert_array = path_data->verts;
		simd::float3 *vert_ptr = new simd::float3[vert_array.size() / 3];
		for(auto i = 0; i < vert_array.size() / 3 ; i++){
			vert_ptr[i] = {vert_array[3 *i], vert_array[3 *i + 1]  /*- (float)0.3*/, vert_array[3 *i + 2]};
		}
		const size_t vertexDataSize = sizeof(simd::float3) * vert_array.size() / 3;
		_pVertexDataBuffer = _pDevice->newBuffer( vertexDataSize, MTL::ResourceStorageModeManaged );
		memcpy( _pVertexDataBuffer->contents(), vert_ptr, vertexDataSize );
		_pVertexDataBuffer->didModifyRange( NS::Range::Make( 0, _pVertexDataBuffer->length() ) );
		delete []vert_ptr;

		if(path_data->closed){
			auto ind_array = path_data->tri_ind;
			const size_t indexDataSize = sizeof(uint16_t) * ind_array.size();
			uint16_t* ind_ptr = new uint16_t[ind_array.size()];
			for(auto i = 0; i < ind_array.size(); i++){
				ind_ptr[i] = static_cast<uint16_t>(ind_array[i]);
			}
			_pIndexBuffer = _pDevice->newBuffer( indexDataSize, MTL::ResourceStorageModeManaged );
			memcpy( _pIndexBuffer->contents(), ind_ptr, indexDataSize );
			_pIndexBuffer->didModifyRange( NS::Range::Make( 0, _pIndexBuffer->length()));
			delete []ind_ptr;
		}
	}

	/*else{
		const size_t vertexDataSize = sizeof(float) * path_data->GetMaxVectorSize(R2grap::PathData::PathVecContentType::t_Vertices);
		MTL::Buffer* pVertexBuffer = _pDevice->newBuffer(vertexDataSize, MTL::ResourceStorageModeManaged);
		pVertDataBufferList_.push_back(pVertexBuffer);
		if(path_data->closed){
			const size_t indexDataSize = sizeof(unsigned int) * path_data->GetMaxVectorSize(R2grap::PathData::PathVecContentType::t_TriangleIndex);
			MTL::Buffer* pIndexBuffer = _pDevice->newBuffer(indexDataSize, MTL::ResourceStorageModeManaged);
			pIndexBufferList_[pVertDataBufferList_.size() - 1] = pIndexBuffer;
		}
	}*/

	const size_t instanceDataSize = sizeof( shader_types::InstanceData );
	_pInstanceDataBuffer = _pDevice->newBuffer( instanceDataSize, MTL::ResourceStorageModeManaged);
}

void MetalRenderer::setCameraData() {
	const size_t cameraDataSize = sizeof( shader_types::CameraData );
	pCameraDataBuffer_ = _pDevice->newBuffer( cameraDataSize, MTL::ResourceStorageModeManaged );
	shader_types::CameraData* pCameraData = reinterpret_cast< shader_types::CameraData *>( pCameraDataBuffer_->contents() );
	Camera camera(glm::vec3(0.0f, 0.0f, 1.0f));
	pCameraData->perspectiveTransform = glm::perspective( glm::radians(camera.Zoom) ,(float)scr_width_/(float)scr_height_, 0.1f, 100.0f ) ;
	//pCameraData->perspectiveTransform = MetalMath::makePerspective(45.f * M_PI / 180.f, 1.f, 0.03f, 500.0f);
	pCameraData->viewTransform = camera.GetViewMatrix();
	pCameraData->modelTransform = glm::mat4(1.0);
	pCameraDataBuffer_->didModifyRange( NS::Range::Make( 0, sizeof( shader_types::CameraData ) ) );
}

void MetalRenderer::drawPathObjs(MTK::View* pView){
	NS::AutoreleasePool* pPool = NS::AutoreleasePool::alloc()->init();

	MTL::CommandBuffer* pCmd = _pCommandQueue->commandBuffer();
	/*dispatch_semaphore_wait( _semaphore, DISPATCH_TIME_FOREVER );
	MetalRenderer* pRenderer = this;
	pCmd->addCompletedHandler( ^void( MTL::CommandBuffer* pCmd ){
			dispatch_semaphore_signal( pRenderer->_semaphore );
	});*/

	glm::mat4 old_trans;
	//for(auto ind = 0; ind < path_objs_.size(); ind++){
	for(auto ind = 0; ind < 1; ind++){
		/*MTL::Buffer* pVertexBuffer = pVertDataBufferList_[ind];
		MTL::Buffer* pIndexBuffer = pIndexBufferList_[ind];
		MTL::Buffer* pInstanceDataBuffer = pInstanceDataBufferList_[ind];*/
		MTL::Buffer* pVertexBuffer = _pVertexDataBuffer;
		MTL::Buffer* pIndexBuffer = _pIndexBuffer;
		MTL::Buffer* pInstanceDataBuffer = _pInstanceDataBuffer;
		shader_types::InstanceData* pInstanceData = reinterpret_cast< shader_types::InstanceData *>( pInstanceDataBuffer->contents() );
		
		auto obj = path_objs_[ind];
		//if(obj.in_pos > static_cast<float>(played_) || obj.out_pos < static_cast<float>(played_)) continue;
		
		//set InstanceData.transform
		if(!obj.keep_trans){
			pInstanceData->instanceTransform = obj.trans[played_];
		}/*
		else if(obj.keep_trans && !path_objs_[ind - 1].keep_trans){
			pInstanceData->instanceTransform = path_objs_[ind - 1].trans[played_];
			old_trans = pInstanceData[ind].instanceTransform;
		}else{
			pInstanceData->instanceTransform = old_trans;
		}*/

		//set InstacneData.color
		if(obj.fill){
			if(obj.fill->trans_color.empty())
				pInstanceData->instanceColor = obj.fill->color;
			//else
			//	pInstanceData->instanceColor = obj.fill->trans_color[played_];
		}

		/*if(obj.stroke){
			if(obj.stroke->trans_color.empty())
				pInstanceData->instanceColor = obj.stroke->color;
			else
				pInstanceData->instanceColor = obj.stroke->trans_color[played_];
		}*/
		pInstanceDataBuffer->didModifyRange( NS::Range::Make( 0, pInstanceDataBuffer->length() ) );

		//set vertices
		auto path = obj.path;
		/*
		if(path->has_keyframe){
			auto vert_vec = path->trans_verts[played_];
			memcpy(pVertexBuffer->contents(), &vert_vec[0], sizeof(float) * vert_vec.size());
			pVertexBuffer->didModifyRange(NS::Range::Make(0, pVertexBuffer->length()));
			if(path->closed){
				auto trig_vec = path->trans_tri_ind[played_];
				memcpy(pIndexBuffer->contents(), &trig_vec[0], sizeof(unsigned int) * trig_vec.size());
				pIndexBuffer->didModifyRange(NS::Range::Make(0, pIndexBuffer->length()));
			}
		}*/

		// Begin render pass:
		MTL::RenderPassDescriptor* pRpd = pView->currentRenderPassDescriptor();
		MTL::RenderCommandEncoder* pEnc = pCmd->renderCommandEncoder( pRpd );
		pEnc->setRenderPipelineState( _pPSO );
		pEnc->setDepthStencilState( _pDepthStencilState );

		pEnc->setVertexBuffer( _pVertexDataBuffer, /* offset */ 0, /* index */ 0 );
		pEnc->setVertexBuffer( pInstanceDataBuffer, /* offset */ 0, /* index */ 1 );
		pEnc->setVertexBuffer( pCameraDataBuffer_, /* offset */ 0, /* index */ 2 );

		pEnc->setCullMode( MTL::CullModeBack );
		pEnc->setFrontFacingWinding( MTL::Winding::WindingCounterClockwise );

		if(path->closed){
			//auto index_size = path->has_keyframe ? path->trans_tri_ind[played_].size() : path->tri_ind.size();
			unsigned index_size = path->tri_ind.size();
			pEnc->drawIndexedPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle,
																	index_size,
																	MTL::IndexType::IndexTypeUInt16,
																	_pIndexBuffer, 0);
		}
		/*
		else{
			auto vert_size = path->has_keyframe ? path->trans_verts[played_].size() : path->verts.size();
			pEnc->drawPrimitives(MTL::PrimitiveType::PrimitiveTypeLineStrip,
													 NS::UInteger(0),
													 NS::UInteger(vert_size));
		}*/
		pEnc->endEncoding();
	}
	played_ = played_ > frame_count_ ? 0 : ++played_;

	pCmd->presentDrawable( pView->currentDrawable() );
	pCmd->commit();
	pPool->release();
}
