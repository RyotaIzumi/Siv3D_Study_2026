#pragma once
#include "GameObject.h"

// 敵から発射される弾
class EnemyBullet : public GameObject {
private:
	static constexpr double HitBoxRadius = 8.0;

	Vec2 m_velocity;

public:
	EnemyBullet(const Vec2& pos, const Vec2& velocity);

	// 弾を速度の向きへ移動させる
	void update() override;

	// 敵弾を紫色の円として描く
	void draw() const override;

	// 弾が画面外に出たかを返す
	bool isOutsideScreen() const;
};
