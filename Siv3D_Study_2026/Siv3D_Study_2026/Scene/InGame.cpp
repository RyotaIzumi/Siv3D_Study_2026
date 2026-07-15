#include "Scene.h"

// インゲームシーンのコンストラクタ
InGame::InGame(const InitData& data)
	: IScene(data)
	, m_player{ Scene::Center() }
	, m_enemy{ Vec2{ Scene::Center().x, 150.0 } } {
}

// インゲームシーンの更新処理
void InGame::update() {
	// プレイヤーのキー入力と移動を更新する
	m_player.update();
	m_enemy.update();

	// 生存中の敵に当たった弾を数え、1発につき1ダメージを与える
	if (!m_enemy.isDead()) {
		const int32 hitCount = m_player.countBulletHits(m_enemy);
		m_enemy.damage(hitCount);
		m_enemy.countBulletHits(m_player);
	}


	// Qキーが押されたら、タイトルシーンに遷移する
	if (KeyQ.down()) {
		changeScene(SceneType::Title, TRANSITION_DURATION);
	}
}

// インゲームシーンの描画処理
void InGame::draw() const {
	// 薄緑の背景を描画
	Scene::SetBackground(ColorF{ 0.6, 0.8, 0.7 });

	// 敵をプレイヤーより先に描画する
	m_enemy.draw();

	// 更新された座標にプレイヤーを描画する
	m_player.draw();

	// プレイヤーの操作方法を画面左上に表示する
	FontAsset(U"Guide")(U"Move: Arrow keys / WASD   Shot: Z").draw(20, 20, Palette::Black);

	// 敵のHPが0になったら撃破メッセージを表示する
	if (m_enemy.isDead()) {
		FontAsset(U"Guide")(U"Enemy defeated!")
			.drawAt(Scene::Center().x, 100, Palette::Black);
	}
}
