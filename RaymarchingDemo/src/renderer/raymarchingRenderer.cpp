//------------------------------------------------------------------------------------------
//! @file	raymarchingRenderer.cpp
//! @brief	レイマーチングレンダラークラス実装
//! @author	
//! @date	
//------------------------------------------------------------------------------------------

#include "src/system/sysMain.h"
#include "src/renderer/raymarchingRenderer.h"

#include <vector>
#include <string>
#include <fstream>

namespace {

};

//------------------------------------------------------------
//! @brief	コンストラクタ
//! @param	
//! @return	
//------------------------------------------------------------
raymarchingRenderer::raymarchingRenderer( void )
: m_window( nullptr )
, m_shaderHandle( 0 )
, m_vertHandle( 0 )
, m_fragHandle( 0 )
, m_uniformResolution( 0 )
, m_uniformCameraPosition( 0 )
, m_uniformCameraTarget( 0 )
, m_uniformCameraZoom( 0 )
, m_uniformRaymarchingType( 0 )
, m_attribPosition( 0 )
, m_vboHandle( 0 )
, m_vaoHandle( 0 )
{
}

//------------------------------------------------------------
//! @brief	デストラクタ
//! @param	
//! @return	
//------------------------------------------------------------
raymarchingRenderer::~raymarchingRenderer( void )
{
}

//------------------------------------------------------------
//! @brief	初期化
//! @param	window	ウインドウポインタ
//! @return	成否
//------------------------------------------------------------
bool raymarchingRenderer::initialize( GLFWwindow* window )
{
	m_window = window;
	
	loadShader();
	createVertex();
	
	return true;
}

//------------------------------------------------------------
//! @brief	後片付け
//! @param	
//! @return	
//------------------------------------------------------------
void raymarchingRenderer::terminate( void )
{
	deleteVertex();
	deleteShader();
}

//------------------------------------------------------------
//! @brief	描画
//! @param	
//! @return	
//------------------------------------------------------------
void raymarchingRenderer::render( void )
{
	glUseProgram( m_shaderHandle );
	
	int display_w, display_h;
	glfwGetFramebufferSize( m_window, &display_w, &display_h );
	
	const float resolution[] = {
		static_cast<float>( display_w ),
		static_cast<float>( display_h )
	};
	glUniform2fv( m_uniformResolution, 1, resolution );
	
	float cameraPosition[3]	= { 0.0f, 0.0f, 4.0f };
	float cameraTarget[3]	= { 0.0f, 0.0f, 0.0f };
	float cameraZoom		= sysMain::getInstance()->getCamera()->getZoom();
	vecParam cameraRotate;
	sysMain::getInstance()->getCamera()->getRotate( cameraRotate );
	
	// カメラ回転
	float distance	= std::sqrt( ( cameraPosition[0] - cameraTarget[0] ) * ( cameraPosition[0] - cameraTarget[0] ) +
								 ( cameraPosition[1] - cameraTarget[1] ) * ( cameraPosition[1] - cameraTarget[1] ) +
								 ( cameraPosition[2] - cameraTarget[2] ) * ( cameraPosition[2] - cameraTarget[2] ) );
	cameraPosition[0]	= cameraTarget[0] + distance * ( sin( cameraRotate.x ) * cos ( cameraRotate.y ) );
	cameraPosition[1]	= cameraTarget[1] + distance * ( sin( cameraRotate.y ) );
	cameraPosition[2]	= cameraTarget[2] + distance * ( cos( cameraRotate.x ) * cos ( cameraRotate.y ) );
	
	glUniform3fv( m_uniformCameraPosition, 1, cameraPosition );
	glUniform3fv( m_uniformCameraTarget, 1, cameraTarget );
	glUniform1f(  m_uniformCameraZoom, cameraZoom );
	glUniform1i(  m_uniformRaymarchingType, m_type );
	
	glEnableVertexAttribArray( m_attribPosition );
	
	glBindBuffer( GL_ARRAY_BUFFER, m_vboHandle );
	
	glVertexAttribPointer( m_attribPosition, 3, GL_FLOAT, GL_FALSE,	0, (void*)0 );
	
	glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
	
	glDisableVertexAttribArray( m_attribPosition );
}

//------------------------------------------------------------
//! @brief	GUI描画
//! @param	
//! @return	
//------------------------------------------------------------
void raymarchingRenderer::renderGui( void )
{
	static const char* names[Raymarching::Num] = {
		"Sphere",
		"Mandelbulb",
		"MengerSponge",
		"SierpinskiGasket",
	};
	
	if( ImGui::TreeNodeEx( "Raymarching", ImGuiTreeNodeFlags_DefaultOpen ) ) {
		for( int i = 0; i < Raymarching::Num; ++i ) {
			ImGui::RadioButton( names[i], &m_type, i );
		}
		ImGui::TreePop();
	}
}

//------------------------------------------------------------
//! @brief	シェーダー読み込み
//! @param	
//! @return	
//------------------------------------------------------------
void raymarchingRenderer::loadShader( void )
{
	m_shaderHandle	= glCreateProgram();
	m_vertHandle	= glCreateShader( GL_VERTEX_SHADER );
	m_fragHandle	= glCreateShader( GL_FRAGMENT_SHADER );
	
	// 頂点シェーダファイル読み込み
	const char* vertexShaderPath = "data\\shader\\basic.vert";
	std::string vertexShaderCode;
	loadShaderFile( vertexShaderPath, vertexShaderCode );
	char const * vertexSourcePointer = vertexShaderCode.c_str();
	
	// フラグメントシェーダファイル読み込み
	const char* fragmentShaderPath = "data\\shader\\basic.frag";
	std::string fragmentShaderCode;
	loadShaderFile( fragmentShaderPath, fragmentShaderCode );
	char const * fragmentSourcePointer = fragmentShaderCode.c_str();
	
	// コンパイル
	glShaderSource( m_vertHandle, 1, &vertexSourcePointer, 0 );
	glShaderSource( m_fragHandle, 1, &fragmentSourcePointer, 0 );
	glCompileShader( m_vertHandle );
	glCompileShader( m_fragHandle );
	
	GLint result = GL_FALSE;
	int infoLogLength;
	
	// 頂点シェーダをチェック
	glGetShaderiv( m_vertHandle, GL_COMPILE_STATUS, &result );
	glGetShaderiv( m_vertHandle, GL_INFO_LOG_LENGTH, &infoLogLength );
	if( 0 < infoLogLength ) {
		std::vector<char> vertexShaderErrorMessage( infoLogLength );
		glGetShaderInfoLog( m_vertHandle, infoLogLength, NULL, &vertexShaderErrorMessage[0] );
		fprintf( stdout, "%s\n", &vertexShaderErrorMessage[0] );
	}
	
	// フラグメントシェーダをチェック
	glGetShaderiv( m_fragHandle, GL_COMPILE_STATUS, &result );
	glGetShaderiv( m_fragHandle, GL_INFO_LOG_LENGTH, &infoLogLength );
	if( 0 < infoLogLength ) {
		std::vector<char> fragmentShaderErrorMessage( infoLogLength );
		glGetShaderInfoLog( m_fragHandle, infoLogLength, NULL, &fragmentShaderErrorMessage[0] );
		fprintf( stdout, "%s\n", &fragmentShaderErrorMessage[0] );
	}
	
	// リンク
	glAttachShader( m_shaderHandle, m_vertHandle );
	glAttachShader( m_shaderHandle, m_fragHandle );
	glLinkProgram( m_shaderHandle );
	
	// 最終チェック
	glGetProgramiv( m_shaderHandle, GL_LINK_STATUS, &result );
	glGetProgramiv( m_shaderHandle, GL_INFO_LOG_LENGTH, &infoLogLength );
	if( 0 < infoLogLength ) {
		std::vector<char> shaderErrorMessage( infoLogLength );
		glGetProgramInfoLog( m_shaderHandle, infoLogLength, NULL, &shaderErrorMessage[0] );
		fprintf( stdout, "%s\n", &shaderErrorMessage[0]);
	}
	
	m_uniformResolution			= glGetUniformLocation( m_shaderHandle, "Resolution" );
	m_uniformCameraPosition		= glGetUniformLocation( m_shaderHandle, "CameraPosition" );
	m_uniformCameraTarget		= glGetUniformLocation( m_shaderHandle, "CameraTarget" );
	m_uniformCameraZoom			= glGetUniformLocation( m_shaderHandle, "CameraZoom" );
	m_uniformRaymarchingType	= glGetUniformLocation( m_shaderHandle, "RaymarchingType" );
	m_attribPosition			= glGetAttribLocation( m_shaderHandle, "Position" );
}

//------------------------------------------------------------
//! @brief	シェーダー読み込み
//! @param	
//! @return	
//------------------------------------------------------------
void raymarchingRenderer::loadShaderFile( const char* path, std::string& outShaderCode )
{
	std::ifstream shaderStream( path, std::ios::in );
	if( shaderStream.is_open() ) {
		std::string Line = "";
		while( getline( shaderStream, Line ) ) {
			outShaderCode += "\n" + Line;
		}
		shaderStream.close();
	}
}

//------------------------------------------------------------
//! @brief	シェーダー破棄
//! @param	
//! @return	
//------------------------------------------------------------
void raymarchingRenderer::deleteShader( void )
{
	glDetachShader( m_shaderHandle, m_vertHandle );
	glDeleteShader( m_vertHandle );
	m_vertHandle = 0;
	
	glDetachShader( m_shaderHandle, m_fragHandle );
	glDeleteShader( m_fragHandle );
	m_fragHandle = 0;
	
	glDeleteProgram( m_shaderHandle );
	m_shaderHandle = 0;
}

//------------------------------------------------------------
//! @brief	頂点バッファ作成
//! @param	
//! @return	
//------------------------------------------------------------
void raymarchingRenderer::createVertex( void )
{
	glGenVertexArrays( 1, &m_vaoHandle );
	glBindVertexArray( m_vaoHandle );
	
	glGenBuffers( 1, &m_vboHandle );
	glBindBuffer( GL_ARRAY_BUFFER, m_vboHandle );
	
	const float vertexBufferData[] = {
		-1.0f, -1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f, 0.0f
	};
	
	glBufferData( GL_ARRAY_BUFFER, sizeof(vertexBufferData), vertexBufferData, GL_STATIC_DRAW );
}

//------------------------------------------------------------
//! @brief	頂点バッファ削除
//! @param	
//! @return	
//------------------------------------------------------------
void raymarchingRenderer::deleteVertex( void )
{
	if( m_vaoHandle )	glDeleteVertexArrays( 1, &m_vaoHandle );
	if( m_vboHandle )	glDeleteBuffers( 1, &m_vboHandle );
	m_vaoHandle = m_vboHandle = 0;
}
