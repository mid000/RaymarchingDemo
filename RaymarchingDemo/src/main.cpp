//------------------------------------------------------------------------------------------
//! @file	main.cpp
//! @brief	メイン関数定義
//! @author	
//! @date	
//------------------------------------------------------------------------------------------

#include "src/system/sysMain.h"

namespace {
	
	int WindowWidth = 640;
	int WindowHeight = 480;
	char WindowTitle[] = "Raymarching Demo";
	
}

//------------------------------------------------------------
//! @brief	アプリケーションのエントリーポイント
//! @param	argc	コマンドライン引数の総個数
//! @param	argv	コマンドライン引数の文字列
//! @return	終了ステータス
//------------------------------------------------------------
int main( int argc, char** argv )
{
	sysMain::createInstance();
	sysMain* systemMain = sysMain::getInstance();
	
	// 初期化
	systemMain->initialize( WindowTitle, WindowWidth, WindowHeight );
	
	// メインループ
	systemMain->mainLoop();
	
	// 後片付け
	systemMain->terminate();
	
	sysMain::destroyInstance();
	systemMain = nullptr;
	
	return 0;
}
