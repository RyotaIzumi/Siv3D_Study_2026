#pragma once
#include "GameObject.h"

// プレイヤーが発射し、画面上方向へ進む弾
class PlayerBullet : public GameObject {
private:
	static constexpr double MoveSpeed = 500.0;
	static constexpr double HitBoxRadius = 6.0;

public:
	PlayerBullet(const Vec2& pos);

	// 弾を1フレーム分、上方向へ移動する
	void update() override;

	// 弾を円として画面に描く
	void draw() const override;

	// 弾全体が画面上端を越えたかを調べる
	bool isOutsideScreen() const;
};
