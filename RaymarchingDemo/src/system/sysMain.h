//------------------------------------------------------------------------------------------
//! @file	sysMain.h
//! @brief	メインシステム定義
//! @author	
//! @date	
//------------------------------------------------------------------------------------------
#pragma once

#include <vector>
#include "src/system/sysImgui.h"
#include "src/camera/mouseCamera.h"
#include "src/renderer/raymarchingRenderer.h"

struct GLFWwindow;

//------------------------------------------------------------
// メインシステムクラス
//------------------------------------------------------------
class sysMain
{
public:
	static void createInstance( void );
	static void destroyInstance( void );
	
	static sysMain*	getInstance( void ) { return m_instance; }
	
	sysMain( void );
	~sysMain( void );
	
	bool		initialize( const char* windowTitle, int windowWidth, int windowHeight );
	void		terminate( void );
	
	void		mainLoop( void );
	
	GLFWwindow*		getWindow( void )	{ return m_window; }
	mouseCamera*	getCamera( void )	{ return &m_camera; }
	
private:
	void	update( void );
	void	render( void );
	
private:
	static sysMain*		m_instance;
	
	GLFWwindow*			m_window;
	
	mouseCamera			m_camera;
	raymarchingRenderer	m_raymarching;
};
