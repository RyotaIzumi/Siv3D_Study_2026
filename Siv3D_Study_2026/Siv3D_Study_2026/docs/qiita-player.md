<!--
タイトル案:
Siv3Dでシューティングゲームを作る - GameObjectを継承したPlayerを動かす

タグ案:
C++, Siv3D, ゲーム制作, 初心者, シューティングゲーム
-->

今回は、上下左右に動かせるプレイヤーを作ります。

最終的にはボス1体と戦う簡単なシューティングゲームにする予定です。そのため、ただ画面に図形を出して動かすだけではなく、後から弾や敵との当たり判定を追加しやすい形にしておきます。

具体的には、ゲーム内のオブジェクトが継承する`GameObject`、当たり判定を表す`HitBox`、そしてそれらを使った`Player`を作成します。

使用するものは、Siv3D v0.6.16、Visual Studio 2026です。

この記事では途中に課題を用意しています。勉強会で使う場合は、すぐ完成コードを見ず、まず5分ほど自分で考えてみてください。全部解けなくても問題ありません。手を動かして詰まった場所が分かるだけでも、完成コードの見え方がかなり変わります。

前回までに、`Title`シーンと`InGame`シーンが作られているところから始めます。

## 今回作るもの

今回の完成条件は以下です。

- 矢印キー、またはWASDキーで上下左右に移動できる
- 斜め移動だけ速くならない
- ウィンドウの外へ出ない
- `Player`が`GameObject`を継承している
- `Player`が円形の`HitBox`を持っている

プレイヤーの見た目には、画像ではなく三角形と円を使います。画像素材を用意しなくても動作確認でき、移動処理に集中できるためです。

> 実行画面の画像をここに追加予定

## ファイルの準備

`Main.cpp`と同じ階層に`GameObject`フォルダを作り、次の5ファイルを追加します。

```text
GameObject/
├── GameObject.h
├── HitBox.h
├── HitBox.cpp
├── Player.h
└── Player.cpp
```

Visual Studioでは、フォルダまたはプロジェクトを右クリックして「追加」からファイルを作成できます。

日本語コメントを含む新規ファイルで文字化けの警告が出る場合は、プロジェクトの「C/C++ → コマンドライン → 追加のオプション」に`/utf-8`を追加してください。

## HitBoxを作る

まず、当たり判定を表す`HitBox`を作ります。

プレイヤーに直接`Circle`を持たせるだけでも、今の段階では当たり判定を作れます。しかし、この先は弾や敵など、円以外の当たり判定も登場するかもしれません。

そこで、どの形でも共通して「移動する」「相手と重なっているか調べる」といった操作ができるよう、親クラスを用意します。

### 課題1：共通する機能を考える

円形と長方形の当たり判定を同じように扱うには、どのような関数が必要でしょうか。実装を見る前に、`HitBox`のメンバ関数を2つ以上考えてみてください。

<details>
<summary>ヒント</summary>

ゲームオブジェクトが移動したとき、当たり判定も一緒に移動する必要があります。また、弾が敵に当たったかを調べる機能も必要です。

</details>

今回は次の機能を用意します。

- `intersects()`：別のHitBoxと重なっているか調べる
- `setPos()`：HitBoxを指定した座標へ移動する
- `getCenterPos()`：中心座標を取得する
- `getCircle()`、`getRect()`：HitBoxが持つ図形を取得する

`HitBox.h`を次のようにします。

```cpp:GameObject/HitBox.h
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

	bool intersects(const HitBox& other) const override;
	void setPos(const Vec2& pos) override;
	Vec2 getCenterPos() const override;
	const Circle* getCircle() const override;
};

// 長方形の当たり判定を表すクラス
class RectHitBox : public HitBox {
private:
	RectF m_rect;

public:
	RectHitBox(const Vec2& center, const SizeF& size);

	bool intersects(const HitBox& other) const override;
	void setPos(const Vec2& pos) override;
	Vec2 getCenterPos() const override;
	const RectF* getRect() const override;
};
```

`virtual`と`= 0`が付いた関数は純粋仮想関数です。`HitBox`を継承したクラスに「この関数は必ず実装してください」と約束させるために使います。

`getCircle()`と`getRect()`は、自分が持っていない図形なら`nullptr`を返します。例えば`CircleHitBox`は`getCircle()`を上書きしますが、`getRect()`は上書きしないため`nullptr`を返します。

次に`HitBox.cpp`を作ります。

```cpp:GameObject/HitBox.cpp
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

// 円の中心を指定した座標へ移動する
void CircleHitBox::setPos(const Vec2& pos) {
	m_circle.setCenter(pos);
}

// 円の中心座標を返す
Vec2 CircleHitBox::getCenterPos() const {
	return m_circle.center;
}

// このHitBoxが持つ円を返す
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

// 長方形の中心を指定した座標へ移動する
void RectHitBox::setPos(const Vec2& pos) {
	m_rect.setCenter(pos);
}

// 長方形の中心座標を返す
Vec2 RectHitBox::getCenterPos() const {
	return m_rect.center();
}

// このHitBoxが持つ長方形を返す
const RectF* RectHitBox::getRect() const {
	return &m_rect;
}
```

Siv3Dの`intersects()`を使うと、円と円、円と長方形などの交差判定ができます。今回は仕組みだけを用意し、実際に弾や敵との判定を行うのは今後に回します。

## GameObjectを作る

次に、プレイヤーや敵、弾が共通して継承する`GameObject`を作ります。

```cpp:GameObject/GameObject.h
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
	GameObject(ObjectType type, const Vec2& pos,
		std::shared_ptr<HitBox> hitBox)
		: m_type{ type }
		, m_pos{ pos }
		, m_hitBox{ std::move(hitBox) } {
	}

	virtual ~GameObject() = default;

	// オブジェクトを1フレーム分更新する
	virtual void update() = 0;

	// オブジェクトを画面に描く
	virtual void draw() const = 0;

	// 現在の中心座標を返す
	const Vec2& getPos() const {
		return m_pos;
	}

	// PlayerやEnemyなど、このオブジェクトの種類を返す
	ObjectType getType() const {
		return m_type;
	}

	// 攻撃判定などから利用できるようHitBoxを返す
	const HitBox& getHitBox() const {
		return *m_hitBox;
	}

	// このオブジェクトと相手のHitBoxが重なっているかを調べる
	bool intersects(const GameObject& other) const {
		return m_hitBox->intersects(other.getHitBox());
	}
};
```

`m_pos`と`m_hitBox`は`protected`にしています。`private`と違い、`GameObject`を継承した`Player`からは使えます。

座標を直接変更せず`setPos()`を通すことで、オブジェクトの座標とHitBoxの座標を必ず一緒に更新できます。ここが別々になると、「見た目では当たっているのに判定されない」という、ゲーム制作ではおなじみの少々つらいバグが生まれます。

`ObjectType`は、将来当たり判定が起きたときに、相手が敵なのか敵弾なのかを見分けるために使う予定です。

### 課題2：継承する理由を考える

`Player`、`Enemy`、`Bullet`に、それぞれ別々に座標とHitBoxを持たせることもできます。それでも動くのに、なぜ`GameObject`へまとめるのでしょうか。

<details>
<summary>回答例</summary>

全オブジェクトを同じ型として管理しやすくなり、共通処理を何度も書かずに済むためです。例えば、将来`Array<std::shared_ptr<GameObject>>`へ敵や弾をまとめれば、同じループで`update()`や`draw()`を呼び出せます。

</details>

## Playerを作る

いよいよプレイヤーです。まず`Player.h`を作ります。

```cpp:GameObject/Player.h
#pragma once
#include "GameObject.h"

// キー操作で上下左右に移動するプレイヤー
class Player : public GameObject {
private:
	static constexpr double MoveSpeed = 300.0;
	static constexpr double HitBoxRadius = 18.0;

	// 押されているキーから移動する方向を作る
	Vec2 getMoveDirection() const;

	// プレイヤーの中心が画面外へ出ない位置に直す
	void keepInsideScreen();

public:
	Player(const Vec2& pos);

	// キー入力を読み取り、プレイヤーを移動する
	void update() override;

	// プレイヤーを簡単な宇宙船の形で描く
	void draw() const override;
};
```

`class Player : public GameObject`の部分が、`Player`は`GameObject`を継承する、という意味です。

移動速度とHitBoxの半径には`static constexpr`で名前を付けています。コード中に`300.0`や`18.0`が何度も直接現れるより、何の値なのかが分かりやすく、後から調整もしやすくなります。

### 課題3：キー入力から移動方向を作る

次の条件を満たす`getMoveDirection()`を考えてみてください。

- 左キーまたはAキーで、x方向を`-1.0`する
- 右キーまたはDキーで、x方向を`+1.0`する
- 上キーまたはWキーで、y方向を`-1.0`する
- 下キーまたはSキーで、y方向を`+1.0`する
- 複数のキーを同時に押せる

関数の最初は次の状態です。

```cpp
Vec2 Player::getMoveDirection() const {
	Vec2 direction{ 0.0, 0.0 };

	// ここへ入力処理を書く

	return direction;
}
```

<details>
<summary>ヒント</summary>

キーを押している間は`KeyLeft.pressed()`のように書けます。「または」は`||`です。

</details>

<details>
<summary>解答例</summary>

```cpp
if (KeyLeft.pressed() || KeyA.pressed()) {
	direction.x -= 1.0;
}
if (KeyRight.pressed() || KeyD.pressed()) {
	direction.x += 1.0;
}
if (KeyUp.pressed() || KeyW.pressed()) {
	direction.y -= 1.0;
}
if (KeyDown.pressed() || KeyS.pressed()) {
	direction.y += 1.0;
}
```

`else if`にすると、例えば左と上を同時に判定できなくなります。斜め移動を可能にするため、今回はそれぞれ独立した`if`を使います。

</details>

### 課題4：斜め移動が速くなる理由

左右だけに移動するときの方向は`(1, 0)`で、長さは`1`です。一方、右下へ移動するときは`(1, 1)`になります。

`(1, 1)`の長さはいくつになるでしょうか。また、縦横と斜めの速さを同じにするにはどうすればよいでしょうか。

<details>
<summary>解答</summary>

三平方の定理から、`(1, 1)`の長さは`√2`、約`1.414`です。そのまま速度を掛けると、斜め方向だけ約1.414倍速くなります。

`direction.normalize()`を使い、方向ベクトルの長さを`1`へそろえることで解決できます。ただし長さ`0`のベクトルは正規化しないよう、先に`isZero()`で確認します。

</details>

### 課題5：update()を書いてプレイヤーを移動させる

`getMoveDirection()`で移動方向を作れるようになったので、次は実際に座標を変更します。

次の条件を満たす`update()`を書いてみてください。

1. `getMoveDirection()`を呼び、結果を`direction`へ代入する
2. `direction`がゼロベクトルでない場合だけ正規化する
3. `方向 × 速さ × 前フレームからの経過時間`で移動量を求める
4. `setPos()`で新しい座標を設定する
5. 最後に`keepInsideScreen()`を呼ぶ

```cpp
void Player::update() {
	// ここへ処理を書く
}
```

ここでは、まず自分でコンパイルが通るところまで書いてみましょう。動きが少しおかしくても構いません。コンパイルエラーと実行時の動作は、別々に確認すると原因を探しやすいです。

<details>
<summary>ヒント1：移動量の計算</summary>

1秒間の移動速度は`MoveSpeed`、前フレームからの経過時間は`Scene::DeltaTime()`で取得できます。

```cpp
direction * MoveSpeed * Scene::DeltaTime()
```

この移動量を現在位置の`m_pos`へ足します。

</details>

<details>
<summary>解答例</summary>

```cpp
void Player::update() {
	Vec2 direction = getMoveDirection();

	if (not direction.isZero()) {
		direction.normalize();
	}

	setPos(m_pos + direction * MoveSpeed * Scene::DeltaTime());
	keepInsideScreen();
}
```

</details>

移動量には`Scene::DeltaTime()`を掛けています。これは前のフレームから経過した秒数です。掛けておくことで、PCの性能やフレームレートが変わっても、1秒間に進む距離をほぼ同じにできます。

### 課題6：keepInsideScreen()を書いて画面外への移動を防ぐ

ここは今回のメイン課題の一つです。完成コードを見る前に、関数を丸ごと書いてみてください。

画面の幅が800、プレイヤーの半径が18だとします。プレイヤー全体が画面内に残るには、中心のx座標を`18`から`800 - 18`までに収める必要があります。

y座標についても同様です。実際の画面サイズは、次の関数で取得できます。

```cpp
Scene::Width()  // 画面の幅
Scene::Height() // 画面の高さ
```

また、`Clamp(値, 最小値, 最大値)`を使うと、値を指定範囲内へ収められます。

次の条件を満たす`keepInsideScreen()`を書いてください。

- x座標の最小値は`HitBoxRadius`
- x座標の最大値は`Scene::Width() - HitBoxRadius`
- y座標の最小値は`HitBoxRadius`
- y座標の最大値は`Scene::Height() - HitBoxRadius`
- 最後に`setPos()`を使い、補正後のx座標とy座標を設定する

```cpp
void Player::keepInsideScreen() {
	// 補正後のx座標を求める

	// 補正後のy座標を求める

	// プレイヤーとHitBoxの座標を更新する
}
```

<details>
<summary>ヒント1：x座標だけ書いてみる</summary>

```cpp
const double x = Clamp(
	m_pos.x,
	HitBoxRadius,
	Scene::Width() - HitBoxRadius);
```

y座標では、`m_pos.x`と`Scene::Width()`を何へ置き換えればよいか考えてみてください。

</details>

<details>
<summary>ヒント2：座標を反映する</summary>

求めた`x`と`y`から`Vec2{ x, y }`を作り、`setPos()`へ渡します。

</details>

<details>
<summary>解答例</summary>

```cpp
void Player::keepInsideScreen() {
	const double x = Clamp(
		m_pos.x,
		HitBoxRadius,
		Scene::Width() - HitBoxRadius);

	const double y = Clamp(
		m_pos.y,
		HitBoxRadius,
		Scene::Height() - HitBoxRadius);

	setPos(Vec2{ x, y });
}
```

</details>

中心座標を`0`まで許すと、円の左半分や上半分が画面外へ出てしまいます。そのため、画面サイズだけでなくプレイヤーの半径も考慮しています。

また、`Scene::DeltaTime()`が一時的に大きくなり、一度の更新で画面端を飛び越えても、移動後に`Clamp()`を使うため画面内へ戻せます。

#### 発展課題：Clamp()を使わずに書く

余裕があれば、`Clamp()`を使わず、`if`文だけで同じ処理を書いてみてください。

例えば、x座標が左端より小さい場合は次のように補正できます。

```cpp
if (m_pos.x < HitBoxRadius) {
	m_pos.x = HitBoxRadius;
}
```

右端、上端、下端の判定も追加してみましょう。書けたら、`Clamp()`を使った場合とコードの長さや読みやすさを比べてみてください。

4方向の補正が終わったら、最後に`setPos(m_pos)`を呼び、HitBoxの座標も更新してください。

### 課題7：コンストラクタでHitBoxを作る

`Player`を作るとき、親クラス`GameObject`へ次の3つを渡す必要があります。

- オブジェクトの種類：`ObjectType::Player`
- 初期位置：`pos`
- 円形HitBox：`std::make_shared<CircleHitBox>(pos, HitBoxRadius)`

コンストラクタの初期化リストを書いてみてください。

```cpp
Player::Player(const Vec2& pos)
	// ここへGameObjectの初期化を書く
{
}
```

<details>
<summary>解答例</summary>

```cpp
Player::Player(const Vec2& pos)
	: GameObject{ ObjectType::Player, pos,
		std::make_shared<CircleHitBox>(pos, HitBoxRadius) } {
}
```

</details>

### 課題8：draw()で自分のプレイヤーを描く

`m_pos`を中心として、好きな図形を組み合わせたプレイヤーを描いてみてください。

最低条件は「移動すると図形も一緒に動く」ことだけです。例えば円なら次のように描けます。

```cpp
Circle{ m_pos, HitBoxRadius }.draw(Palette::Skyblue);
```

三角形にする場合は、中心座標`m_pos`から各頂点を少しずつずらします。

```cpp
Triangle{
	m_pos + Vec2{ 0, -18 },
	m_pos + Vec2{ 16, 16 },
	m_pos + Vec2{ -16, 16 }
}.draw(ColorF{ 0.2, 0.55, 1.0 });
```

画面外へ出ない処理では半径18を使っているため、図形も中心から上下左右へ18より遠くならない大きさにすると、見た目まで画面内に収まります。

ここまで書けたら、各関数をつないだ完成版と見比べてみましょう。`Player.cpp`全体は次のようになります。

```cpp:GameObject/Player.cpp
#include "Player.h"

// 初期位置と、プレイヤー用の円形HitBoxを設定する
Player::Player(const Vec2& pos)
	: GameObject{ ObjectType::Player, pos,
		std::make_shared<CircleHitBox>(pos, HitBoxRadius) } {
}

// 矢印キーまたはWASDキーから、縦横それぞれの移動方向を求める
Vec2 Player::getMoveDirection() const {
	Vec2 direction{ 0.0, 0.0 };

	if (KeyLeft.pressed() || KeyA.pressed()) {
		direction.x -= 1.0;
	}
	if (KeyRight.pressed() || KeyD.pressed()) {
		direction.x += 1.0;
	}
	if (KeyUp.pressed() || KeyW.pressed()) {
		direction.y -= 1.0;
	}
	if (KeyDown.pressed() || KeyS.pressed()) {
		direction.y += 1.0;
	}

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
	const double x = Clamp(
		m_pos.x,
		HitBoxRadius,
		Scene::Width() - HitBoxRadius);

	const double y = Clamp(
		m_pos.y,
		HitBoxRadius,
		Scene::Height() - HitBoxRadius);

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
```

### 確認：画面外へ出ない範囲を説明してみる

画面の幅が800、プレイヤーの半径が18だとします。プレイヤー全体が画面内に残るために、中心のx座標はどこからどこまで移動できるでしょうか。

<details>
<summary>説明例</summary>

`18`から`800 - 18`までです。

中心座標を`0`まで許すと、円の左半分が画面外へ出てしまいます。そのため、最小値と最大値の両方から半径を考慮します。

`Clamp(値, 最小値, 最大値)`を使うと、値を指定範囲内へ収められます。y座標も同様に、`HitBoxRadius`から`Scene::Height() - HitBoxRadius`までにします。

</details>

なお、`Scene::DeltaTime()`が一時的に大きくなり、一度の更新で画面端を飛び越えても、最後に`Clamp()`を使うため画面内へ戻せます。

## InGameシーンへPlayerを追加する

作った`Player`を`InGame`シーンから使えるようにします。

`Scene.h`で`Player.h`をインクルードし、`InGame`へメンバ変数を追加します。

```cpp:Scene/Scene.h
#pragma once
#include "../GameObject/Player.h"

// シーン変更時のフェードイン・アウトの時間
static const Duration TRANSITION_DURATION = 0.50s;

// シーンの種類
enum class SceneType {
	Title,
	IN_GAME
};

using App = SceneManager<SceneType>;

// ----- 各シーンのクラス ----- //

class Title : public App::Scene {
private:
public:
	Title(const InitData& data);

	void update() override;
	void draw() const override;
};

class InGame : public App::Scene {
private:
	Player m_player;

public:
	InGame(const InitData& data);

	void update() override;
	void draw() const override;
};
```

`InGame.cpp`では、コンストラクタでプレイヤーを画面中央に作ります。そして`update()`で更新し、`draw()`で描画します。

```cpp:Scene/InGame.cpp
#include "Scene.h"

// インゲームシーンのコンストラクタ
InGame::InGame(const InitData& data)
	: IScene(data)
	, m_player{ Scene::Center() } {
}

// インゲームシーンの更新処理
void InGame::update() {
	// プレイヤーのキー入力と移動を更新する
	m_player.update();

	// Qキーが押されたら、タイトルシーンに遷移する
	if (KeyQ.down()) {
		changeScene(SceneType::Title, TRANSITION_DURATION);
	}
}

// インゲームシーンの描画処理
void InGame::draw() const {
	// 薄緑の背景を描画
	Scene::SetBackground(ColorF{ 0.6, 0.8, 0.7 });

	// 更新された座標にプレイヤーを描画する
	m_player.draw();

	// プレイヤーの操作方法を画面左上に表示する
	FontAsset(U"Guide")(U"Move: Arrow keys / WASD")
		.draw(20, 20, Palette::Black);
}
```

操作説明用のフォントを、`Main.cpp`へ追加します。

```cpp
// フォントの宣言
FontAsset::Register(U"Title", 60, Typeface::Regular);
FontAsset::Register(U"Guide", 24, Typeface::Regular);
```

これで実行してみましょう。スペースキーでゲームシーンへ移動し、矢印キーまたはWASDキーで青いプレイヤーが動けば成功です。画面端へ移動し続けても、プレイヤー全体が画面内に残ることも確認してください。

## 余裕がある人向けの追加課題

完成した人は、次の改造にも挑戦してみてください。

1. `MoveSpeed`を変更し、操作感がどう変わるか試す
2. Shiftキーを押している間だけ低速移動にする
3. `H`キーを押している間、HitBoxの円を半透明で表示する
4. ウィンドウサイズを変更しても画面外へ出ないか確認する
5. プレイヤーの色や形を自分好みに変える

低速移動は、例えば更新時に使う速度を次のように切り替えられます。

```cpp
const double currentSpeed = KeyShift.pressed()
	? MoveSpeed * 0.5
	: MoveSpeed;
```

この値を`MoveSpeed`の代わりに移動量の計算へ使ってみてください。

HitBoxの表示については、`HitBox`へデバッグ描画用の`draw()`を追加する方法があります。親クラスと子クラスのどちらへ何を書くべきか、今回の`intersects()`を参考に考えてみるとよい練習になります。

## まとめ

今回は、上下左右に動くプレイヤーを作りました。

単純な移動処理だけでなく、次の点も意識して実装しています。

- 入力方向を`Vec2`へまとめる
- 方向を正規化し、斜め移動の速度をそろえる
- `Scene::DeltaTime()`を使い、フレームレートによる速度差を抑える
- `Clamp()`でプレイヤー全体を画面内へ収める
- `GameObject`へ共通情報をまとめる
- `HitBox`を分離し、今後の攻撃判定へ備える

今の段階では`GameObject`や`HitBox`が少し大げさに見えるかもしれません。しかし、敵や弾が増えたときに、今回の準備が効いてきます。たぶん。効いてくれないと少し困ります。

次回は、プレイヤーが弾を発射できるようにし、画面外へ出た弾を削除する処理を作る予定です。

## 参考

- [Siv3D リファレンス](https://siv3d.github.io/ja-jp/)
- [筆者の過去記事：Siv3Dで耐久アイワナを作ってみる その1](https://qiita.com/Ryota123/items/a63acdcdfc3987ca0acf)
