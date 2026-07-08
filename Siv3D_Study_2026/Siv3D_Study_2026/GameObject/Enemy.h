#pragma once
#include "GameObject.h"

// 画面上部に配置され、プレイヤーの弾でダメージを受ける敵
class Enemy : public GameObject {
private:
	static constexpr double HitBoxRadius = 45.0;
	static constexpr int32 MaxHP = 10;

	int32 m_hp = MaxHP;

public:
	Enemy(const Vec2& pos);

	// 今回の敵は移動しないため、更新処理は空にする
	void update() override;

	// 敵本体と現在のHPを表すHPバーを描く
	void draw() const override;

	// 指定された値だけHPを減らす
	void damage(int32 amount);

	// HPが0になったかを返す
	bool isDead() const;
};
