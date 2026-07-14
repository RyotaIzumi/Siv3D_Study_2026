#include "Enemy.h"

// 初期位置と、敵用の円形HitBoxを設定する
Enemy::Enemy(const Vec2& pos)
	: GameObject{ ObjectType::Enemy, pos,
		std::make_shared<CircleHitBox>(pos, HitBoxRadius) }
	, m_startPos{ pos } {
}

// sinの値を使って、初期位置を中心に左右へなめらかに往復移動させる
void Enemy::update() {
	if (isDead()) {
		return;
	}

	m_moveTime += Scene::DeltaTime();

	const double moveX = Math::Sin(m_moveTime * MoveSpeed) * MoveRange;
	setPos(m_startPos + Vec2{ moveX, 0.0 });
}

// 敵を赤い円で描き、その上に残りHPを表すバーを描く
void Enemy::draw() const {
	if (isDead()) {
		return;
	}

	Circle{ m_pos, HitBoxRadius }.draw(ColorF{ 0.9, 0.2, 0.2 });

	const double barWidth = 120.0;
	const double barHeight = 10.0;
	const double hpRate = static_cast<double>(m_hp) / MaxHP;
	const Vec2 barPos{ m_pos.x - barWidth / 2.0,
		m_pos.y - HitBoxRadius - 20.0 };

	RectF{ barPos, barWidth, barHeight }.draw(ColorF{ 0.2 });
	RectF{ barPos, barWidth * hpRate, barHeight }
	.draw(ColorF{ 0.2, 0.9, 0.3 });
}

// HPが0より小さくならないようにしながらダメージを与える
void Enemy::damage(int32 amount) {
	m_hp = Max(0, m_hp - amount);
}

// HPが0以下なら敵が倒されたと判断する
bool Enemy::isDead() const {
	return (m_hp <= 0);
}
