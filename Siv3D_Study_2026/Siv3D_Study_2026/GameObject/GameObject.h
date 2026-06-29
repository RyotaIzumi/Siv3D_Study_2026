#pragma once
#include <Siv3D.hpp>
#include "HitBox.h"

// 当たり判定で相手の種類を見分けるための一覧
enum class ObjectType {
	Player,
	Enemy,
	PlayerBullet,
	EnemyBullet
};

// ゲーム内に登場する物が共通して持つ情報と処理をまとめた親クラス
class GameObject {
private:
	ObjectType m_type;

protected:
	Vec2 m_pos;
	std::shared_ptr<HitBox> m_hitBox;

	// 座標とHitBoxを同時に動かし、位置がずれないようにする
	void setPos(const Vec2& pos) {
		m_pos = pos;
		m_hitBox->setPos(m_pos);
	}

public:
	// 種類・初期位置・HitBoxを受け取り、共通の情報を準備する
	GameObject(ObjectType type, const Vec2& pos, std::shared_ptr<HitBox> hitBox)
		: m_type{ type }, m_pos{ pos }, m_hitBox{ std::move(hitBox) } {
	}

	// 親クラスのポインタから子クラスを安全に破棄するためのデストラクタ
	virtual ~GameObject() = default;
	// オブジェクトを1フレーム分更新する
	virtual void update() = 0;
	// オブジェクトを画面に描く
	virtual void draw() const = 0;
	// 現在の中心座標を返す
	const Vec2& getPos() const { return m_pos; }
	// PlayerやEnemyなど、このオブジェクトの種類を返す
	ObjectType getType() const { return m_type; }
	// 攻撃判定などから利用できるようHitBoxを返す
	const HitBox& getHitBox() const { return *m_hitBox; }
	// このオブジェクトと相手のHitBoxが重なっているかを調べる
	bool intersects(const GameObject& other) const {
		return m_hitBox->intersects(other.getHitBox());
	}
};
