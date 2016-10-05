//------------------------------------------------------------------------------------------
//! @file	sysImgui.h
//! @brief	imgui組み込みシステムクラス定義
//! @author	
//! @date	
//------------------------------------------------------------------------------------------

#pragma once

#include <imgui.h>

struct GLFWwindow;

//------------------------------------------------------------
// imgui組み込みシステムクラス定義
//------------------------------------------------------------
class sysImgui
{
public:
	enum {
		MOUSE_BUTTON_NUM = 3	// サポートするマウスボタン数
	};
	
public:
	static void createInstance( void );
	static void destroyInstance( void );
	
	static sysImgui*	getInstance( void ) { return m_instance; }
	
	sysImgui( void );
	~sysImgui( void );
	
	bool initialize( GLFWwindow* window );
	void terminate( void );
	void frameBegin( void );
	void frameEnd( void );
	bool renderBegin( void );
	void renderEnd( void );
	
	void			mouseButtonCallback( GLFWwindow* window, int button, int action, int mods );
	void			scrollCallback( GLFWwindow* window, double xoffset, double yoffset );
	void			keyCallback( GLFWwindow* window, int key, int scancode, int action, int mods );
	void			charCallback( GLFWwindow* window, unsigned int c );
	
	GLFWwindow*		getWindow( void )						{ return m_window; }
	int				getShaderHandle( void ) const			{ return m_shaderHandle; }
	int				getAttribLocationTex( void ) const		{ return m_attribLocationTex; }
	int				getAttribLocationProjMtx( void ) const	{ return m_attribLocationProjMtx; }
	unsigned int	getVboHandle( void ) const				{ return m_vboHandle; }
	unsigned int	getVaoHandle( void ) const				{ return m_vaoHandle; }
	unsigned int	getElementsHandle( void ) const			{ return m_elementsHandle; }
	
	bool			isAppEnd( void ) const	{ return m_isAppEnd; }
	bool			isShowGui( void ) const	{ return m_isShowGui; }
	
private:
	void newFrame( void );
	
	void invalidateDeviceObjects( void );
	bool createDeviceObjects( void );
	
	bool createFontsTexture( void );
	
	void setMousePressed( int button, bool b ) {
		if( button >= 0 && button < MOUSE_BUTTON_NUM ) {
			m_mousePressed[button] = b;
		}
	}
	
	void addMouseWheel( float val ) {
		m_mouseWheel += val;
	}
	
private:
	static sysImgui*	m_instance;
	
	GLFWwindow*			m_window;
	double				m_time;
	bool				m_mousePressed[MOUSE_BUTTON_NUM];
	float				m_mouseWheel;
	GLuint				m_fontTexture;
	int					m_shaderHandle;
	int					m_vertHandle;
	int					m_fragHandle;
	int					m_attribLocationTex;
	int					m_attribLocationProjMtx;
	int					m_attribLocationPosition;
	int					m_attribLocationUV;
	int					m_attribLocationColor;
	unsigned int		m_vboHandle;
	unsigned int		m_vaoHandle;
	unsigned int		m_elementsHandle;
	
	bool				m_isAppEnd;
	bool				m_isShowGui;
	bool				m_isShowTestWindow;
};
