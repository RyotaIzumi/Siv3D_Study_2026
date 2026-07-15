#pragma once
#include "GameObject.h"
#include "EnemyBullet.h"

// 画面上部に配置され、プレイヤーの弾でダメージを受ける敵
class Enemy : public GameObject {
private:
	static constexpr double HitBoxRadius = 45.0;
	static constexpr int32 MaxHP = 10;
	static constexpr double MoveRange = 220.0;
	static constexpr double MoveSpeed = 2.0;
	static constexpr double ShotInterval = 1.0;

	Vec2 m_startPos;
	double m_moveTime = 0.0;
	double m_shotTimer = 0.0;
	int32 m_shotCount = 0;
	int32 m_hp = MaxHP;
	Array<EnemyBullet> m_bullets;

	// 一定時間ごとに弾幕を発射する
	void shoot();
	// サンプルの弾幕として、敵を中心に放射状へ弾を発射する
	void shootRadialBullets();
	// 発射済みの敵弾を移動し、画面外の弾を削除する
	void updateBullets();

public:
	Enemy(const Vec2& pos);

	// 敵を左右に往復移動させる
	void update() override;

	// 敵本体と現在のHPを表すHPバーを描く
	void draw() const override;

	// 指定したオブジェクトに当たった敵弾を削除し、命中した数を返す
	int32 countBulletHits(const GameObject& target);

	// 指定された値だけHPを減らす
	void damage(int32 amount);

	// HPが0になったかを返す
	bool isDead() const;
};
