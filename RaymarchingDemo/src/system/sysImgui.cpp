//------------------------------------------------------------------------------------------
//! @file	sysImgui.cpp
//! @brief	imgui組み込みシステムクラス実装
//! @author	
//! @date	
//------------------------------------------------------------------------------------------

#include <imgui.h>
#include "src/system/sysImgui.h"

namespace {
	//------------------------------------------------------------
	//! @brief	描画処理
	//! @param	draw_data	描画データ
	//! @return	
	//------------------------------------------------------------
	void renderDrawLists( ImDrawData* draw_data )
	{
		sysImgui* instance = sysImgui::getInstance();
		ASSERT( instance != nullptr, "nullpo" );
		if( instance == nullptr ) return;
		
		ImGuiIO& io			= ImGui::GetIO();
		const int fb_width	= static_cast<int>( io.DisplaySize.x * io.DisplayFramebufferScale.x );
		const int fb_height	= static_cast<int>( io.DisplaySize.y * io.DisplayFramebufferScale.y );
		if( fb_width == 0 || fb_height == 0 ) {
			return;
		}
		draw_data->ScaleClipRects( io.DisplayFramebufferScale );

		// 設定をバックアップ
		GLint last_program;					glGetIntegerv( GL_CURRENT_PROGRAM,				&last_program );
		GLint last_texture;					glGetIntegerv( GL_TEXTURE_BINDING_2D,			&last_texture );
		GLint last_active_texture;			glGetIntegerv( GL_ACTIVE_TEXTURE,				&last_active_texture );
		GLint last_array_buffer;			glGetIntegerv( GL_ARRAY_BUFFER_BINDING,			&last_array_buffer );
		GLint last_element_array_buffer;	glGetIntegerv( GL_ELEMENT_ARRAY_BUFFER_BINDING,	&last_element_array_buffer );
		GLint last_vertex_array;			glGetIntegerv( GL_VERTEX_ARRAY_BINDING,			&last_vertex_array );
		GLint last_blend_src;				glGetIntegerv( GL_BLEND_SRC,					&last_blend_src );
		GLint last_blend_dst;				glGetIntegerv( GL_BLEND_DST,					&last_blend_dst );
		GLint last_blend_equation_rgb;		glGetIntegerv( GL_BLEND_EQUATION_RGB,			&last_blend_equation_rgb );
		GLint last_blend_equation_alpha;	glGetIntegerv( GL_BLEND_EQUATION_ALPHA,			&last_blend_equation_alpha );
		GLint last_viewport[4];				glGetIntegerv( GL_VIEWPORT,						last_viewport );
		GLboolean last_enable_blend			= glIsEnabled( GL_BLEND );
		GLboolean last_enable_cull_face		= glIsEnabled( GL_CULL_FACE );
		GLboolean last_enable_depth_test	= glIsEnabled( GL_DEPTH_TEST );
		GLboolean last_enable_scissor_test	= glIsEnabled( GL_SCISSOR_TEST );

		// 描画設定
		glEnable( GL_BLEND );
		glBlendEquation( GL_FUNC_ADD );
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
		glDisable( GL_CULL_FACE );
		glDisable( GL_DEPTH_TEST );
		glEnable( GL_SCISSOR_TEST );
		glActiveTexture( GL_TEXTURE0 );

		// ビュー設定
		glViewport( 0, 0, static_cast<GLsizei>( fb_width ), static_cast<GLsizei>( fb_height ) );
		const float ortho_projection[4][4] =
		{
			{ 2.0f / io.DisplaySize.x,	0.0f,						 0.0f,	0.0f },
			{ 0.0f,						2.0f / -io.DisplaySize.y,	 0.0f,	0.0f },
			{ 0.0f,						0.0f,						-1.0f,	0.0f },
			{-1.0f,						1.0f,						 0.0f,	1.0f },
		};
		glUseProgram( instance->getShaderHandle() );
		glUniform1i( instance->getAttribLocationTex(), 0 );
		glUniformMatrix4fv( instance->getAttribLocationProjMtx(), 1, GL_FALSE, &ortho_projection[0][0] );
		glBindVertexArray( instance->getVaoHandle() );

		for( int n = 0; n < draw_data->CmdListsCount; n++ ) {
			const ImDrawList* cmd_list = draw_data->CmdLists[n];
			const ImDrawIdx* idx_buffer_offset = 0;
			
			glBindBuffer( GL_ARRAY_BUFFER, instance->getVboHandle() );
			glBufferData(	GL_ARRAY_BUFFER,
							static_cast<GLsizeiptr>( cmd_list->VtxBuffer.size() ) * sizeof(ImDrawVert),
							static_cast<const GLvoid*>( &cmd_list->VtxBuffer.front() ),
							GL_STREAM_DRAW );
			
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, instance->getElementsHandle() );
			glBufferData(	GL_ELEMENT_ARRAY_BUFFER,
							static_cast<GLsizeiptr>( cmd_list->IdxBuffer.size() ) * sizeof(ImDrawIdx),
							static_cast<const GLvoid*>( &cmd_list->IdxBuffer.front() ),
							GL_STREAM_DRAW );
			
			for( const ImDrawCmd* pcmd = cmd_list->CmdBuffer.begin(); pcmd != cmd_list->CmdBuffer.end(); ++pcmd ) {
				if( pcmd->UserCallback ) {
					pcmd->UserCallback( cmd_list, pcmd );
				}
				else {
					glBindTexture( GL_TEXTURE_2D, static_cast<GLuint>( reinterpret_cast<const intptr_t>( pcmd->TextureId ) ) );
					glScissor(	static_cast<int>( pcmd->ClipRect.x ),
								static_cast<int>( fb_height - pcmd->ClipRect.w ),
								static_cast<int>( pcmd->ClipRect.z - pcmd->ClipRect.x ),
								static_cast<int>( pcmd->ClipRect.w - pcmd->ClipRect.y ) );
					glDrawElements(	GL_TRIANGLES,
									static_cast<GLsizei>( pcmd->ElemCount ),
									sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT,
									idx_buffer_offset );
				}
				idx_buffer_offset += pcmd->ElemCount;
			}
		}
		
		// 設定を戻す
		glUseProgram( last_program );
		glActiveTexture( last_active_texture );
		glBindTexture( GL_TEXTURE_2D, last_texture );
		glBindVertexArray( last_vertex_array );
		glBindBuffer( GL_ARRAY_BUFFER, last_array_buffer );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, last_element_array_buffer );
		glBlendEquationSeparate( last_blend_equation_rgb, last_blend_equation_alpha );
		glBlendFunc( last_blend_src, last_blend_dst );
		if( last_enable_blend )			glEnable( GL_BLEND );			else glDisable( GL_BLEND );
		if( last_enable_cull_face )		glEnable( GL_CULL_FACE );		else glDisable( GL_CULL_FACE );
		if( last_enable_depth_test )	glEnable( GL_DEPTH_TEST );		else glDisable( GL_DEPTH_TEST );
		if( last_enable_scissor_test )	glEnable( GL_SCISSOR_TEST );	else glDisable( GL_SCISSOR_TEST );
		glViewport( last_viewport[0], last_viewport[1], static_cast<GLsizei>( last_viewport[2] ), static_cast<GLsizei>( last_viewport[3] ) );
	}

	//------------------------------------------------------------
	//! @brief	クリップボードテキスト取得
	//! @param	
	//! @return	テキスト
	//------------------------------------------------------------
	const char* getClipboardText( void )
	{
		sysImgui* instance = sysImgui::getInstance();
		ASSERT( instance != nullptr, "nullpo" );
		if( instance == nullptr ) return nullptr;
		
		return glfwGetClipboardString( instance->getWindow() );
	}

	//------------------------------------------------------------
	//! @brief	クリップボードテキスト設定
	//! @param	text	テキスト
	//! @return	
	//------------------------------------------------------------
	void setClipboardText( const char* text )
	{
		sysImgui* instance = sysImgui::getInstance();
		ASSERT( instance != nullptr, "nullpo" );
		if( instance == nullptr ) return;
		
		glfwSetClipboardString( instance->getWindow(), text );
	}
};

sysImgui* sysImgui::m_instance = nullptr;

//------------------------------------------------------------
//! @brief	インスタンス作成
//! @param	
//! @return	
//------------------------------------------------------------
void sysImgui::createInstance( void )
{
	ASSERT( m_instance == nullptr, "sysImgui instance not null" );
	if( m_instance == nullptr ) {
		m_instance = new sysImgui();
	}
}

//------------------------------------------------------------
//! @brief	インスタンス削除
//! @param	
//! @return	
//------------------------------------------------------------
void sysImgui::destroyInstance( void )
{
	ASSERT( m_instance != nullptr, "sysImgui instance null" );
	SAFE_DELETE( m_instance );
}

//------------------------------------------------------------
//! @brief	コンストラクタ
//! @param	
//! @return	
//------------------------------------------------------------
sysImgui::sysImgui( void )
: m_window( nullptr )
, m_time( 0.0f )
, m_mouseWheel( 0.0f )
, m_fontTexture( 0 )
, m_shaderHandle( 0 )
, m_vertHandle( 0 )
, m_fragHandle( 0 )
, m_attribLocationTex( 0 )
, m_attribLocationProjMtx( 0 )
, m_attribLocationPosition( 0 )
, m_attribLocationUV( 0 )
, m_attribLocationColor( 0 )
, m_vboHandle( 0 )
, m_vaoHandle( 0 )
, m_elementsHandle( 0 )
, m_isAppEnd( false )
, m_isShowGui( true )
, m_isShowTestWindow( false )
{
	for( int i = 0; i < MOUSE_BUTTON_NUM; ++i ) {
		m_mousePressed[i] = false;
	}
}

//------------------------------------------------------------
//! @brief	デストラクタ
//! @param	
//! @return	
//------------------------------------------------------------
sysImgui::~sysImgui( void )
{
}

//------------------------------------------------------------
//! @brief	初期化
//! @param	window				ウインドウポインタ
//! @return	成否
//------------------------------------------------------------
bool sysImgui::initialize( GLFWwindow* window )
{
	m_window = window;
	
	ImGuiIO& io						= ImGui::GetIO();
	io.KeyMap[ImGuiKey_Tab]			= GLFW_KEY_TAB;
	io.KeyMap[ImGuiKey_LeftArrow]	= GLFW_KEY_LEFT;
	io.KeyMap[ImGuiKey_RightArrow]	= GLFW_KEY_RIGHT;
	io.KeyMap[ImGuiKey_UpArrow]		= GLFW_KEY_UP;
	io.KeyMap[ImGuiKey_DownArrow]	= GLFW_KEY_DOWN;
	io.KeyMap[ImGuiKey_PageUp]		= GLFW_KEY_PAGE_UP;
	io.KeyMap[ImGuiKey_PageDown]	= GLFW_KEY_PAGE_DOWN;
	io.KeyMap[ImGuiKey_Home]		= GLFW_KEY_HOME;
	io.KeyMap[ImGuiKey_End]			= GLFW_KEY_END;
	io.KeyMap[ImGuiKey_Delete]		= GLFW_KEY_DELETE;
	io.KeyMap[ImGuiKey_Backspace]	= GLFW_KEY_BACKSPACE;
	io.KeyMap[ImGuiKey_Enter]		= GLFW_KEY_ENTER;
	io.KeyMap[ImGuiKey_Escape]		= GLFW_KEY_ESCAPE;
	io.KeyMap[ImGuiKey_A]			= GLFW_KEY_A;
	io.KeyMap[ImGuiKey_C]			= GLFW_KEY_C;
	io.KeyMap[ImGuiKey_V]			= GLFW_KEY_V;
	io.KeyMap[ImGuiKey_X]			= GLFW_KEY_X;
	io.KeyMap[ImGuiKey_Y]			= GLFW_KEY_Y;
	io.KeyMap[ImGuiKey_Z]			= GLFW_KEY_Z;
	
	io.RenderDrawListsFn	= renderDrawLists;
	io.SetClipboardTextFn	= setClipboardText;
	io.GetClipboardTextFn	= getClipboardText;
#ifdef _WIN32
	io.ImeWindowHandle		= glfwGetWin32Window( m_window );
#endif
	
	io.Fonts->AddFontFromFileTTF( "data\\font\\ARIALUNI.TTF", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
	
	// スタイルをカスタマイズ
	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowPadding		= ImVec2( 4.0f, 3.0f );
	style.WindowRounding	= 3.0f;
	style.FramePadding		= ImVec2( 0.0f, 2.0f );
	style.IndentSpacing		= 8.0f;
	io.FontGlobalScale		= 0.9f;
	
	return true;
}

//------------------------------------------------------------
//! @brief	後片付け
//! @param	
//! @return	
//------------------------------------------------------------
void sysImgui::terminate( void )
{
	invalidateDeviceObjects();
	ImGui::Shutdown();
}

//------------------------------------------------------------
//! @brief	フレーム開始
//! @param	
//! @return	
//------------------------------------------------------------
void sysImgui::newFrame( void )
{
	if( !m_fontTexture ) {
		createDeviceObjects();
	}
	
	ImGuiIO& io = ImGui::GetIO();
	
	// ディスプレイサイズ設定（動的なサイズ変更に対応するためここでやる）
	int w, h;
	int display_w, display_h;
	glfwGetWindowSize( m_window, &w, &h );
	glfwGetFramebufferSize( m_window, &display_w, &display_h );
	io.DisplaySize				= ImVec2( static_cast<float>( w ), static_cast<float>( h ) );
	io.DisplayFramebufferScale	= ImVec2(	w > 0 ? ( static_cast<float>( display_w ) / w ) : 0,
											h > 0 ? ( static_cast<float>( display_h ) / h ) : 0 );
	
	// 時間設定
	double current_time = glfwGetTime();
	io.DeltaTime = m_time > 0.0 ? static_cast<float>( ( current_time - m_time ) ) : static_cast<float>( 1.0f / 60.0f );
	m_time = current_time;
	
	// 入力設定
	if( glfwGetWindowAttrib( m_window, GLFW_FOCUSED ) ) {
		double mouse_x, mouse_y;
		glfwGetCursorPos( m_window, &mouse_x, &mouse_y );
		io.MousePos = ImVec2( static_cast<float>( mouse_x ), static_cast<float>( mouse_y ) );
	}
	else {
		io.MousePos = ImVec2( -1.0f, -1.0f );
	}
	
	for( int i = 0; i < MOUSE_BUTTON_NUM; ++i ) {
		io.MouseDown[i]		= m_mousePressed[i] || glfwGetMouseButton( m_window, i ) != 0;
		m_mousePressed[i]	= false;
	}
	
	io.MouseWheel	= m_mouseWheel;
	m_mouseWheel	= 0.0f;
	
	// ImGuiがマウスカーソルを表示していた場合、OSのマウスカーソルを非表示にする
	glfwSetInputMode( m_window, GLFW_CURSOR, io.MouseDrawCursor ? GLFW_CURSOR_HIDDEN : GLFW_CURSOR_NORMAL );
	
	// フレーム開始
	ImGui::NewFrame();
}

//------------------------------------------------------------
//! @brief	描画フレーム開始
//! @param	
//! @return	
//------------------------------------------------------------
void sysImgui::frameBegin( void )
{
	newFrame();
	
	// メニューバー
	if( ImGui::BeginMainMenuBar() ) {
		if( ImGui::BeginMenu( "File" ) ) {
			ImGui::MenuItem( "Quit", NULL, &m_isAppEnd );
			ImGui::EndMenu();
		}
		if( ImGui::BeginMenu( "View" ) ) {
			ImGui::MenuItem( "GUI", NULL, &m_isShowGui );
#if defined(DEBUG) || defined(_DEBUG)
			ImGui::MenuItem( "TestWindow", NULL, &m_isShowTestWindow );
#endif
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}

//------------------------------------------------------------
//! @brief	描画
//! @param	
//! @return	描画許可
//------------------------------------------------------------
bool sysImgui::renderBegin( void )
{
	if( m_isShowGui == false ) return false;
	
	if( m_isShowTestWindow ) {
		ImGui::ShowTestWindow();
	}
	
	// FPS表示
	ImGui::SetNextWindowPos( ImVec2( 5,30 ), ImGuiSetCond_Once );
	ImGui::SetNextWindowSize( ImVec2( 150, 300 ), ImGuiSetCond_Once );
	ImGui::Begin( "MainMenu", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_ShowBorders );
	ImGui::Text( " FPS : %.1f", ImGui::GetIO().Framerate );
	ImGui::Separator();
	
	return true;
}

//------------------------------------------------------------
//! @brief	描画終了
//! @param	
//! @return	
//------------------------------------------------------------
void sysImgui::renderEnd( void )
{
	ImGui::End();
}

//------------------------------------------------------------
//! @brief	描画フレーム終了
//! @param	
//! @return	
//------------------------------------------------------------
void sysImgui::frameEnd( void )
{
	ImGui::Render();
}

//------------------------------------------------------------
//! @brief	マウスボタンコールバック
//! @param	window	ウインドウ
//! @param	button	ボタンID
//! @param	action	アクションID
//! @param	mods	装飾キーID
//! @return	
//------------------------------------------------------------
void sysImgui::mouseButtonCallback( GLFWwindow* window, int button, int action, int mods )
{
	if( action == GLFW_PRESS && button >= 0 && button < MOUSE_BUTTON_NUM ) {
		setMousePressed( button, true );
	}
}

//------------------------------------------------------------
//! @brief	マウススクロールコールバック
//! @param	window	ウインドウ
//! @param	xoffset	Xオフセット
//! @param	yoffset	Yオフセット
//! @return	
//------------------------------------------------------------
void sysImgui::scrollCallback(GLFWwindow* window, double xoffset, double yoffset )
{
	addMouseWheel( static_cast<float>( yoffset ) );
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
void sysImgui::keyCallback( GLFWwindow* window, int key, int scancode, int action, int mods )
{
	ImGuiIO& io = ImGui::GetIO();
	if( action == GLFW_PRESS ) {
		io.KeysDown[key] = true;
	}
	if( action == GLFW_RELEASE ) {
		io.KeysDown[key] = false;
	}
	
	io.KeyCtrl	= io.KeysDown[GLFW_KEY_LEFT_CONTROL]	|| io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
	io.KeyShift	= io.KeysDown[GLFW_KEY_LEFT_SHIFT]		|| io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
	io.KeyAlt	= io.KeysDown[GLFW_KEY_LEFT_ALT]		|| io.KeysDown[GLFW_KEY_RIGHT_ALT];
	io.KeySuper	= io.KeysDown[GLFW_KEY_LEFT_SUPER]		|| io.KeysDown[GLFW_KEY_RIGHT_SUPER];
}

//------------------------------------------------------------
//! @brief	文字入力コールバック
//! @param	window	ウインドウ
//! @param	c		文字
//! @return	
//------------------------------------------------------------
void sysImgui::charCallback( GLFWwindow* window, unsigned int c )
{
	ImGuiIO& io = ImGui::GetIO();
	if( c > 0 && c < 0x10000 ) {
		io.AddInputCharacter( static_cast<unsigned short>( c ) );
	}
}

//------------------------------------------------------------
//! @brief	フォントテクスチャ作成
//! @param	
//! @return	成否
//------------------------------------------------------------
bool sysImgui::createFontsTexture( void )
{
	// テクスチャアトラス化
	ImGuiIO& io = ImGui::GetIO();
	unsigned char* pixels;
	int width, height;
	io.Fonts->GetTexDataAsRGBA32( &pixels, &width, &height );
	
	// グラフィックスシステムにテクスチャを登録
	GLint last_texture;
	glGetIntegerv( GL_TEXTURE_BINDING_2D, &last_texture );	// 現在のテクスチャをバックアップ
	glGenTextures( 1, &m_fontTexture );
	glBindTexture( GL_TEXTURE_2D, m_fontTexture );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels );
	
	io.Fonts->TexID = reinterpret_cast<void*>( static_cast<intptr_t>( m_fontTexture ) );
	
	// バックアップしたテクスチャに戻す
	glBindTexture( GL_TEXTURE_2D, last_texture );
	
	return true;
}

//------------------------------------------------------------
//! @brief	描画用オブジェクト作成
//! @param	
//! @return	成否
//------------------------------------------------------------
bool sysImgui::createDeviceObjects()
{
	// 設定をバックアップ
	GLint last_texture;			glGetIntegerv( GL_TEXTURE_BINDING_2D, &last_texture );
	GLint last_array_buffer;	glGetIntegerv( GL_ARRAY_BUFFER_BINDING, &last_array_buffer );
	GLint last_vertex_array;	glGetIntegerv( GL_VERTEX_ARRAY_BINDING, &last_vertex_array );
	
	const GLchar *vertex_shader =
		"#version 330\n"
		"uniform mat4 ProjMtx;\n"
		"in vec2 Position;\n"
		"in vec2 UV;\n"
		"in vec4 Color;\n"
		"out vec2 Frag_UV;\n"
		"out vec4 Frag_Color;\n"
		"void main()\n"
		"{\n"
		"	Frag_UV = UV;\n"
		"	Frag_Color = Color;\n"
		"	gl_Position = ProjMtx * vec4(Position.xy,0,1);\n"
		"}\n";
	
	const GLchar* fragment_shader =
		"#version 330\n"
		"uniform sampler2D Texture;\n"
		"in vec2 Frag_UV;\n"
		"in vec4 Frag_Color;\n"
		"out vec4 Out_Color;\n"
		"void main()\n"
		"{\n"
		"	Out_Color = Frag_Color * texture( Texture, Frag_UV.st);\n"
		"}\n";
	
	m_shaderHandle	= glCreateProgram();
	m_vertHandle	= glCreateShader( GL_VERTEX_SHADER );
	m_fragHandle	= glCreateShader( GL_FRAGMENT_SHADER );
	glShaderSource( m_vertHandle, 1, &vertex_shader, 0 );
	glShaderSource( m_fragHandle, 1, &fragment_shader, 0 );
	glCompileShader( m_vertHandle );
	glCompileShader( m_fragHandle );
	glAttachShader( m_shaderHandle, m_vertHandle );
	glAttachShader( m_shaderHandle, m_fragHandle );
	glLinkProgram( m_shaderHandle );
	
	m_attribLocationTex			= glGetUniformLocation( m_shaderHandle, "Texture" );
	m_attribLocationProjMtx		= glGetUniformLocation( m_shaderHandle, "ProjMtx" );
	m_attribLocationPosition	= glGetAttribLocation( m_shaderHandle, "Position" );
	m_attribLocationUV			= glGetAttribLocation( m_shaderHandle, "UV" );
	m_attribLocationColor		= glGetAttribLocation( m_shaderHandle, "Color" );
	
	glGenBuffers( 1, &m_vboHandle );
	glGenBuffers( 1, &m_elementsHandle );
	
	glGenVertexArrays( 1, &m_vaoHandle );
	glBindVertexArray( m_vaoHandle );
	glBindBuffer( GL_ARRAY_BUFFER, m_vboHandle );
	glEnableVertexAttribArray( m_attribLocationPosition );
	glEnableVertexAttribArray( m_attribLocationUV );
	glEnableVertexAttribArray( m_attribLocationColor );
	
#define OFFSETOF( TYPE, ELEMENT ) ( (size_t)&(((TYPE *)0)->ELEMENT) )
	glVertexAttribPointer( m_attribLocationPosition,	2, GL_FLOAT,			GL_FALSE,	sizeof(ImDrawVert), reinterpret_cast<GLvoid*>( OFFSETOF( ImDrawVert, pos ) ) );
	glVertexAttribPointer( m_attribLocationUV,			2, GL_FLOAT,			GL_FALSE,	sizeof(ImDrawVert), reinterpret_cast<GLvoid*>( OFFSETOF( ImDrawVert, uv ) ) );
	glVertexAttribPointer( m_attribLocationColor,		4, GL_UNSIGNED_BYTE,	GL_TRUE,	sizeof(ImDrawVert), reinterpret_cast<GLvoid*>( OFFSETOF( ImDrawVert, col ) ) );
#undef OFFSETOF
	
	createFontsTexture();
	
	// 設定を戻す
	glBindTexture( GL_TEXTURE_2D, last_texture );
	glBindBuffer( GL_ARRAY_BUFFER, last_array_buffer );
	glBindVertexArray( last_vertex_array );
	
	return true;
}

//------------------------------------------------------------
//! @brief	描画用オブジェクト破棄
//! @param	
//! @return	
//------------------------------------------------------------
void sysImgui::invalidateDeviceObjects( void )
{
	if( m_vaoHandle )		glDeleteVertexArrays( 1, &m_vaoHandle );
	if( m_vboHandle )		glDeleteBuffers( 1, &m_vboHandle );
	if( m_elementsHandle )	glDeleteBuffers( 1, &m_elementsHandle );
	m_vaoHandle = m_vboHandle = m_elementsHandle = 0;
	
	glDetachShader( m_shaderHandle, m_vertHandle );
	glDeleteShader( m_vertHandle );
	m_vertHandle = 0;
	
	glDetachShader( m_shaderHandle, m_fragHandle );
	glDeleteShader( m_fragHandle );
	m_fragHandle = 0;
	
	glDeleteProgram( m_shaderHandle );
	m_shaderHandle = 0;
	
	if( m_fontTexture ) {
		glDeleteTextures( 1, &m_fontTexture );
		ImGui::GetIO().Fonts->TexID = 0;
		m_fontTexture = 0;
	}
}
