//------------------------------------------------------------------------------------------
//! @file	raymarchingRenderer.h
//! @brief	レイマーチングレンダラークラス定義
//! @author	
//! @date	
//------------------------------------------------------------------------------------------

#pragma once

struct GLFWwindow;

//------------------------------------------------------------
// レイマーチングレンダラークラス定義
//------------------------------------------------------------
class raymarchingRenderer
{
public:
	struct Raymarching {
		enum Type {
			  Sphere = 0
			, Mandelbulb
			, MengerSponge
			, SierpinskiGasket
			
			, Num
		};
	};
public:
	raymarchingRenderer( void );
	~raymarchingRenderer( void );
	
	bool initialize( GLFWwindow* window );
	void terminate( void );
	void render( void );
	void renderGui( void );
	
private:
	void loadShader( void );
	void loadShaderFile( const char* path, std::string& outShaderCode );
	void deleteShader( void );
	void createVertex( void );
	void deleteVertex( void );
	
private:
	GLFWwindow*		m_window;
	int				m_shaderHandle;
	int				m_vertHandle;
	int				m_fragHandle;
	int				m_uniformResolution;
	int				m_uniformCameraPosition;
	int				m_uniformCameraTarget;
	int				m_uniformCameraZoom;
	int				m_uniformRaymarchingType;
	int				m_attribPosition;
	unsigned int	m_vboHandle;
	unsigned int	m_vaoHandle;
	
	int				m_type;
};
