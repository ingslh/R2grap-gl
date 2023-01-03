//
// Created by ingslh on 2022/12/31.
//
#include "MetalAppDelegate.h"
#include "MetalMTKViewDelegate.h"

MetalAppDelegate::~MetalAppDelegate()
{
	_pMtkView->release();
	_pWindow->release();
	_pDevice->release();
	delete _pViewDelegate;
}

NS::Menu* MetalAppDelegate::createMenuBar()
{
	using NS::StringEncoding::UTF8StringEncoding;

	NS::Menu* pMainMenu = NS::Menu::alloc()->init();
	NS::MenuItem* pAppMenuItem = NS::MenuItem::alloc()->init();
	NS::Menu* pAppMenu = NS::Menu::alloc()->init( NS::String::string( "Appname", UTF8StringEncoding ) );

	NS::String* appName = NS::RunningApplication::currentApplication()->localizedName();
	NS::String* quitItemName = NS::String::string( "Quit ", UTF8StringEncoding )->stringByAppendingString( appName );
	SEL quitCb = NS::MenuItem::registerActionCallback( "appQuit", [](void*,SEL,const NS::Object* pSender){
			auto pApp = NS::Application::sharedApplication();
			pApp->terminate( pSender );
	} );

	NS::MenuItem* pAppQuitItem = pAppMenu->addItem( quitItemName, quitCb, NS::String::string( "q", UTF8StringEncoding ) );
	pAppQuitItem->setKeyEquivalentModifierMask( NS::EventModifierFlagCommand );
	pAppMenuItem->setSubmenu( pAppMenu );

	NS::MenuItem* pWindowMenuItem = NS::MenuItem::alloc()->init();
	NS::Menu* pWindowMenu = NS::Menu::alloc()->init( NS::String::string( "Window", UTF8StringEncoding ) );

	SEL closeWindowCb = NS::MenuItem::registerActionCallback( "windowClose", [](void*, SEL, const NS::Object*){
			auto pApp = NS::Application::sharedApplication();
			pApp->windows()->object< NS::Window >(0)->close();
	} );
	NS::MenuItem* pCloseWindowItem = pWindowMenu->addItem( NS::String::string( "Close Window", UTF8StringEncoding ), closeWindowCb, NS::String::string( "w", UTF8StringEncoding ) );
	pCloseWindowItem->setKeyEquivalentModifierMask( NS::EventModifierFlagCommand );

	pWindowMenuItem->setSubmenu( pWindowMenu );

	pMainMenu->addItem( pAppMenuItem );
	pMainMenu->addItem( pWindowMenuItem );

	pAppMenuItem->release();
	pWindowMenuItem->release();
	pAppMenu->release();
	pWindowMenu->release();

	return pMainMenu->autorelease();
}

void MetalAppDelegate::applicationWillFinishLaunching( NS::Notification* pNotification )
{
	NS::Menu* pMenu = createMenuBar();
	NS::Application* pApp = reinterpret_cast< NS::Application* >( pNotification->object() );
	pApp->setMainMenu( pMenu );
	pApp->setActivationPolicy( NS::ActivationPolicy::ActivationPolicyRegular );
}

void MetalAppDelegate::applicationDidFinishLaunching( NS::Notification* pNotification )
{
	CGRect frame = (CGRect){ {100.0, 100.0}, {512.0, 512.0} };
	if(window_width_ != 0 && window_height_ != 0){
		frame.size.width = window_width_;
		frame.size.height = window_height_;
	}

	_pWindow = NS::Window::alloc()->init(
		frame,
		NS::WindowStyleMaskClosable|NS::WindowStyleMaskTitled,
		NS::BackingStoreBuffered,
		false );

	_pDevice = MTL::CreateSystemDefaultDevice();

	_pMtkView = MTK::View::alloc()->init( frame, _pDevice );
	_pMtkView->setColorPixelFormat( MTL::PixelFormat::PixelFormatBGRA8Unorm_sRGB );
	_pMtkView->setClearColor( MTL::ClearColor::Make( 0.0, 0.0, 0.0, 1.0 ) );
	_pMtkView->setDepthStencilPixelFormat( MTL::PixelFormat::PixelFormatDepth16Unorm );
	_pMtkView->setClearDepth( 1.0f );

	_pViewDelegate = new MetalMTKViewDelegate( _pDevice );
	if(!path_objs_.empty())
		_pViewDelegate->setRePathObjs(path_objs_);
	if(window_width_ != 0 && window_height_ != 0)
		_pViewDelegate->setScrSize(window_width_, window_height_);
	if(frame_count_ != 0)
		_pViewDelegate->setFrameCount(frame_count_);
	_pMtkView->setDelegate( _pViewDelegate );

	_pWindow->setContentView( _pMtkView );
	_pWindow->setTitle( NS::String::string( "R2grapMetal", NS::StringEncoding::UTF8StringEncoding ) );

	_pWindow->makeKeyAndOrderFront( nullptr );

	NS::Application* pApp = reinterpret_cast< NS::Application* >( pNotification->object() );
	pApp->activateIgnoringOtherApps( true );
}

bool MetalAppDelegate::applicationShouldTerminateAfterLastWindowClosed( NS::Application* pSender )
{
	return true;
}

void MetalAppDelegate::setWindowSize(unsigned int width, unsigned int height) {
	window_width_ = width;
	window_height_ = height;
}

void MetalAppDelegate::setPathObjs(const std::vector<R2grap::RePathObj> &objs) {
	path_objs_ = objs;
}

void MetalAppDelegate::setFrameCount(unsigned count){
	frame_count_ = count;
}
