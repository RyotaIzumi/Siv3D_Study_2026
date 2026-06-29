#include "Player.h"

// 初期位置と、プレイヤー用の円形HitBoxを設定する
Player::Player(const Vec2& pos)
	: GameObject{ ObjectType::Player, pos,
		std::make_shared<CircleHitBox>(pos, HitBoxRadius) } {
}

// 矢印キーまたはWASDキーから、縦横それぞれの移動方向を求める
Vec2 Player::getMoveDirection() const {
	Vec2 direction{ 0.0, 0.0 };
	if (KeyLeft.pressed() || KeyA.pressed()) direction.x -= 1.0;
	if (KeyRight.pressed() || KeyD.pressed()) direction.x += 1.0;
	if (KeyUp.pressed() || KeyW.pressed()) direction.y -= 1.0;
	if (KeyDown.pressed() || KeyS.pressed()) direction.y += 1.0;
	return direction;
}

// 入力方向へ移動し、最後に画面外へ出ていないかを確認する
void Player::update() {
	Vec2 direction = getMoveDirection();

	// 斜め移動だけ速くならないよう、方向ベクトルの長さを1にそろえる
	if (not direction.isZero()) {
		direction.normalize();
	}

	setPos(m_pos + direction * MoveSpeed * Scene::DeltaTime());
	keepInsideScreen();
}

// Clampでx座標とy座標を画面内の範囲に収める
void Player::keepInsideScreen() {
	const double x = Clamp(m_pos.x, HitBoxRadius, Scene::Width() - HitBoxRadius);
	const double y = Clamp(m_pos.y, HitBoxRadius, Scene::Height() - HitBoxRadius);
	setPos(Vec2{ x, y });
}

// 三角形と円を組み合わせ、上向きの宇宙船として描く
void Player::draw() const {
	Triangle{
		m_pos + Vec2{ 0, -18 },
		m_pos + Vec2{ 16, 16 },
		m_pos + Vec2{ -16, 16 }
	}.draw(ColorF{ 0.2, 0.55, 1.0 });
	Circle{ m_pos, 7 }.draw(ColorF{ 0.8, 0.95, 1.0 });
}
