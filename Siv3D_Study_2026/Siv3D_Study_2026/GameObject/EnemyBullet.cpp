#include "EnemyBullet.h"

// 初期位置・速度・敵弾用の円形HitBoxを設定する
EnemyBullet::EnemyBullet(const Vec2& pos, const Vec2& velocity)
	: GameObject{ ObjectType::EnemyBullet, pos,
		std::make_shared<CircleHitBox>(pos, HitBoxRadius) }
	, m_velocity{ velocity } {
}

// 速度に経過時間をかけ、フレームレートに左右されない移動量にする
void EnemyBullet::update() {
	setPos(m_pos + m_velocity * Scene::DeltaTime());
}

// 敵弾を紫色の円で描く
void EnemyBullet::draw() const {
	Circle{ m_pos, HitBoxRadius }.draw(ColorF{ 0.85, 0.25, 1.0 });
}

// 少し余裕を持たせて、画面から十分離れた弾を削除対象にする
bool EnemyBullet::isOutsideScreen() const {
	const double margin = HitBoxRadius;

	return (m_pos.x < -margin)
		|| (Scene::Width() + margin < m_pos.x)
		|| (m_pos.y < -margin)
		|| (Scene::Height() + margin < m_pos.y);
}
