#include "PlayerBullet.h"

// 初期位置と、プレイヤー弾用の円形HitBoxを設定する
PlayerBullet::PlayerBullet(const Vec2& pos)
	: GameObject{ ObjectType::PlayerBullet, pos,
		std::make_shared<CircleHitBox>(pos, HitBoxRadius) } {
}

// y座標を小さくすることで、弾を画面上方向へ移動させる
void PlayerBullet::update() {
	setPos(m_pos + Vec2{ 0.0, -MoveSpeed * Scene::DeltaTime() });
}

// 弾の見た目を水色の円として描く
void PlayerBullet::draw() const {
	Circle{ m_pos, HitBoxRadius }.draw(ColorF{ 0.3, 0.9, 1.0 });
}

// 弾の下端が0より小さければ、弾全体が画面外に出ている
bool PlayerBullet::isOutsideScreen() const {
	return (m_pos.y + HitBoxRadius) < 0.0;
}
