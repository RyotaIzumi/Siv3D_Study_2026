#include "Scene.h"

// タイトルシーンのコンストラクタ
Title::Title(const InitData& data) : IScene(data) {
}

// タイトルシーンの更新処理
void Title::update() {

	// スペースキーが押されたら、ゲームシーンに遷移する
	if (KeySpace.down()) {
		changeScene(SceneType::IN_GAME, TRANSITION_DURATION);
	}

}

// タイトルシーンの描画処理
void Title::draw() const {

	// 黒の背景を描画
	Scene::SetBackground(ColorF{ Palette::Black });
	// 文字表示
	FontAsset(U"Title")(U"Press space to start").drawAt(400, 304);

}
