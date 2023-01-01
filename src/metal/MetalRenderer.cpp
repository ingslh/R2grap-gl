//
// Created by ingslh on 2022/12/31.
//
#include "MetalRenderer.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

const int MetalRenderer::kMaxFramesInFlight = 3;
static constexpr size_t kNumInstances = 32;

MetalRenderer::MetalRenderer( MTL::Device* pDevice )
	: _pDevice( pDevice->retain() )
	, _angle ( 0.f )
	, _frame( 0 )
{
	_pCommandQueue = _pDevice->newCommandQueue();
	buildShaders();
	buildDepthStencilStates();
	buildBuffers();

	_semaphore = dispatch_semaphore_create( MetalRenderer::kMaxFramesInFlight );
}

MetalRenderer::~MetalRenderer()
{
	_pShaderLibrary->release();
	_pDepthStencilState->release();
	_pVertexDataBuffer->release();
	for ( int i = 0; i < kMaxFramesInFlight; ++i )
	{
		_pInstanceDataBuffer[i]->release();
	}
	for ( int i = 0; i < kMaxFramesInFlight; ++i )
	{
		_pCameraDataBuffer[i]->release();
	}
	_pIndexBuffer->release();
	_pPSO->release();
	_pCommandQueue->release();
	_pDevice->release();
}

void MetalRenderer::setRePathObjs(const std::vector<R2grap::RePathObj> objs) {
	path_objs_ = objs;
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
            float4x4 worldTransform;
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
            pos = cameraData.perspectiveTransform * cameraData.worldTransform * pos;
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

void MetalRenderer::buildBuffers()
{
	const float s = 0.5f;

	float verts[] = {
		 -s, -s, +s ,
		 +s, -s, +s ,
		 +s, +s, +s ,
		 -s, +s, +s ,

		 -s, -s, -s ,
		 -s, +s, -s ,
		 +s, +s, -s ,
		 +s, -s, -s
	};

	uint16_t indices[] = {
		0, 1, 2, /* front */
		2, 3, 0,

		1, 7, 6, /* right */
		6, 2, 1,

		7, 4, 5, /* back */
		5, 6, 7,

		4, 0, 3, /* left */
		3, 5, 4,

		3, 2, 6, /* top */
		6, 5, 3,

		4, 7, 1, /* bottom */
		1, 0, 4
	};

	const size_t vertexDataSize = sizeof( verts );
	const size_t indexDataSize = sizeof( indices );

	MTL::Buffer* pVertexBuffer = _pDevice->newBuffer( vertexDataSize, MTL::ResourceStorageModeManaged );
	MTL::Buffer* pIndexBuffer = _pDevice->newBuffer( indexDataSize, MTL::ResourceStorageModeManaged );

	_pVertexDataBuffer = pVertexBuffer;
	_pIndexBuffer = pIndexBuffer;

	memcpy( _pVertexDataBuffer->contents(), verts, vertexDataSize );
	memcpy( _pIndexBuffer->contents(), indices, indexDataSize );

	_pVertexDataBuffer->didModifyRange( NS::Range::Make( 0, _pVertexDataBuffer->length() ) );
	_pIndexBuffer->didModifyRange( NS::Range::Make( 0, _pIndexBuffer->length() ) );

	const size_t instanceDataSize = kMaxFramesInFlight * kNumInstances * sizeof( shader_types::InstanceData );
	for ( size_t i = 0; i < kMaxFramesInFlight; ++i )
	{
		_pInstanceDataBuffer[ i ] = _pDevice->newBuffer( instanceDataSize, MTL::ResourceStorageModeManaged );
	}

	const size_t cameraDataSize = kMaxFramesInFlight * sizeof( shader_types::CameraData );
	for ( size_t i = 0; i < kMaxFramesInFlight; ++i )
	{
		_pCameraDataBuffer[ i ] = _pDevice->newBuffer( cameraDataSize, MTL::ResourceStorageModeManaged );
	}
}

void MetalRenderer::buildBuffers(const R2grap::RePathObj &obj) {
	auto path_data = obj.path;
	if(!path_data->has_keyframe){
		auto vert_array = path_data->verts;
		const size_t vertexDataSize = sizeof(float) * vert_array.size();
		MTL::Buffer* pVertexBuffer = _pDevice->newBuffer( vertexDataSize, MTL::ResourceStorageModeManaged );
		pVertDataBufferList_.push_back(pVertexBuffer);
		memcpy(pVertDataBufferList_.back()->contents(), &vert_array[0], vertexDataSize);
		pVertDataBufferList_.back()->didModifyRange(NS::Range::Make(0, pVertexBuffer->length()));

		if(path_data->closed){
			auto ind_array = path_data->tri_ind;
			const size_t indexDataSize = sizeof(unsigned int) * ind_array.size();
			MTL::Buffer* pIndexBuffer = _pDevice->newBuffer( indexDataSize, MTL::ResourceStorageModeManaged );
			pIndexBufferList_.push_back(pIndexBuffer);
			memcpy(pIndexBufferList_.back()->contents(), &ind_array[0], indexDataSize);
			pVertDataBufferList_.back()->didModifyRange(NS::Range::Make(0,pIndexBuffer->length()));
		}
	}
}

void MetalRenderer::draw( MTK::View* pView )
{
	NS::AutoreleasePool* pPool = NS::AutoreleasePool::alloc()->init();

	_frame = (_frame + 1) % MetalRenderer::kMaxFramesInFlight;
	MTL::Buffer* pInstanceDataBuffer = _pInstanceDataBuffer[ _frame ];

	MTL::CommandBuffer* pCmd = _pCommandQueue->commandBuffer();
	dispatch_semaphore_wait( _semaphore, DISPATCH_TIME_FOREVER );
	MetalRenderer* pRenderer = this;
	pCmd->addCompletedHandler( ^void( MTL::CommandBuffer* pCmd ){
			dispatch_semaphore_signal( pRenderer->_semaphore );
	});

	_angle += 0.01f;

	const float scl = 0.1f;
	shader_types::InstanceData* pInstanceData = reinterpret_cast< shader_types::InstanceData *>( pInstanceDataBuffer->contents() );

	glm::vec3 objectPosition = { 0.f, 0.f, -5.f };

	// Update instance positions:
	auto rt = glm::translate(glm::mat4(1), objectPosition );
	auto rr = glm::rotate(glm::mat4(1), -_angle, glm::vec3(0,1,0));
	auto rtInv = glm::translate(glm::mat4(1), -glm::vec3(objectPosition.x, objectPosition.y, objectPosition.z) );
	auto fullObjectRot = rt * rr * rtInv;

	for ( size_t i = 0; i < kNumInstances; ++i )
	{
		float iDivNumInstances = i / (float)kNumInstances;
		float xoff = (iDivNumInstances * 2.0f - 1.0f) + (1.f/kNumInstances);
		float yoff = sin( ( iDivNumInstances + _angle ) * 2.0f * M_PI);

		// Use the tiny math library to apply a 3D transformation to the instance.
		auto scale = glm::scale(glm::mat4(1), glm::vec3(scl));
		auto zrot = glm::rotate(glm::mat4(1), _angle, glm::vec3(0,0,1));
		auto yrot = glm::rotate(glm::mat4(1), _angle,glm::vec3(0,1,0) );
		auto translate = glm::translate( glm::mat4(1) ,objectPosition + glm::vec3(xoff, yoff, 0.f)  );

		pInstanceData[ i ].instanceTransform = fullObjectRot * translate * yrot * zrot * scale;

		float r = iDivNumInstances;
		float g = 1.0f - r;
		float b = sinf( M_PI * 2.0f * iDivNumInstances );
		pInstanceData[ i ].instanceColor = glm::vec4(r, g, b, 1.0f) ;
	}
	pInstanceDataBuffer->didModifyRange( NS::Range::Make( 0, pInstanceDataBuffer->length() ) );

	// Update camera state:

	MTL::Buffer* pCameraDataBuffer = _pCameraDataBuffer[ _frame ];
	shader_types::CameraData* pCameraData = reinterpret_cast< shader_types::CameraData *>( pCameraDataBuffer->contents() );
	pCameraData->perspectiveTransform = glm::perspective( glm::radians(45.f) , 1.f, 0.03f, 500.0f ) ;
	pCameraData->worldTransform = glm::mat4(1);
	pCameraDataBuffer->didModifyRange( NS::Range::Make( 0, sizeof( shader_types::CameraData ) ) );

	// Begin render pass:

	MTL::RenderPassDescriptor* pRpd = pView->currentRenderPassDescriptor();
	MTL::RenderCommandEncoder* pEnc = pCmd->renderCommandEncoder( pRpd );

	pEnc->setRenderPipelineState( _pPSO );
	pEnc->setDepthStencilState( _pDepthStencilState );

	pEnc->setVertexBuffer( _pVertexDataBuffer, /* offset */ 0, /* index */ 0 );
	pEnc->setVertexBuffer( pInstanceDataBuffer, /* offset */ 0, /* index */ 1 );
	pEnc->setVertexBuffer( pCameraDataBuffer, /* offset */ 0, /* index */ 2 );

	pEnc->setCullMode( MTL::CullModeBack );
	pEnc->setFrontFacingWinding( MTL::Winding::WindingCounterClockwise );

	pEnc->drawIndexedPrimitives( MTL::PrimitiveType::PrimitiveTypeTriangleStrip,
															 6 * 6, MTL::IndexType::IndexTypeUInt16,
															 _pIndexBuffer,
															 0,
															 kNumInstances );

	pEnc->endEncoding();
	pCmd->presentDrawable( pView->currentDrawable() );
	pCmd->commit();

	pPool->release();
}
