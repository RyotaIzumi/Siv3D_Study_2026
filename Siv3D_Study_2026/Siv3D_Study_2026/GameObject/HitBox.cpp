#include "HitBox.h"

// 中心座標と半径から円形のHitBoxを作る
CircleHitBox::CircleHitBox(const Vec2& center, double radius)
	: m_circle{ center, radius } {
}

// 相手の形を調べ、円同士または円と長方形の重なりを判定する
bool CircleHitBox::intersects(const HitBox& other) const {
	if (const Circle* circle = other.getCircle()) {
		return m_circle.intersects(*circle);
	}
	if (const RectF* rect = other.getRect()) {
		return m_circle.intersects(*rect);
	}
	return false;
}

// プレイヤーなどの移動に合わせて円の中心を動かす
void CircleHitBox::setPos(const Vec2& pos) {
	m_circle.setCenter(pos);
}

// 円の中心座標を返す
Vec2 CircleHitBox::getCenterPos() const {
	return m_circle.center;
}

// 他のHitBoxから形を確認できるよう、円へのポインタを返す
const Circle* CircleHitBox::getCircle() const {
	return &m_circle;
}

// 中心座標と大きさから長方形のHitBoxを作る
RectHitBox::RectHitBox(const Vec2& center, const SizeF& size)
	: m_rect{ Arg::center(center), size } {
}

// 相手の形を調べ、長方形と円または長方形同士の重なりを判定する
bool RectHitBox::intersects(const HitBox& other) const {
	if (const Circle* circle = other.getCircle()) {
		return m_rect.intersects(*circle);
	}
	if (const RectF* rect = other.getRect()) {
		return m_rect.intersects(*rect);
	}
	return false;
}

// プレイヤーなどの移動に合わせて長方形の中心を動かす
void RectHitBox::setPos(const Vec2& pos) {
	m_rect.setCenter(pos);
}

// 長方形の中心座標を返す
Vec2 RectHitBox::getCenterPos() const {
	return m_rect.center();
}

// 他のHitBoxから形を確認できるよう、長方形へのポインタを返す
const RectF* RectHitBox::getRect() const {
	return &m_rect;
}
