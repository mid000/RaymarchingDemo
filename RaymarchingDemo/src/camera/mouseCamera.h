//------------------------------------------------------------------------------------------
//! @file	mouseCamera.h
//! @brief	マウス操作カメラ定義
//! @author	
//! @date	
//------------------------------------------------------------------------------------------
#pragma once

struct vecParam {
	union {
		struct {
			float x;
			float y;
			float z;
		};
		
		float vec[3];
	};
};

struct MouseState {
	enum Type {
		  Push = 0
		, Release
		, Move
		, None
	};
};

struct mouseButton
{
	struct position
	{
		double x;
		double y;
		
		position( double nx = 0.0f, double ny = 0.0f ) { x = nx; y = ny; }
		~position( void ) {}
		
		void reset( void ) { x = 0.0f, y = 0.0f; }
	};
	
	mouseButton( void ) { reset(); }
	~mouseButton( void ) {};
	
	void reset() {
		before.x	= 0.0f;
		before.y	= 0.0f;
		current.x	= 0.0f;
		current.y	= 0.0f;
		state		= MouseState::None;
	}
	
	position			before;
	position			current;
	MouseState::Type	state;
};

class mouseCamera
{
public:
	mouseCamera( void );
	~mouseCamera( void );
	
	void reset( void );
	void renderGui( void );
	
	void mouseButtonCallback( GLFWwindow* window, int button, int action, int mods );
	void cursorPosCallback( GLFWwindow* window, double xpos, double ypos );
	void scrollCallback( GLFWwindow* window, double xoffset, double yoffset );
	
	void getRotate( vecParam& param ) {
		param.x = static_cast<float>( m_rotate[0] );
		param.y = static_cast<float>( m_rotate[1] );
		param.z = static_cast<float>( m_rotate[2] );
	};
	
	float getZoom( void ) const { return static_cast<float>( m_zoom ); }
	
private:
	mouseButton	m_right;
	mouseButton	m_left;
	mouseButton	m_middle;
	
	float		m_rotate[3];
	float		m_zoom;
};

