#include "Scene.h"

// インゲームシーンのコンストラクタ
InGame::InGame(const InitData& data)
	: IScene(data)
	, m_player{ Scene::Center() } {
}

// インゲームシーンの更新処理
void InGame::update() {
	// プレイヤーのキー入力と移動を更新する
	m_player.update();

	// Qキーが押されたら、タイトルシーンに遷移する
	if (KeyQ.down()) {
		changeScene(SceneType::Title, TRANSITION_DURATION);
	}
}

// インゲームシーンの描画処理
void InGame::draw() const {
	// 薄緑の背景を描画
	Scene::SetBackground(ColorF{ 0.6, 0.8, 0.7 });

	// 更新された座標にプレイヤーを描画する
	m_player.draw();

	// プレイヤーの操作方法を画面左上に表示する
	FontAsset(U"Guide")(U"Move: Arrow keys / WASD").draw(20, 20, Palette::Black);
}
