//------------------------------------------------------------------------------------------
//! @file	sysMain.cpp
//! @brief	メインシステム実装
//! @author	
//! @date	
//------------------------------------------------------------------------------------------

#include "src/system/sysMain.h"

namespace {
	//------------------------------------------------------------
	//! @brief	エラーコールバック
	//! @param	error		エラーコード
	//! @param	description	エラーメッセージ
	//! @return	
	//------------------------------------------------------------
	static void errorCallback( int error, const char* description )
	{
		fprintf( stderr, "Error %d: %s\n", error, description );
	}
	
	//------------------------------------------------------------
	//! @brief	マウスボタンコールバック
	//! @param	window	ウインドウ
	//! @param	button	ボタンID
	//! @param	action	アクションID
	//! @param	mods	装飾キーID
	//! @return	
	//------------------------------------------------------------
	void mouseButtonCallback( GLFWwindow* window, int button, int action, int mods )
	{
		sysImgui* imgui = sysImgui::getInstance();
		ASSERT( imgui != nullptr, "nullpo" );
		if( imgui == nullptr ) return;
		
		imgui->mouseButtonCallback( window, button, action, mods );
		
		sysMain* instance = sysMain::getInstance();
		ASSERT( instance != nullptr, "nullpo" );
		if( instance == nullptr ) return;
		
		instance->getCamera()->mouseButtonCallback( window, button, action, mods );
	}
	
	//------------------------------------------------------------
	//! @brief	マウス移動コールバック
	//! @param	window	ウインドウ
	//! @param	xpos	X座標
	//! @param	ypos	Y座標
	//! @return	
	//------------------------------------------------------------
	void cursorPosCallback( GLFWwindow* window, double xpos, double ypos )
	{
		sysMain* instance = sysMain::getInstance();
		ASSERT( instance != nullptr, "nullpo" );
		if( instance == nullptr ) return;
		
		instance->getCamera()->cursorPosCallback( window, xpos, ypos );
	}
	
	//------------------------------------------------------------
	//! @brief	マウススクロールコールバック
	//! @param	window	ウインドウ
	//! @param	xoffset	Xオフセット
	//! @param	yoffset	Yオフセット
	//! @return	
	//------------------------------------------------------------
	void scrollCallback( GLFWwindow* window, double xoffset, double yoffset )
	{
		sysImgui* imgui = sysImgui::getInstance();
		ASSERT( imgui != nullptr, "nullpo" );
		if( imgui == nullptr ) return;
		
		imgui->scrollCallback( window, xoffset, yoffset );
		
		sysMain* instance = sysMain::getInstance();
		ASSERT( instance != nullptr, "nullpo" );
		if( instance == nullptr ) return;
		
		instance->getCamera()->scrollCallback( window, xoffset, yoffset );
	}
	
	//------------------------------------------------------------
	//! @brief	キーボードコールバック
	//! @param	window		ウインドウ
	//! @param	key			キーID
	//! @param	scancode	スキャンコード
	//! @param	action		アクションID
	//! @param	mods		装飾キーID
	//! @return	
	//------------------------------------------------------------
	void keyCallback( GLFWwindow* window, int key, int scancode, int action, int mods )
	{
		sysImgui* imgui = sysImgui::getInstance();
		ASSERT( imgui != nullptr, "nullpo" );
		if( imgui == nullptr ) return;
		
		imgui->keyCallback( window, key, scancode, action, mods );
	}
	
	//------------------------------------------------------------
	//! @brief	文字入力コールバック
	//! @param	window	ウインドウ
	//! @param	c		文字
	//! @return	
	//------------------------------------------------------------
	void charCallback( GLFWwindow* window, unsigned int c )
	{
		sysImgui* imgui = sysImgui::getInstance();
		ASSERT( imgui != nullptr, "nullpo" );
		if( imgui == nullptr ) return;
		
		imgui->charCallback( window, c );
	}
}

sysMain* sysMain::m_instance = nullptr;

//------------------------------------------------------------
//! @brief	インスタンス作成
//! @param	
//! @return	
//------------------------------------------------------------
void sysMain::createInstance( void )
{
	ASSERT( m_instance == nullptr, "sysMain instance not null" );
	if( m_instance == nullptr ) {
		m_instance = new sysMain();
	}
}

//------------------------------------------------------------
//! @brief	インスタンス削除
//! @param	
//! @return	
//------------------------------------------------------------
void sysMain::destroyInstance( void )
{
	ASSERT( m_instance != nullptr, "sysMain instance null" );
	SAFE_DELETE( m_instance );
}

//------------------------------------------------------------
//! @brief	コンストラクタ
//! @param	
//! @return	
//------------------------------------------------------------
sysMain::sysMain( void )
: m_window( nullptr )
{
}

//------------------------------------------------------------
//! @brief	デストラクタ
//! @param	
//! @return	
//------------------------------------------------------------
sysMain::~sysMain( void )
{
}

//------------------------------------------------------------
//! @brief	初期化
//! @param	
//! @return	成否
//------------------------------------------------------------
bool sysMain::initialize( const char* windowTitle, int windowWidth, int windowHeight )
{
	// GLFWとGLEWの初期化
	glfwSetErrorCallback( errorCallback );
	if( !glfwInit() ) {
		return false;
	}
	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
	glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
	m_window = glfwCreateWindow( windowWidth, windowHeight, windowTitle, nullptr, nullptr );
	glfwMakeContextCurrent( m_window );
	glewInit();
	
	// コールバック登録
	glfwSetMouseButtonCallback( m_window, mouseButtonCallback );
	glfwSetCursorPosCallback( m_window, cursorPosCallback  );
	glfwSetScrollCallback( m_window, scrollCallback );
	glfwSetKeyCallback( m_window, keyCallback );
	glfwSetCharCallback( m_window, charCallback );
	
	// Imgui初期化
	sysImgui::createInstance();
	sysImgui* imgui = sysImgui::getInstance();
	imgui->initialize( m_window );
	
	// カメラ初期化
	m_camera.reset();
	
	// レイマーチングレンダラー初期化
	m_raymarching.initialize( m_window );
	
	return true;
}

//------------------------------------------------------------
//! @brief	後片付け
//! @param	
//! @return	
//------------------------------------------------------------
void sysMain::terminate( void )
{
	// レイマーチングレンダラー後片付け
	m_raymarching.terminate();
	
	// Imgui後片付け
	sysImgui* imgui = sysImgui::getInstance();
	imgui->terminate();
	sysImgui::destroyInstance();
	
	glfwTerminate();
}

//------------------------------------------------------------
//! @brief	メインループ
//! @param	
//! @return	
//------------------------------------------------------------
void sysMain::mainLoop( void )
{
	// メインループ
	while( !glfwWindowShouldClose( m_window ) ) {
		update();
		render();
		
		sysImgui* imgui = sysImgui::getInstance();
		if( imgui != nullptr && imgui->isAppEnd() ) {
			break;
		}
	}
}

//------------------------------------------------------------
//! @brief	更新
//! @param	
//! @return	
//------------------------------------------------------------
void sysMain::update( void )
{
}

//------------------------------------------------------------
//! @brief	描画
//! @param	
//! @return	
//------------------------------------------------------------
void sysMain::render( void )
{
	glfwPollEvents();
	int display_w, display_h;
	glfwGetFramebufferSize( m_window, &display_w, &display_h );
	glViewport( 0, 0, display_w, display_h );
	glClearColor( 0.5, 0.5, 1.0, 1.0 );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	
	// レイマーチング描画
	m_raymarching.render();
	
	// Imgui描画
	{
		sysImgui* imgui = sysImgui::getInstance();
		imgui->frameBegin();
		if( imgui->renderBegin() ) {
			m_camera.renderGui();
			m_raymarching.renderGui();
			
			imgui->renderEnd();
		}
		imgui->frameEnd();
	}
	
	glfwSwapBuffers( m_window );
}
