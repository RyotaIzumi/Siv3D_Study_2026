#pragma once
#include <Siv3D.hpp>

// 当たり判定の形を共通の方法で扱うための親クラス
class HitBox {
public:
	virtual ~HitBox() = default;

	// 別のHitBoxと重なっているかを調べる
	virtual bool intersects(const HitBox& other) const = 0;
	// ゲームオブジェクトの移動に合わせてHitBoxの中心も移動する
	virtual void setPos(const Vec2& pos) = 0;
	// HitBoxの中心座標を返す
	virtual Vec2 getCenterPos() const = 0;
	// 保持している形が円なら、その円を返す
	virtual const Circle* getCircle() const { return nullptr; }
	// 保持している形が長方形なら、その長方形を返す
	virtual const RectF* getRect() const { return nullptr; }
};

// 円形の当たり判定を表すクラス
class CircleHitBox : public HitBox {
private:
	Circle m_circle;

public:
	CircleHitBox(const Vec2& center, double radius);
	// 別のHitBoxと重なっているかを調べる
	bool intersects(const HitBox& other) const override;
	// 円の中心を指定した座標へ移動する
	void setPos(const Vec2& pos) override;
	// 円の中心座標を返す
	Vec2 getCenterPos() const override;
	// このHitBoxが持つ円を返す
	const Circle* getCircle() const override;
};

// 長方形の当たり判定を表すクラス
class RectHitBox : public HitBox {
private:
	RectF m_rect;

public:
	RectHitBox(const Vec2& center, const SizeF& size);
	// 別のHitBoxと重なっているかを調べる
	bool intersects(const HitBox& other) const override;
	// 長方形の中心を指定した座標へ移動する
	void setPos(const Vec2& pos) override;
	// 長方形の中心座標を返す
	Vec2 getCenterPos() const override;
	// このHitBoxが持つ長方形を返す
	const RectF* getRect() const override;
};
