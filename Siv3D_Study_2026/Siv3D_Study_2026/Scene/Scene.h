#pragma once

#include "../GameObject/Player.h"
#include "../GameObject/Enemy.h"

// シーン変更時のフェードイン・アウトの時間
static const Duration TRANSITION_DURATION = 0.50s;

// シーンの種類
enum class SceneType {
	Title,
	IN_GAME
};

// ----- 各シーンのクラス ----- //

class Title : public SceneManager<SceneType>::Scene {
private:
public:
	Title(const InitData& data);

	void update() override;
	void draw() const override;
};

class InGame : public SceneManager<SceneType>::Scene {
private:
	Player m_player;
	Enemy m_enemy;
	bool m_isGameOver = false;

	// プレイヤー・敵・ゲーム状態を初期状態に戻す
	void resetGame();

public:
	InGame(const InitData& data);

	void update() override;
	void draw() const override;
};
