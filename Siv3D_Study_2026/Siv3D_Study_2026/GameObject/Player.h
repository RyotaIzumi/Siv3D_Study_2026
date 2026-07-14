#pragma once
#include "GameObject.h"
#include "PlayerBullet.h"

// キー操作で上下左右に移動するプレイヤー
class Player : public GameObject {
private:
	static constexpr double MoveSpeed = 300.0;
	static constexpr double HitBoxRadius = 18.0;
	Array<PlayerBullet> m_bullets;

	// 押されているキーから移動する方向を作る
	Vec2 getMoveDirection() const;
	// プレイヤーの中心が画面外へ出ない位置に直す
	void keepInsideScreen();
	// Zキーが押されたら、プレイヤーの正面に弾を作る
	void shoot();
	// 発射済みの弾を移動し、画面外の弾を削除する
	void updateBullets();

public:
	Player(const Vec2& pos);
	// 指定したオブジェクトに当たった弾を削除し、命中した数を返す
	int32 countBulletHits(const GameObject& target);
	// キー入力を読み取り、プレイヤーを移動する
	void update() override;
	// プレイヤーを簡単な宇宙船の形で描く
	void draw() const override;
};
