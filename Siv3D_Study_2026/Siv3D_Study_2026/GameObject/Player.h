#pragma once
#include "GameObject.h"

// キー操作で上下左右に移動するプレイヤー
class Player : public GameObject {
private:
	static constexpr double MoveSpeed = 300.0;
	static constexpr double HitBoxRadius = 18.0;

	// 押されているキーから移動する方向を作る
	Vec2 getMoveDirection() const;
	// プレイヤーの中心が画面外へ出ない位置に直す
	void keepInsideScreen();

public:
	Player(const Vec2& pos);
	// キー入力を読み取り、プレイヤーを移動する
	void update() override;
	// プレイヤーを簡単な宇宙船の形で描く
	void draw() const override;
};
