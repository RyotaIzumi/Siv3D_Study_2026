# 目次
https://qiita.com/Ryota123/private/ad363a4778e02b4035a2
***
<!-- Qiita投稿時に目次リンクを追加する -->

前回は、敵を作成し、プレイヤーの弾が敵へ当たるようにしました。

<!-- Part 4公開後、ここへリンクを追加する -->

今回は、敵を左右に移動させ、敵から弾幕を発射できるようにします。

さらに、敵弾がプレイヤーに一度でも当たると`GameOver`になり、`Rキー`でリスタートできるようにします。

使用するものは、Siv3D v0.6.16、Visual Studio 2026です。

## 今回作るもの

今回の完成条件は次の通りです。

- 敵がx軸方向に`sin`の動きで往復移動する
- 敵弾用の`EnemyBullet`クラスを作る
- 敵弾も`GameObject`を継承し、円形の`HitBox`を持つ
- 敵が一定時間ごとに弾幕を発射する
- サンプル弾幕として、放射状弾幕を作る
- 敵弾が画面外へ出たら削除する
- 敵弾がプレイヤーに当たったら`GameOver`にする
- `Rキー`でゲームを最初からやり直せるようにする

今回の実装では、弾幕を後から増やしやすいようにします。

具体的には、`Enemy`の中に次のような関数を用意します。

```cpp
void shoot();
void shootRadialBullets();
void updateBullets();
```

今回は`shootRadialBullets()`で放射状弾幕を作ります。

後から別の弾幕を追加したいときは、例えば次のような関数を増やせます。

```cpp
void shootAimingBullets();
void shootSpiralBullets();
void shootNWayBullets();
```

「弾幕ごとに関数を分ける」ことで、課題として別の弾幕を作るときにも変更する場所が分かりやすくなります。

## 今回の実装順

今回は、次の順番で実装します。

1. 敵を`sin`で左右に移動させる
2. `EnemyBullet`を作る
3. 敵が敵弾を持てるようにする
4. 敵弾を描画・移動・削除する
5. 放射状弾幕を作る
6. 敵弾とプレイヤーの当たり判定を作る
7. GameOverとRキーリスタートを作る

実装順と記事の流れを同じにしているので、上から順番に進めてください。

## 敵をsinで左右に移動させる

まずは、前回作った敵を左右に動かします。

敵の動きには`sin`を使います。

`sin`は、時間が進むと`-1`から`1`の間をなめらかに往復する値を作れます。

その値をx座標に使うことで、敵が左右へなめらかに移動します。

### Enemy.hに移動用の変数を追加する

`Enemy.h`へ、移動に使う定数と変数を追加します。

```diff_cpp:GameObject/Enemy.h
 private:
 	static constexpr double HitBoxRadius = 45.0;
 	static constexpr int32 MaxHP = 10;
+	static constexpr double MoveRange = 220.0;
+	static constexpr double MoveSpeed = 2.0;
 
+	Vec2 m_startPos;
+	double m_moveTime = 0.0;
 	int32 m_hp = MaxHP;
```

それぞれの役割は次の通りです。

- `MoveRange`：初期位置から左右にどれくらい動くか
- `MoveSpeed`：往復する速さ
- `m_startPos`：敵の最初の位置
- `m_moveTime`：移動に使う経過時間

`m_startPos`は、敵がどこを中心に往復するかを覚えておくために使います。

### コンストラクタで初期位置を保存する

`Enemy.cpp`のコンストラクタを変更し、受け取った初期位置を`m_startPos`へ保存します。

```diff_cpp:GameObject/Enemy.cpp
 Enemy::Enemy(const Vec2& pos)
 	: GameObject{ ObjectType::Enemy, pos,
-		std::make_shared<CircleHitBox>(pos, HitBoxRadius) } {
+		std::make_shared<CircleHitBox>(pos, HitBoxRadius) }
+	, m_startPos{ pos } {
 }
```

これで、敵の最初の位置を後から使えるようになりました。

## 課題1：敵をsinで左右に動かす

`Enemy::update()`に、敵の移動処理を書いてください。

やることは次の通りです。

1. 敵が倒されていたら何もせず終了する
2. `m_moveTime`に`Scene::DeltaTime()`を足す
3. `Math::Sin()`で左右移動用の値を作る
4. `setPos()`で敵の位置を更新する

途中まで書くと、次のようになります。

```cpp
void Enemy::update() {
	if (isDead()) {
		return;
	}

	m_moveTime += Scene::DeltaTime();

	const double moveX = /* ここでsinを使う */;
	setPos(m_startPos + Vec2{ moveX, 0.0 });
}
```

`setPos()`を使うと、見た目の座標だけでなく`HitBox`の位置も一緒に更新されます。

そのため、移動中の敵にもプレイヤー弾が正しく当たります。

<details>
<summary>ヒント</summary>

`Math::Sin()`の結果に`MoveRange`をかけると、移動幅を大きくできます。

```cpp
Math::Sin(m_moveTime * MoveSpeed) * MoveRange
```

</details>

<details>
<summary>解答例</summary>

```cpp
void Enemy::update() {
	if (isDead()) {
		return;
	}

	m_moveTime += Scene::DeltaTime();

	const double moveX = Math::Sin(m_moveTime * MoveSpeed) * MoveRange;
	setPos(m_startPos + Vec2{ moveX, 0.0 });
}
```

</details>

### 課題1の動作確認

ビルドして実行してください。

次の2点を確認できれば成功です。

- 敵が左右になめらかに往復する
- 移動中の敵に弾を当てると、HPバーが減る

もし敵の見た目は動いているのに弾が当たらない場合は、`m_pos`だけを直接変更していないか確認してください。

今回のように、位置とHitBoxを一緒に動かしたいときは`setPos()`を使います。

## EnemyBullet用のファイルを追加する

次に、敵から発射される弾を作ります。

`GameObject`フォルダへ、次の2ファイルを追加します。

- EnemyBullet.h
- EnemyBullet.cpp

敵弾もプレイヤー弾と同じように、ゲーム内に存在するオブジェクトです。

そのため、`GameObject`を継承し、円形の`HitBox`を持たせます。

## EnemyBullet.hを書く

`EnemyBullet.h`は次のように書きます。

```cpp:GameObject/EnemyBullet.h
#pragma once
#include "GameObject.h"

// 敵から発射される弾
class EnemyBullet : public GameObject {
private:
	static constexpr double HitBoxRadius = 8.0;

	Vec2 m_velocity;

public:
	EnemyBullet(const Vec2& pos, const Vec2& velocity);

	// 弾を速度の向きへ移動させる
	void update() override;

	// 敵弾を紫色の円として描く
	void draw() const override;

	// 弾が画面外に出たかを返す
	bool isOutsideScreen() const;
};
```

`m_velocity`は、弾の速度を表します。

プレイヤー弾は上方向にしか進まなかったので、速度は固定でした。

しかし敵弾は、放射状にいろいろな方向へ飛ばしたいです。

そのため、敵弾ごとに速度を持たせます。

```cpp
Vec2 m_velocity;
```

`Vec2`にはx方向とy方向の値を入れられるため、「右下へ進む」「左へ進む」「斜め上へ進む」といった方向と速さをまとめて扱えます。

## EnemyBullet.cppへ穴埋めコードを書く

`EnemyBullet.cpp`へ、次の穴埋め用コードを書きます。

```cpp:GameObject/EnemyBullet.cpp
#include "EnemyBullet.h"

// 初期位置・速度・敵弾用の円形HitBoxを設定する
EnemyBullet::EnemyBullet(const Vec2& pos, const Vec2& velocity)
	: GameObject{ ObjectType::EnemyBullet, pos,
		std::make_shared<CircleHitBox>(pos, HitBoxRadius) }
	, m_velocity{ velocity } {
}

// 課題2で、速度を使って弾を移動させる
void EnemyBullet::update() {
}

// 課題2で、敵弾を描く
void EnemyBullet::draw() const {
}

// 課題3で、弾が画面外へ出たか調べる
bool EnemyBullet::isOutsideScreen() const {
	return false;
}
```

この時点では、敵弾はまだ作られていないため、ゲーム画面には変化がありません。

ただし、次に`Enemy`から敵弾を使う準備ができます。

## Enemyが敵弾を持てるようにする

`Enemy.h`で`EnemyBullet.h`を読み込みます。

```diff_cpp:GameObject/Enemy.h
 #pragma once
 #include "GameObject.h"
+#include "EnemyBullet.h"
```

次に、敵弾を保存する配列と、弾幕用の関数を追加します。

```diff_cpp:GameObject/Enemy.h
 private:
 	static constexpr double HitBoxRadius = 45.0;
 	static constexpr int32 MaxHP = 10;
 	static constexpr double MoveRange = 220.0;
 	static constexpr double MoveSpeed = 2.0;
+	static constexpr double ShotInterval = 1.0;
 
 	Vec2 m_startPos;
 	double m_moveTime = 0.0;
+	double m_shotTimer = 0.0;
+	int32 m_shotCount = 0;
 	int32 m_hp = MaxHP;
+	Array<EnemyBullet> m_bullets;
+
+	// 一定時間ごとに弾幕を発射する
+	void shoot();
+	// サンプルの弾幕として、敵を中心に放射状へ弾を発射する
+	void shootRadialBullets();
+	// 発射済みの敵弾を移動し、画面外の弾を削除する
+	void updateBullets();
```

それぞれの役割は次の通りです。

- `ShotInterval`：何秒ごとに弾幕を発射するか
- `m_shotTimer`：前回の発射から何秒経ったか
- `m_shotCount`：何回弾幕を撃ったか
- `m_bullets`：画面に存在している敵弾の配列
- `shoot()`：発射タイミングを管理する
- `shootRadialBullets()`：放射状弾幕を発射する
- `updateBullets()`：敵弾を更新し、画面外の弾を削除する

`shoot()`と`shootRadialBullets()`を分けているのが今回のポイントです。

`shoot()`は「いつ撃つか」を管理します。

`shootRadialBullets()`は「どんな弾幕を撃つか」を管理します。

このように分けておくと、後から別の弾幕を追加しやすくなります。

## Enemyのupdate()から弾幕処理を呼ぶ

`Enemy::update()`へ、弾幕関連の呼び出しを追加します。

```diff_cpp:GameObject/Enemy.cpp
 void Enemy::update() {
 	if (isDead()) {
+		m_bullets.clear();
 		return;
 	}
 
 	m_moveTime += Scene::DeltaTime();
 
 	const double moveX = Math::Sin(m_moveTime * MoveSpeed) * MoveRange;
 	setPos(m_startPos + Vec2{ moveX, 0.0 });
+
+	shoot();
+	updateBullets();
 }
```

敵が倒されているときは、敵弾も消すようにしています。

```cpp
m_bullets.clear();
```

これにより、敵を倒した後に画面へ敵弾だけが残り続けることを防げます。

## 課題2：敵弾を描画して移動させる

まずは、敵弾1つが移動できるようにします。

`EnemyBullet::update()`を完成させてください。

やることは、現在位置に速度分の移動量を足すだけです。

ただし、フレームレートによって速さが変わらないように、`Scene::DeltaTime()`をかけます。

```cpp
void EnemyBullet::update() {
	setPos(/* 現在位置 + 速度 * 経過時間 */);
}
```

次に、`EnemyBullet::draw()`で敵弾を描画してください。

今回は紫色の円として描きます。

```cpp
void EnemyBullet::draw() const {
	// Circleを使って描く
}
```

<details>
<summary>ヒント</summary>

現在位置は`m_pos`、速度は`m_velocity`です。

円の描画は、プレイヤー弾の描画とかなり似ています。

</details>

<details>
<summary>解答例</summary>

```cpp
void EnemyBullet::update() {
	setPos(m_pos + m_velocity * Scene::DeltaTime());
}

void EnemyBullet::draw() const {
	Circle{ m_pos, HitBoxRadius }.draw(ColorF{ 0.85, 0.25, 1.0 });
}
```

</details>

この段階では、まだ敵弾を作っていないため、画面には敵弾が出ません。

ただし、敵弾自身の移動と描画の準備はできました。

## 課題3：画面外に出た敵弾を削除できるようにする

弾幕では、弾がどんどん増えます。

画面外に出た弾を削除しないと、見えない弾が配列に残り続けます。

そこで、`EnemyBullet::isOutsideScreen()`を完成させます。

今回は、弾が画面の上下左右のどこかから外に出たら`true`を返します。

```cpp
bool EnemyBullet::isOutsideScreen() const {
	const double margin = HitBoxRadius;

	return /* 画面外ならtrue */;
}
```

`margin`は、弾の半径ぶんだけ余裕を持たせるための値です。

弾の中心が画面端を少し超えただけで削除すると、弾が半分残っているのに消えるように見えることがあります。

そのため、弾全体が画面外へ出たあたりで削除するようにします。

<details>
<summary>ヒント</summary>

調べる条件は4つです。

- 左へ出たか
- 右へ出たか
- 上へ出たか
- 下へ出たか

横方向は`m_pos.x`、縦方向は`m_pos.y`を使います。

</details>

<details>
<summary>解答例</summary>

```cpp
bool EnemyBullet::isOutsideScreen() const {
	const double margin = HitBoxRadius;

	return (m_pos.x < -margin)
		|| (Scene::Width() + margin < m_pos.x)
		|| (m_pos.y < -margin)
		|| (Scene::Height() + margin < m_pos.y);
}
```

</details>

## Enemy側で敵弾を更新・描画する

敵弾を実際にゲームで使うため、`Enemy`側へ処理を追加します。

まず、`Enemy::updateBullets()`を追加します。

```cpp:GameObject/Enemy.cpp
// すべての敵弾を更新し、画面外へ出た弾を配列から取り除く
void Enemy::updateBullets() {
	for (auto& bullet : m_bullets) {
		bullet.update();
	}

	m_bullets.remove_if([](const EnemyBullet& bullet) {
		return bullet.isOutsideScreen();
	});
}
```

プレイヤー弾のときと同じように、すべての弾を更新してから、画面外の弾を削除しています。

次に、`Enemy::draw()`の最初で敵弾を描画します。

```diff_cpp:GameObject/Enemy.cpp
 void Enemy::draw() const {
+	for (const auto& bullet : m_bullets) {
+		bullet.draw();
+	}
+
 	if (isDead()) {
 		return;
 	}
```

敵弾を先に描いてから敵本体を描くことで、敵本体が弾より手前に見えます。

## 敵が一定時間ごとに弾を撃つようにする

次に、敵が一定時間ごとに弾幕を撃つ処理を作ります。

`Enemy::shoot()`を追加します。

```cpp:GameObject/Enemy.cpp
// 発射間隔を管理し、時間が来たら弾幕を発射する
void Enemy::shoot() {
	m_shotTimer += Scene::DeltaTime();

	if (ShotInterval <= m_shotTimer) {
		shootRadialBullets();
		m_shotTimer -= ShotInterval;
	}
}
```

`m_shotTimer`へ毎フレーム時間を足し、`ShotInterval`秒以上になったら弾幕を発射します。

発射した後は、`m_shotTimer`から`ShotInterval`を引きます。

```cpp
m_shotTimer -= ShotInterval;
```

`m_shotTimer = 0.0;`にしても動きますが、少し時間が余ったときのズレを残せるため、今回は引き算にしています。

## 課題4：放射状弾幕を作る

今回のサンプル弾幕として、敵を中心に全方向へ弾を出す放射状弾幕を作ります。

`Enemy::shootRadialBullets()`を完成させてください。

考え方は次の通りです。

1. 発射する弾の数を決める
2. 弾の速さを決める
3. `for`文で弾の数だけ繰り返す
4. 角度を少しずつ変える
5. 角度から方向ベクトルを作る
6. `m_bullets.emplace_back()`で敵弾を追加する

穴埋め用コードです。

```cpp
void Enemy::shootRadialBullets() {
	const int32 bulletCount = 16;
	const double bulletSpeed = 150.0;
	const double angleOffset = m_shotCount * 0.15;

	for (int32 i = 0; i < bulletCount; ++i) {
		const double angle = /* 角度を計算する */;
		const Vec2 direction{ /* x方向 */, /* y方向 */ };

		m_bullets.emplace_back(m_pos, direction * bulletSpeed);
	}

	++m_shotCount;
}
```

`Math::TwoPi`は、円1周ぶんの角度です。

放射状に16発撃つ場合、1周を16等分します。

```cpp
Math::TwoPi * i / bulletCount
```

また、`m_shotCount * 0.15`を足すことで、発射するたびに角度が少しずつずれます。

これにより、弾幕が少し回転しているように見えます。

<details>
<summary>ヒント</summary>

角度から方向を作るには、x方向に`Math::Cos(angle)`、y方向に`Math::Sin(angle)`を使います。

```cpp
const Vec2 direction{ Math::Cos(angle), Math::Sin(angle) };
```

</details>

<details>
<summary>解答例</summary>

```cpp
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
```

</details>

### 課題4の動作確認

ビルドして実行してください。

次の3点を確認できれば成功です。

- 敵から一定時間ごとに紫色の弾が発射される
- 弾が敵を中心に放射状へ広がる
- 時間が経つと、弾幕の角度が少しずつ変わる

弾が出ない場合は、`Enemy::update()`から`shoot()`と`updateBullets()`を呼んでいるか確認してください。

弾が動かない場合は、`EnemyBullet::update()`で`setPos()`を呼んでいるか確認してください。

## 敵弾とプレイヤーの当たり判定を作る

敵弾が出るようになったので、次はプレイヤーとの当たり判定を作ります。

`Enemy.h`の`public`へ、次の関数を追加します。

```diff_cpp:GameObject/Enemy.h
 public:
 	Enemy(const Vec2& pos);
 
 	// 敵を左右に往復移動させる
 	void update() override;
 
 	// 敵本体と現在のHPを表すHPバーを描く
 	void draw() const override;
+
+	// 指定したオブジェクトに当たった敵弾を削除し、命中した数を返す
+	int32 countBulletHits(const GameObject& target);
```

この関数は、前回作った`Player::countBulletHits()`とほとんど同じ考え方です。

「対象に当たった弾を消し、何発当たったかを返す」関数です。

今回は、敵弾がプレイヤーに当たったかを調べるために使います。

## 課題5：プレイヤーに当たった敵弾を数える

`Enemy::countBulletHits()`を完成させてください。

条件は次の通りです。

1. 命中数を保存する`hitCount`を`0`で用意する
2. `m_bullets.remove_if()`ですべての敵弾を調べる
3. `bullet.intersects(target)`が`true`なら命中数を1増やす
4. 命中した弾は削除するため`true`を返す
5. 命中していない弾は残すため`false`を返す
6. 最後に`hitCount`を返す

```cpp
int32 Enemy::countBulletHits(const GameObject& target) {
	int32 hitCount = 0;

	m_bullets.remove_if([&](const EnemyBullet& bullet) {
		// ここに当たり判定を書く
	});

	return hitCount;
}
```

<details>
<summary>ヒント</summary>

前回作った`Player::countBulletHits()`を参考にできます。

敵弾も`GameObject`を継承しているため、`intersects()`を使えます。

</details>

<details>
<summary>解答例</summary>

```cpp
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
```

</details>

### 課題5の動作確認

この段階では、関数を作っただけなので、まだ`GameOver`にはなりません。

次の章で`InGame`からこの関数を呼び出します。

ただし、ここまででビルドできるかは確認してください。

## GameOverとRキーリスタートを作る

最後に、敵弾がプレイヤーへ当たったら`GameOver`にします。

さらに、`Rキー`で最初からやり直せるようにします。

### Scene.hにゲーム状態を追加する

`Scene.h`の`InGame`へ、GameOver中かどうかを保存する変数を追加します。

```diff_cpp:Scene/Scene.h
 class InGame : public SceneManager<SceneType>::Scene {
 private:
 	Player m_player;
 	Enemy m_enemy;
+	bool m_isGameOver = false;
+
+	// プレイヤー・敵・ゲーム状態を初期状態に戻す
+	void resetGame();
```

`m_isGameOver`が`true`ならゲームオーバー中、`false`なら通常プレイ中です。

`resetGame()`は、プレイヤーや敵を作り直して、ゲームを最初から始めるための関数です。

### InGame.cppにresetGame()を追加する

`InGame.cpp`へ、`resetGame()`を追加します。

```cpp:Scene/InGame.cpp
// プレイヤーと敵を作り直し、ゲームオーバー状態を解除する
void InGame::resetGame() {
	m_player = Player{ Scene::Center() };
	m_enemy = Enemy{ Vec2{ Scene::Center().x, 150.0 } };
	m_isGameOver = false;
}
```

`m_player`と`m_enemy`を作り直すことで、プレイヤーの位置、敵の位置、敵のHP、敵弾の配列が初期状態に戻ります。

## 課題6：被弾したらGameOverにする

`InGame::update()`で、敵弾がプレイヤーに当たったかを調べます。

プレイヤーが被弾したら`m_isGameOver`を`true`にしてください。

```cpp
// 生存中の敵に当たった弾を数え、1発につき1ダメージを与える
if (!m_enemy.isDead()) {
	const int32 hitCount = m_player.countBulletHits(m_enemy);
	m_enemy.damage(hitCount);

	// ここに敵弾とプレイヤーの当たり判定を書く
}
```

`Enemy::countBulletHits()`は、命中した敵弾の数を返します。

今回は、一度でも当たったらGameOverなので、返ってきた値が`0`より大きいかを調べます。

<details>
<summary>解答例</summary>

```cpp
if (0 < m_enemy.countBulletHits(m_player)) {
	m_isGameOver = true;
}
```

</details>

このままだと、GameOverになっても次のフレームでゲームが更新され続けます。

そこで、`InGame::update()`の最初にGameOver中の処理を追加します。

```cpp
// GameOver中はゲームを止め、Rキーで最初からやり直せるようにする
if (m_isGameOver) {
	if (KeyR.down()) {
		resetGame();
	}

	return;
}
```

`return;`で関数を終了することで、GameOver中はプレイヤーや敵の更新を止めています。

### 課題6の動作確認

ビルドして実行し、敵弾に当たってください。

次の3点を確認できれば成功です。

- 敵弾に当たると、ゲームの動きが止まる
- プレイヤーを動かそうとしても動かない
- Rキーを押すと、敵・プレイヤー・弾幕が初期状態に戻る

この時点では、まだ画面に`Game Over`の文字は出ません。

次に、GameOver中の表示を追加します。

## GameOverの文字を表示する

`InGame::draw()`へ、GameOver中だけメッセージを表示する処理を追加します。

```cpp:Scene/InGame.cpp
// GameOver中は画面中央にメッセージを表示する
if (m_isGameOver) {
	FontAsset(U"Title")(U"Game Over")
		.drawAt(Scene::Center().x, Scene::Center().y - 30, Palette::Black);
	FontAsset(U"Guide")(U"Press R to Restart")
		.drawAt(Scene::Center().x, Scene::Center().y + 30, Palette::Black);
}
```

`FontAsset(U"Title")`は、第1回で登録したタイトル用のフォントです。

`FontAsset(U"Guide")`は、操作説明などに使っている小さめのフォントです。

### 最終動作確認

ここまで書いたら、ビルドして実行してください。

次の動作を確認できれば完成です。

1. 敵が左右へなめらかに往復する
2. 敵から放射状に紫色の弾が発射される
3. 敵弾が画面外へ出ると削除される
4. プレイヤーが敵弾に当たると`Game Over`が表示される
5. GameOver中はゲームの更新が止まる
6. Rキーを押すと最初からリスタートできる
7. プレイヤー弾を敵に当てると、前回と同じように敵のHPが減る

## EnemyBullet.cppの完成コード

完成した`EnemyBullet.cpp`は次のようになります。

```cpp:GameObject/EnemyBullet.cpp
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
```

## Enemy.hの完成コード

完成した`Enemy.h`は次のようになります。

```cpp:GameObject/Enemy.h
#pragma once
#include "GameObject.h"
#include "EnemyBullet.h"

// 画面上部に配置され、プレイヤーの弾でダメージを受ける敵
class Enemy : public GameObject {
private:
	static constexpr double HitBoxRadius = 45.0;
	static constexpr int32 MaxHP = 10;
	static constexpr double MoveRange = 220.0;
	static constexpr double MoveSpeed = 2.0;
	static constexpr double ShotInterval = 1.0;

	Vec2 m_startPos;
	double m_moveTime = 0.0;
	double m_shotTimer = 0.0;
	int32 m_shotCount = 0;
	int32 m_hp = MaxHP;
	Array<EnemyBullet> m_bullets;

	// 一定時間ごとに弾幕を発射する
	void shoot();
	// サンプルの弾幕として、敵を中心に放射状へ弾を発射する
	void shootRadialBullets();
	// 発射済みの敵弾を移動し、画面外の弾を削除する
	void updateBullets();

public:
	Enemy(const Vec2& pos);

	// 敵を左右に往復移動させる
	void update() override;

	// 敵本体と現在のHPを表すHPバーを描く
	void draw() const override;

	// 指定したオブジェクトに当たった敵弾を削除し、命中した数を返す
	int32 countBulletHits(const GameObject& target);

	// 指定された値だけHPを減らす
	void damage(int32 amount);

	// HPが0になったかを返す
	bool isDead() const;
};
```

## Enemy.cppの完成コード

完成した`Enemy.cpp`は次のようになります。

```cpp:GameObject/Enemy.cpp
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
```

## Scene.hの完成コード

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
	bool m_isGameOver = false;

	// プレイヤー・敵・ゲーム状態を初期状態に戻す
	void resetGame();

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
	, m_enemy{ Vec2{ Scene::Center().x, 150.0 } } {
}

// プレイヤーと敵を作り直し、ゲームオーバー状態を解除する
void InGame::resetGame() {
	m_player = Player{ Scene::Center() };
	m_enemy = Enemy{ Vec2{ Scene::Center().x, 150.0 } };
	m_isGameOver = false;
}

// インゲームシーンの更新処理
void InGame::update() {
	// GameOver中はゲームを止め、Rキーで最初からやり直せるようにする
	if (m_isGameOver) {
		if (KeyR.down()) {
			resetGame();
		}

		return;
	}

	// プレイヤーのキー入力と移動を更新する
	m_player.update();
	m_enemy.update();

	// 生存中の敵に当たった弾を数え、1発につき1ダメージを与える
	if (!m_enemy.isDead()) {
		const int32 hitCount = m_player.countBulletHits(m_enemy);
		m_enemy.damage(hitCount);

		// 敵弾が1発でもプレイヤーに当たったらGameOverにする
		if (0 < m_enemy.countBulletHits(m_player)) {
			m_isGameOver = true;
		}
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

	// GameOver中は画面中央にメッセージを表示する
	if (m_isGameOver) {
		FontAsset(U"Title")(U"Game Over")
			.drawAt(Scene::Center().x, Scene::Center().y - 30, Palette::Black);
		FontAsset(U"Guide")(U"Press R to Restart")
			.drawAt(Scene::Center().x, Scene::Center().y + 30, Palette::Black);
	}

	// 敵のHPが0になったら撃破メッセージを表示する
	if (m_enemy.isDead()) {
		FontAsset(U"Guide")(U"Enemy defeated!")
			.drawAt(Scene::Center().x, 100, Palette::Black);
	}
}
```

## Visual Studioのプロジェクトへファイルを追加する

新しく作った次の2ファイルは、Visual Studioのプロジェクトへ追加してください。

- `GameObject/EnemyBullet.h`
- `GameObject/EnemyBullet.cpp`

追加されていない場合、ファイルを書いていてもビルド対象にならず、リンクエラーになることがあります。

## 余裕がある人向けの追加課題

### 放射状弾幕の弾数を変える

`Enemy::shootRadialBullets()`の`bulletCount`を変更してみましょう。

```cpp
const int32 bulletCount = 24;
```

弾数を増やすと密度が上がり、避けるのが難しくなります。

### 弾幕の速さを変える

`bulletSpeed`を変更して、難易度がどう変わるか確認してみましょう。

```cpp
const double bulletSpeed = 220.0;
```

### 弾幕の発射間隔を変える

`Enemy.h`の`ShotInterval`を変更すると、弾幕を撃つ間隔が変わります。

```cpp
static constexpr double ShotInterval = 0.5;
```

値を小さくすると、より短い間隔で弾幕を撃ちます。

### 回転しない放射状弾幕にする

現在は、次の行で発射するたびに角度を少しずらしています。

```cpp
const double angleOffset = m_shotCount * 0.15;
```

これを`0.0`にすると、毎回同じ角度へ弾を撃ちます。

```cpp
const double angleOffset = 0.0;
```

### 自分だけの弾幕を追加する

新しく関数を作り、`shoot()`から呼んでみましょう。

```cpp
void Enemy::shootNWayBullets() {
	// ここに自分の弾幕を書く
}
```

例えば、下方向だけに広がる弾幕、プレイヤー方向を狙う弾幕、時間とともに回転する弾幕などが考えられます。

今回の構成では、弾幕の中身は`EnemyBullet`を`m_bullets`へ追加するだけです。

```cpp
m_bullets.emplace_back(発射位置, 速度);
```

この形さえ守れば、いろいろな弾幕を作れます。

## まとめ

今回は、敵の移動と敵弾の弾幕、そしてプレイヤーの被弾判定を実装しました。

実装した内容は次の通りです。

- `Math::Sin()`を使って敵を左右に往復移動させた
- `EnemyBullet`クラスを追加した
- 敵弾にも`GameObject`と`HitBox`を持たせた
- 敵が`Array<EnemyBullet>`で複数の弾を管理するようにした
- `shoot()`と`shootRadialBullets()`を分け、弾幕を追加しやすい構成にした
- 放射状弾幕を作った
- 画面外に出た敵弾を削除した
- 敵弾がプレイヤーに当たったら`GameOver`にした
- `Rキー`でリスタートできるようにした

## 次回

次回は、ゲームとしてより遊びやすくなるように、演出やUI、ゲームクリアなどを追加していく予定です。

## 参考

- [Siv3D公式サイト](https://siv3d.github.io/ja-jp/)
- [Siv3D 図形と交差判定](https://siv3d.github.io/ja-jp/tutorial3/geometry2d/)
