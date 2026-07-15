# 目次
https://qiita.com/Ryota123/private/ad363a4778e02b4035a2
***
<!-- Qiita投稿時に目次リンクを追加する -->

前回は、Zキーでプレイヤーの弾を発射する処理を作りました。

https://qiita.com/Ryota123/items/f5b4315de03f6a9b9686

<!-- Part 3公開後、ここへリンクを追加する -->

今回は画面上部に敵を追加し、プレイヤーの弾が敵へ当たるようにします。

敵はHPを持ち、弾が1発当たるたびにHPが1減ります。HPが0になると敵が消え、撃破メッセージが表示されます。

使用するものは、Siv3D v0.6.16、Visual Studio 2026です。

## 今回作るもの

今回の完成条件は次の通りです。

- 画面上部に敵を1体表示する
- 敵も`GameObject`を継承し、円形の`HitBox`を持つ
- 敵の上にHPバーを表示する
- プレイヤーの弾と敵のHitBoxが重なったか調べる
- 命中した弾を配列から削除する
- 弾1発につき敵のHPを1減らす
- HPが0になると敵を消し、撃破メッセージを表示する

![image.png](https://qiita-image-store.s3.ap-northeast-1.amazonaws.com/0/3821471/09e628cc-1f14-4599-aa91-2cedd20e0c0c.png)

今回の敵は、ボスの土台として画面上部に固定します。敵の移動や攻撃は次回以降に追加します。

## Enemy用のファイルを追加する

`GameObject`フォルダへ、次の2ファイルを追加します。

- Enemy.h
- Enemy.cpp

![image.png](https://qiita-image-store.s3.ap-northeast-1.amazonaws.com/0/3821471/035c3966-0bba-4ba5-ad3c-fff39aab597a.png)


## 動作確認用のEnemyを用意する

まず、敵を表す`Enemy`クラスを作ります。

敵もプレイヤーや弾と同じゲーム内オブジェクトです。そのため、`GameObject`を継承し、共通の座標やHitBoxを利用します。

`Enemy.h`は最初から次のコードを使用してください。

```cpp:GameObject/Enemy.h
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
```

今回は次の情報と関数を用意します。

- `HitBoxRadius`：敵の円形HitBoxの半径
- `MaxHP`：敵の最大HP
- `m_hp`：現在のHP
- `update()`：敵を毎フレーム更新する
- `draw()`：敵とHPバーを描画する
- `damage()`：敵のHPを減らす
- `isDead()`：敵を倒したか調べる

敵の最大HPは`10`、HitBoxの半径は`45.0`にしました。

`m_hp`はゲーム中に変化する現在HPです。最初は最大HPと同じ値から始めます。

```cpp
int32 m_hp = MaxHP;
```

`int32`は整数を保存する型です。今回のHPは`10`、`9`、`8`というように整数で減るため、`int32`を使用します。

## Enemy.cppへ穴埋めコードを書く

次に`Enemy.cpp`へ、次の穴埋め用コードを書きます。

```cpp:GameObject/Enemy.cpp
#include "Enemy.h"

// 初期位置と、敵用の円形HitBoxを設定する
Enemy::Enemy(const Vec2& pos)
	: GameObject{ ObjectType::Enemy, pos,
		std::make_shared<CircleHitBox>(pos, HitBoxRadius) } {
}

// 今回の敵は画面上部から動かないため、処理は何も書かない
void Enemy::update() {
}

// 課題1で、敵本体とHPバーを描く
void Enemy::draw() const {
}

// 課題3で、HPを減らす処理を書く
void Enemy::damage(int32 amount) {
}

// 課題4で、敵を倒したか調べる処理を書く
bool Enemy::isDead() const {
	return false;
}
```

この状態では、まだ敵は描画されず、ダメージも受けません。ただし、すべての関数が定義されているため、これから追加する呼び出し側のコードを含めてビルドできます。

### Enemyのコンストラクタ

コンストラクタでは、親クラスの`GameObject`へ次の3つを渡します。

- オブジェクトの種類：`ObjectType::Enemy`
- 初期位置：`pos`
- 円形HitBox：`std::make_shared<CircleHitBox>(pos, HitBoxRadius)`

```cpp
Enemy::Enemy(const Vec2& pos)
	: GameObject{ ObjectType::Enemy, pos,
		std::make_shared<CircleHitBox>(pos, HitBoxRadius) } {
}
```

### update()が空でよい理由

今回の敵は画面上部から動きません。そのため、毎フレーム行う更新処理はまだありません。

しかし、`GameObject`では`update()`が純粋仮想関数になっているため、処理がなくても関数自体は実装する必要があります。

```cpp
void Enemy::update() {
}
```

後から敵の移動や攻撃を追加するときは、この関数へ処理を書きます。

## Playerへ命中数を数える関数を追加する

プレイヤーは、発射済みの弾を`m_bullets`へ保存しています。

敵と重なった弾を調べるには、弾の配列へアクセスできる`Player`側に関数を追加します。

`Player.h`の`public`へ、次の宣言を追加してください。

```diff_cpp:GameObject/Player.h
 public:
 	Player(const Vec2& pos);
+	// 指定したオブジェクトに当たった弾を削除し、命中した数を返す
+	int32 countBulletHits(const GameObject& target);
```

この関数は、指定されたゲームオブジェクトと重なった弾を削除し、命中した弾の数を返します。

`Enemy`だけを受け取るのではなく、親クラスの`GameObject`を受け取る形にしています。この勉強会では増やす予定はありませんが、別の敵を作りたい場合でも、この関数を利用できます。

`Player.cpp`へは、課題2で完成させる穴埋めコードを追加します。

```cpp
// 課題2で、対象に当たった弾を削除して命中数を返す
int32 Player::countBulletHits(const GameObject& target) {
	return 0;
}
```

今は必ず`0`を返すため、弾は敵に当たりません。ただし関数は呼び出せるため、シーン側の土台を先に用意できます。

## InGameへ敵と当たり判定を追加する

続いて、ゲームシーンへ敵を追加します。

### Scene.hへEnemyを追加する

`Scene.h`で`Enemy.h`を読み込みます。

```diff_cpp:Scene/Scene.h
 #include "../GameObject/Player.h"
+#include "../GameObject/Enemy.h"
```

`InGame`へ、敵を保存するメンバ変数を追加します。

```diff_cpp:Scene/Scene.h
 class InGame : public SceneManager<SceneType>::Scene {
 private:
 	Player m_player;
+	Enemy m_enemy;
```

### 画面上部にEnemyを作る

`InGame`のコンストラクタで、敵の初期位置を設定します。

```diff_cpp:Scene/InGame.cpp
 InGame::InGame(const InitData& data)
 	: IScene(data)
-	, m_player{ Scene::Center() } {
+	, m_player{ Scene::Center() }
+	, m_enemy{ Vec2{ Scene::Center().x, 150.0 } } {
 }
```

敵のx座標には画面中央、y座標には`150.0`を指定しています。

```cpp
Vec2{ Scene::Center().x, 150.0 }
```

これにより、敵が画面上部の中央に配置されます。

### Enemyを更新する

`InGame::update()`で、敵の`update()`を呼びます。

```diff_cpp
 void InGame::update() {
 	m_player.update();
+	m_enemy.update();
```

今は空の関数ですが、後から敵の移動を追加しても、シーン側の呼び出しを変更せずに済みます。

### 弾の命中数をダメージとして渡す

敵が生きている間だけ、プレイヤー弾との当たり判定を行います。

```cpp
// 生存中の敵に当たった弾を数え、1発につき1ダメージを与える
if (!m_enemy.isDead()) {
	const int32 hitCount = m_player.countBulletHits(m_enemy);
	m_enemy.damage(hitCount);
}
```

`countBulletHits()`が返す命中数を`hitCount`へ保存し、その数だけ敵へダメージを与えます。

例えば同じフレームに弾が2発当たった場合、`hitCount`は`2`になり、HPも2減ります。

今は`isDead()`が必ず`false`、`countBulletHits()`が必ず`0`、`damage()`が空なので、まだ敵の状態は変化しません。

### Enemyを描画する

`InGame::draw()`で、プレイヤーより先に敵を描画します。

```diff_cpp
 void InGame::draw() const {
 	Scene::SetBackground(ColorF{ 0.6, 0.8, 0.7 });

+	// 敵をプレイヤーより先に描画する
+	m_enemy.draw();

 	m_player.draw();
```

### 撃破メッセージを用意する

`InGame::draw()`の最後へ、敵を倒したときの表示を追加します。

```cpp
// 敵のHPが0になったら撃破メッセージを表示する
if (m_enemy.isDead()) {
	FontAsset(U"Guide")(U"Enemy defeated!")
		.drawAt(Scene::Center().x, 100, Palette::Black);
}
```

今は`isDead()`が必ず`false`なので、まだメッセージは表示されません。課題4を完成させたときに表示されます。

## 土台がビルドできるか確認する

ここまで書いたら、一度ビルドして実行してください。

この段階では、敵はまだ表示されません。弾も敵を通り抜けます。これは正しい状態です。

次の動作が壊れていないことだけ確認してください。

- プレイヤーが上下左右に移動できる
- Zキーで弾を発射できる
- 弾が上方向へ進む

もしビルドできない場合は、次の点を確認してください。

- `Enemy.cpp`をVisual Studioのプロジェクトへ追加したか
- `Scene.h`で`Enemy.h`を読み込んでいるか
- `Enemy`の`update()`、`draw()`、`isDead()`が定義されているか
- `Player::countBulletHits()`の穴埋め関数を追加したか

これで、各課題のコードがゲームから呼ばれる準備ができました。

## 課題1：敵とHPバーを描画する

最初の課題では、`Enemy::draw()`を完成させます。

敵本体は赤い円として描きます。その上には、現在HPを表す緑色のHPバーを描きます。

最初に、敵が倒されていたら何も描かずに関数を終了します。

```cpp
if (isDead()) {
	return;
}
```

今は`isDead()`が必ず`false`なので、敵は必ず描画されます。

次に、円の描画や、hpバーの大きさ定義なども追加します。

- `m_pos`を中心に、半径`HitBoxRadius`の赤い円を描く
- HPバーの幅は`120.0`、高さは`10.0`にする
- 現在HPを最大HPで割り、HPの割合を求める
- HPの割合に応じて、緑色のバーの幅を変える

```cpp
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

    // hpバーを構成する図形描画 //
}
```

あとはhpバーを構成する図形を描画してあげるだけです。ここを書いてみましょう！
長方形の描画には、`Rect`や`RectF`が使えます。

https://siv3d.github.io/ja-jp/tutorial/rect/

<details>
<summary>ヒント</summary>

必要な長方形は2つあります。
- 長さの変わらないバー(黒)
- hpによって幅が変化するバー(緑)

</details>


<details>
<summary>解答例</summary>

```cpp
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
```

</details>

### 課題1の動作確認

ビルドして実行してください。

次の3点を確認できれば成功です。

- 画面上部の中央に赤い敵が表示される
- 敵の上に緑色のHPバーが表示される
- 弾はまだ敵を通り抜け、HPバーも減らない

弾が通り抜けるのは、`countBulletHits()`がまだ必ず`0`を返しているためです。

## 課題2：敵に当たった弾を削除する

次は、プレイヤーの弾と敵の当たり判定を作ります。

`GameObject`には、別のゲームオブジェクトとHitBoxが重なっているか調べる`intersects()`があります。

```cpp
bool intersects(const GameObject& other) const {
	return m_hitBox->intersects(other.getHitBox());
}
```

この関数を使い、弾と敵が重なっているか調べます。

`Player::countBulletHits()`を次の条件で完成させてください。

1. 命中数を保存する`hitCount`を`0`で用意する
2. `m_bullets.remove_if()`ですべての弾を調べる
3. `bullet.intersects(target)`が`true`なら命中数を1増やす
4. 命中した弾は`true`、命中していない弾は`false`を返す
5. 最後に`hitCount`を返す

途中まで埋めるとこうなります。

```cpp
int32 Player::countBulletHits(const GameObject& target) {
	int32 hitCount = 0;

	m_bullets.remove_if([&](const PlayerBullet& bullet) {
        // ここからhitCountを変更できる
	});

    // ???を返す //
}
```

`remove_if()`のラムダ式から、外側にある`hitCount`を変更する必要があります。

```cpp
m_bullets.remove_if([&](const PlayerBullet& bullet) {
	// ここからhitCountを変更できる
});
```

`[&]`は、外側の変数(ここでは`countBulletHits`)を参照として利用することを表します。

<details>
<summary>ヒント：命中したときの処理</summary>

```cpp
if (bullet.intersects(target)) {
	++hitCount;
	return true;
}
```

`remove_if()`では、`true`を返した弾が配列から削除されます。

</details>

<details>
<summary>解答例</summary>

```cpp
int32 Player::countBulletHits(const GameObject& target) {
	int32 hitCount = 0;

	m_bullets.remove_if([&](const PlayerBullet& bullet) {
		if (bullet.intersects(target)) {
			++hitCount;
			return true;
		}

		return false;
	});

	return hitCount;
}
```

</details>

### 課題2の動作確認

ビルドして実行し、敵へ向かって弾を撃ってください。

次の2点を確認できれば成功です。

- 敵へ当たった弾が、その場で消える
- 敵へ当たらなかった弾は、そのまま上へ進む

この段階では、弾が当たってもHPバーは減りません。`countBulletHits()`は命中数を返していますが、`Enemy::damage()`の中身がまだ空だからです。

「弾が消えるがHPは減らない」という状態になれば、当たり判定までは正しく動いています。

## 課題3：敵のHPを減らす

次は、`Enemy::damage()`を完成させます。

引数の`amount`には、そのフレームに命中した弾の数が渡されます。

```cpp
m_enemy.damage(hitCount);
```

現在HPの`m_hp`から`amount`を引いてください。ただし、HPが0より小さくならないようにします。

```cpp
void Enemy::damage(int32 amount) {
	// ここにHPを減らす処理を書く
}
```

やり方はいろいろありますが、便利な関数があるので紹介します。
`Max(a, b)`は、2つの値のうち大きい方を返します。ぜひ使ってみましょう。

https://siv3d.github.io/ja-jp/tutorial2/utility/

<details>
<summary>解答例</summary>

```cpp
void Enemy::damage(int32 amount) {
	m_hp = Max(0, m_hp - amount);
}
```

</details>

### 課題3の動作確認

ビルドして実行し、敵へ弾を当ててください。

次の3点を確認できれば成功です。

- 弾が1発当たるたびにHPバーが少し短くなる
- 5発当てるとHPバーがおよそ半分になる
- 10発当てるとHPバーが空になる

ただし、HPが0になっても敵はまだ画面に残ります。`isDead()`が必ず`false`を返しているためです。

次の課題で、HPが0になったら敵を倒したと判断できるようにします。

## 課題4：敵の撃破を判定する

最後に、`Enemy::isDead()`を完成させます。

現在HPの`m_hp`が`0`以下なら`true`、HPが残っているなら`false`を返してください。

```cpp
bool Enemy::isDead() const {
	// ここに条件式を書く
}
```

<details>
<summary>解答例</summary>

```cpp
bool Enemy::isDead() const {
	return (m_hp <= 0);
}
```

</details>

`Enemy::draw()`の最初では、`isDead()`が`true`なら描画せずに関数を終了します。

```cpp
if (isDead()) {
	return;
}
```

また、`InGame::draw()`では同じ関数を使い、撃破メッセージを表示します。

### 課題4の動作確認

ビルドして実行し、敵へ10発の弾を当ててください。

次の3点を確認できれば成功です。

- HPが0になると敵とHPバーが消える
- 敵がいた場所へ「Enemy defeated!」と表示される
- 撃破後に発射した弾は、敵がいた場所を通過する

これで、敵の生成から撃破までの処理が完成しました。

## Enemy.cppの完成コード

4つの課題が終わった`Enemy.cpp`は次のようになります。

```cpp:GameObject/Enemy.cpp
#include "Enemy.h"

// 初期位置と、敵用の円形HitBoxを設定する
Enemy::Enemy(const Vec2& pos)
	: GameObject{ ObjectType::Enemy, pos,
		std::make_shared<CircleHitBox>(pos, HitBoxRadius) } {
}

// 今回の敵は画面上部から動かないため、処理は何も書かない
void Enemy::update() {
}

// 敵を赤い円で描き、その上に残りHPを表すバーを描く
void Enemy::draw() const {
	if (isDead()) {
		return;
	}

	Circle{ m_pos, HitBoxRadius }.draw(ColorF{ 0.9, 0.2, 0.2 });
	Circle{ m_pos + Vec2{ -15, -8 }, 6 }.draw(Palette::White);
	Circle{ m_pos + Vec2{ 15, -8 }, 6 }.draw(Palette::White);
	Circle{ m_pos + Vec2{ -15, -8 }, 3 }.draw(Palette::Black);
	Circle{ m_pos + Vec2{ 15, -8 }, 3 }.draw(Palette::Black);

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
```

## Playerへ追加した完成コード

`Player.h`へ追加した宣言は次の通りです。

```cpp
// 指定したオブジェクトに当たった弾を削除し、命中した数を返す
int32 countBulletHits(const GameObject& target);
```

`Player.cpp`へ追加した関数は次の通りです。

```cpp
// 対象と重なった弾を配列から取り除き、ダメージに使う命中数を返す
int32 Player::countBulletHits(const GameObject& target) {
	int32 hitCount = 0;

	m_bullets.remove_if([&](const PlayerBullet& bullet) {
		if (bullet.intersects(target)) {
			++hitCount;
			return true;
		}

		return false;
	});

	return hitCount;
}
```

## Scene.hの変更後コード

今回の変更を含む`Scene.h`は次のようになります。

```cpp:Scene/Scene.h
#pragma once

#include "../GameObject/Player.h"
#include "../GameObject/Enemy.h"

// シーン変更時のフェードイン・アウトの時間
static const Duration TRANSITION_DURATION = 0.50s;

// シーンの種類
enum class SceneType {
	Title,
	IN_GAME
};

// ----- 各シーンのクラス ----- //

class Title : public SceneManager<SceneType>::Scene {
private:
public:
	Title(const InitData& data);

	void update() override;
	void draw() const override;
};

class InGame : public SceneManager<SceneType>::Scene {
private:
	Player m_player;
	Enemy m_enemy;

public:
	InGame(const InitData& data);

	void update() override;
	void draw() const override;
};
```

## InGame.cppの完成コード

今回の変更を含む`InGame.cpp`は次のようになります。

```cpp:Scene/InGame.cpp
#include "Scene.h"

// インゲームシーンのコンストラクタ
InGame::InGame(const InitData& data)
	: IScene(data)
	, m_player{ Scene::Center() }
	, m_enemy{ Vec2{ Scene::Center().x, 100.0 } } {
}

// インゲームシーンの更新処理
void InGame::update() {
	// プレイヤーのキー入力と移動を更新する
	m_player.update();
	m_enemy.update();

	// 生存中の敵に当たった弾を数え、1発につき1ダメージを与える
	if (not m_enemy.isDead()) {
		const int32 hitCount = m_player.countBulletHits(m_enemy);
		m_enemy.damage(hitCount);
	}

	// Qキーが押されたら、タイトルシーンに遷移する
	if (KeyQ.down()) {
		changeScene(SceneType::Title, TRANSITION_DURATION);
	}
}

// インゲームシーンの描画処理
void InGame::draw() const {
	// 薄緑の背景を描画
	Scene::SetBackground(ColorF{ 0.6, 0.8, 0.7 });

	// 敵をプレイヤーより先に描画する
	m_enemy.draw();

	// 更新された座標にプレイヤーを描画する
	m_player.draw();

	// プレイヤーの操作方法を画面左上に表示する
	FontAsset(U"Guide")(U"Move: Arrow keys / WASD   Shot: Z")
		.draw(20, 20, Palette::Black);

	// 敵のHPが0になったら撃破メッセージを表示する
	if (m_enemy.isDead()) {
		FontAsset(U"Guide")(U"Enemy defeated!")
			.drawAt(Scene::Center().x, 100, Palette::Black);
	}
}
```

## 最終確認

プロジェクトをビルドし、次の動作を確認します。

1. ゲーム画面の上部に敵とHPバーが表示される
2. 敵へ当たった弾だけが消える
3. 弾が当たるたびにHPバーが減る
4. 弾が外れた場合はHPが減らない
5. 10発当てると敵が消える
6. 敵が消れた場所に「Enemy defeated!」と表示される
7. 撃破後もプレイヤーの移動と弾の発射ができる

## 余裕がある人向けの追加課題

### 敵のHPを変更する

`Enemy.h`の`MaxHP`を変更し、倒すまでに必要な弾数が変わるか確認してみてください。

```cpp
static constexpr int32 MaxHP = 20;
```

### 1発のダメージを増やす

現在は、命中数をそのままダメージとして渡しています。

```cpp
m_enemy.damage(hitCount);
```

例えば次のようにすると、弾1発につき2ダメージになります。

```cpp
m_enemy.damage(hitCount * 2);
```

### 敵のHitBoxを表示する

デバッグ中だけ敵のHitBoxを半透明で描けるようにしてみてください。

見た目の円とHitBoxの大きさが一致しているか確認できると、当たり判定の調整がしやすくなります。

### 命中した瞬間だけ敵の色を変える

敵へ弾が当たったときに短い時間だけ白く表示すると、命中したことがさらに分かりやすくなります。

実装するには、次のような情報が必要になりそうです。

- 最後にダメージを受けてからの経過時間
- 現在点滅中かどうか

## まとめ

今回は、画面上部に敵を作り、プレイヤー弾との当たり判定を実装しました。

実装した内容は次の通りです。

- `Enemy`も`GameObject`を継承させた
- 敵へ円形HitBoxとHPを持たせた
- `GameObject::intersects()`で弾と敵の重なりを調べた
- 命中した弾を`remove_if()`で配列から削除した
- 同じフレームに命中した弾数をダメージとして利用した
- 現在HPに応じてHPバーの幅を変更した
- HPが0になると敵を描画せず、撃破メッセージを表示した

## 次回

次回は、敵を動かしたり、敵から弾を発射したりする予定です。

## 参考

- [Siv3D公式サイト](https://siv3d.github.io/ja-jp/)
