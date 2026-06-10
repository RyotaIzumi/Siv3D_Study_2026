#include "Scene.h"

// インゲームシーンのコンストラクタ
InGame::InGame(const InitData& data) : IScene(data) {
}

// インゲームシーンの更新処理
void InGame::update() {
	// Qキーが押されたら、タイトルシーンに遷移する
	if (KeyQ.down()) {
		changeScene(SceneType::Title, TRANSITION_DURATION);
	}
}

// インゲームシーンの描画処理
void InGame::draw() const {
	// 薄緑の背景を描画
	Scene::SetBackground(ColorF{ 0.6, 0.8, 0.7 });
}
