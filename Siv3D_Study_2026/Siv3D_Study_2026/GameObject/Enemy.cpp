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
		m_bullets.clear();
		return;
	}

	m_moveTime += Scene::DeltaTime();

	const double moveX = Math::Sin(m_moveTime * MoveSpeed) * MoveRange;
	setPos(m_startPos + Vec2{ moveX, 0.0 });

	shoot();
	updateBullets();
}

// 発射間隔を管理し、時間が来たら弾幕を発射する
void Enemy::shoot() {
	m_shotTimer += Scene::DeltaTime();

	if (ShotInterval <= m_shotTimer) {
		shootRadialBullets();
		m_shotTimer -= ShotInterval;
	}
}

// 角度を少しずつ変えながら、敵の周りへ同時に弾を発射する
void Enemy::shootRadialBullets() {
	const int32 bulletCount = 16;
	const double bulletSpeed = 150.0;
	const double angleOffset = m_shotCount * 0.15;

	for (int32 i = 0; i < bulletCount; ++i) {
		const double angle = angleOffset + Math::TwoPi * i / bulletCount;
		const Vec2 direction{ Math::Cos(angle), Math::Sin(angle) };

		m_bullets.emplace_back(m_pos, direction * bulletSpeed);
	}

	++m_shotCount;
}

// すべての敵弾を更新し、画面外へ出た弾を配列から取り除く
void Enemy::updateBullets() {
	for (auto& bullet : m_bullets) {
		bullet.update();
	}

	m_bullets.remove_if([](const EnemyBullet& bullet) {
		return bullet.isOutsideScreen();
	});
}

// 敵を赤い円で描き、その上に残りHPを表すバーを描く
void Enemy::draw() const {
	for (const auto& bullet : m_bullets) {
		bullet.draw();
	}

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

// 対象に当たった敵弾を消し、何発当たったかを数える
int32 Enemy::countBulletHits(const GameObject& target) {
	int32 hitCount = 0;

	m_bullets.remove_if([&](const EnemyBullet& bullet) {
		if (bullet.intersects(target)) {
			++hitCount;
			return true;
		}

		return false;
	});

	return hitCount;
}

// HPが0より小さくならないようにしながらダメージを与える
void Enemy::damage(int32 amount) {
	m_hp = Max(0, m_hp - amount);
}

// HPが0以下なら敵が倒されたと判断する
bool Enemy::isDead() const {
	return (m_hp <= 0);
}
