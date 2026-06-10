#include <Siv3D.hpp> // Siv3D v0.6.16
#include "Scene/Scene.h"

using App = SceneManager<SceneType>;

void Main()
{

	Window::SetTitle(U"Sample");

	// フォントの宣言
	FontAsset::Register(U"Title", 60, Typeface::Regular);

	// シーン管理クラスのインスタンスを生成
	App app;
	app.add<Title>(SceneType::Title);
	app.add<InGame>(SceneType::IN_GAME);
	app.init(SceneType::Title, 0s);

	// メインループ
	while (System::Update()) {
		if (not app.update()) {
			break;
		}
	}
}
