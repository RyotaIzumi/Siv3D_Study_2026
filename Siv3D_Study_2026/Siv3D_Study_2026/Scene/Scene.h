#pragma once

#include "../GameObject/Player.h"

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

public:
	InGame(const InitData& data);

	void update() override;
	void draw() const override;
};
