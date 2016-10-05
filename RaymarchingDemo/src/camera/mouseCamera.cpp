//------------------------------------------------------------------------------------------
//! @file	mouseCamera.cpp
//! @brief	マウス操作カメラ実装
//! @author	
//! @date	
//------------------------------------------------------------------------------------------

#include <iostream>
#include <cmath>
#include "src/system/sysImgui.h"
#include "src/camera/mouseCamera.h"

namespace {

const float RotateX_Min	= DegToRad( -180.0f );
const float RotateX_Max	= DegToRad(  180.0f );
const float RotateY_Min	= DegToRad( -89.0f );
const float RotateY_Max	= DegToRad(  89.0f );
const float Zoom_Min	= 0.0f;
const float Zoom_Max	= 3.0f;

}

//------------------------------------------------------------
//! @brief	コンストラクタ
//! @param	
//! @return	
//------------------------------------------------------------
mouseCamera::mouseCamera( void )
{
	reset();
}

//------------------------------------------------------------
//! @brief	デストラクタ
//! @param	
//! @return	
//------------------------------------------------------------
mouseCamera::~mouseCamera( void )
{
}

//------------------------------------------------------------
//! @brief	マウスボタンコールバック
//! @param	window	ウインドウ
//! @param	button	ボタンID
//! @param	action	アクションID
//! @param	mods	装飾キーID
//! @return	
//------------------------------------------------------------
void mouseCamera::mouseButtonCallback( GLFWwindow* window, int button, int action, int mods )
{
	switch( button )
	{
		// 左ボタン
		case GLFW_MOUSE_BUTTON_LEFT:
		{
			if( action == GLFW_PRESS ) {
				m_left.state	= MouseState::Push;
			}
			else if( action == GLFW_RELEASE ) {
				m_left.state	= MouseState::Release;
			}
		} break;
		
		// 右ボタン
		case GLFW_MOUSE_BUTTON_RIGHT:
		{
			if( action == GLFW_PRESS ) {
				m_right.state	= MouseState::Push;
			}
			else if( action == GLFW_RELEASE ) {
				m_right.state	= MouseState::Release;
			}
		} break;
		
		// 中ボタン
		case GLFW_MOUSE_BUTTON_MIDDLE:
		{
			if( action == GLFW_PRESS ) {
				m_middle.state	= MouseState::Push;
			}
			else if( action == GLFW_RELEASE ) {
				m_middle.state	= MouseState::Release;
			}
		} break;
	}
}

//------------------------------------------------------------
//! @brief	マウス移動コールバック
//! @param	window	ウインドウ
//! @param	xpos	X座標
//! @param	ypos	Y座標
//! @return	
//------------------------------------------------------------
void mouseCamera::cursorPosCallback( GLFWwindow* window, double xpos, double ypos )
{
	if( ImGui::IsAnyItemActive() ) return;	// GUI操作中
	
	// クリック → 移動開始
	bool isPush = false;
	if( m_left.state == MouseState::Push ) {
		m_left.current.x	= xpos;
		m_left.current.y	= ypos;
		m_left.state		= MouseState::Move;
		isPush				= true;
	}
//	if( m_right.state == MouseState::Push ) {
//		m_right.current.x	= xpos;
//		m_right.current.y	= ypos;
//		m_right.state		= MouseState::Move;
//		isPush				= true;
//	}
//	if( m_middle.state == MouseState::Push ) {
//		m_middle.current.x	= xpos;
//		m_middle.current.y	= ypos;
//		m_middle.state		= MouseState::Move;
//		isPush				= true;
//	}
	
	// 移動中
	if( isPush != true ) {
		// 左ボタンの処理
		if( m_left.state == MouseState::Move ) {
			m_left.before.x		= m_left.current.x;
			m_left.before.y		= m_left.current.y;
			m_left.current.x	= xpos;
			m_left.current.y	= ypos;
			
			// 移動量を計算
			const double move_x = m_left.current.x - m_left.before.x;
			const double move_y = m_left.current.y - m_left.before.y;
			
			const double scale_x = -0.5f;
			const double scale_y =  0.5f;
			m_rotate[0] = static_cast<float>( DegToRad( RadToDeg( m_rotate[0] ) + move_x * scale_x ) );
			m_rotate[1] = static_cast<float>( DegToRad( RadToDeg( m_rotate[1] ) + move_y * scale_y ) );
			
			if(      m_rotate[0] < RotateX_Min )	m_rotate[0] = RotateX_Min;
			else if( m_rotate[0] > RotateX_Max )	m_rotate[0] = RotateX_Max;
			
			if(      m_rotate[1] < RotateY_Min )	m_rotate[1] = RotateY_Min;
			else if( m_rotate[1] > RotateY_Max )	m_rotate[1] = RotateY_Max;
		}
		
//		// 右ボタンの処理
//		if( m_right.state == MouseState::Push ) {
//			m_right.before.x	= m_right.current.x;
//			m_right.before.y	= m_right.current.y;
//			m_right.current.x	= xpos;
//			m_right.current.y	= ypos;
//			
//			// 移動量を計算
//			const double move_x = m_right.current.x - m_right.before.x;
//			const double move_y = m_right.current.y - m_right.before.y;
//		}
//		
//		// 中ボタンの処理
//		if( m_middle.state == MouseState::Push ) {
//			m_middle.before.x	= m_middle.current.x;
//			m_middle.before.y	= m_middle.current.y;
//			m_middle.current.x	= xpos;
//			m_middle.current.y	= ypos;
//			
//			// 移動量を計算
//			const double move_x = m_middle.current.x - m_middle.before.x;
//			const double move_y = m_middle.current.y - m_middle.before.y;
//		}
	}
}

//------------------------------------------------------------
//! @brief	マウススクロールコールバック
//! @param	window	ウインドウ
//! @param	xoffset	Xオフセット
//! @param	yoffset	Yオフセット
//! @return	
//------------------------------------------------------------
void mouseCamera::scrollCallback( GLFWwindow* window, double xoffset, double yoffset )
{
	if( ImGui::IsAnyItemActive() ) return;	// GUI操作中
	
	const double scale = 0.5f;
	m_zoom	 = static_cast<float>( m_zoom + yoffset * scale );
	
	if(      m_zoom < Zoom_Min )	m_zoom = Zoom_Min;
	else if( m_zoom > Zoom_Max )	m_zoom = Zoom_Max;
}

//------------------------------------------------------------
//! @brief	リセット
//! @param	
//! @return	
//------------------------------------------------------------
void mouseCamera::reset( void )
{
	m_left.reset();
	m_right.reset();
	m_middle.reset();
	
	m_rotate[0]		= DegToRad( 0.0f );
	m_rotate[1]		= DegToRad( 0.0f );
	m_rotate[2]		= 0.0f;
	m_zoom			= 0.0f;
}

//------------------------------------------------------------
//! @brief	GUI描画
//! @param	
//! @return	
//------------------------------------------------------------
void mouseCamera::renderGui( void )
{
	if( ImGui::TreeNodeEx( "Camera", ImGuiTreeNodeFlags_DefaultOpen ) ) {
		ImGui::PushItemWidth( 80 );
		ImGui::SliderAngle( "Rotate X", &m_rotate[0], RadToDeg( RotateX_Min ), RadToDeg( RotateX_Max ) );
		ImGui::SliderAngle( "Rotate Y", &m_rotate[1], RadToDeg( RotateY_Min ), RadToDeg( RotateY_Max ) );
		ImGui::SliderFloat( "Zoom", &m_zoom, Zoom_Min, Zoom_Max );
		if( ImGui::Button( "Reset", ImVec2( 80.0f, 20.0f ) ) ) { reset(); }
		ImGui::PopItemWidth();
		ImGui::TreePop();
	}
}
