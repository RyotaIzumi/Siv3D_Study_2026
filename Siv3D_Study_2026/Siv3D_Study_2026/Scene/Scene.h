#pragma once

#include "../GameObject/Player.h"

// シーン変更時のフェードイン・アウトの時間
static const Duration TRANSITION_DURATION = 0.50s;

// シーンの種類
enum class SceneType {
	Title,
	IN_GAME
};

using App = SceneManager<SceneType>;

// ----- 各シーンのクラス ----- //

class Title : public App::Scene {
private:
public:
	Title(const InitData& data);

	void update() override;
	void draw() const override;
};

class InGame : public App::Scene {
private:
	Player m_player;

public:
	InGame(const InitData& data);

	void update() override;
	void draw() const override;
};
