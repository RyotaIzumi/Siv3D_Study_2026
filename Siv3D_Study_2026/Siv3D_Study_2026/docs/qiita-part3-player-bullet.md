<!--
タイトル案:
Siv3Dでシューティングゲームを作る Part 3 - プレイヤーの弾を発射する

タグ案:
C++, Siv3D, ゲーム制作, 初心者, シューティングゲーム
-->

# 目次

<!-- Qiita投稿時に目次リンクを追加する -->

前回は、プレイヤーの土台となるクラスと、移動処理を作りました。

<!-- Part 2公開後、ここへリンクを追加する -->

今回は、プレイヤーが弾を発射できるようにします。

Zキーを押したときに弾を作り、その弾を画面上方向へ移動させます。また、画面外へ出た弾を削除する処理も作ります。

使用するものは、Siv3D v0.6.16、Visual Studio 2026です。

## 今回作るもの

今回の完成条件は次の通りです。

- Zキーを押すと、プレイヤーの上側から弾が1発作られる
- 弾が画面上方向へ進む
- 弾も`GameObject`を継承し、円形の`HitBox`を持つ
- 発射済みの弾をすべて更新・描画する
- 画面上端を越えた弾を配列から削除する

![image.png](https://qiita-image-store.s3.ap-northeast-1.amazonaws.com/0/3821471/6606a4bc-5224-439d-9d2c-11c096ebdb44.png)

今回はZキーを押した瞬間に1発発射します。押し続けている間の連射は、記事後半の追加課題にします。

## 今回の課題の進め方

この記事では、課題を書くたびにゲームを実行し、成功したことを画面で確認できるようにします。

最初から弾のすべてを生徒に書いてもらうと、途中のコードだけではゲームから呼ばれず、正しく書けたのか確認できません。そこで、先に「弾を作る・更新する・描画する」という周辺部分を、動作確認用の土台として配布します。

そのうえで、`PlayerBullet`の次の関数を一つずつ完成させます。

1. `draw()`を作り、弾が表示されることを確認する
2. `update()`を作り、弾が上へ動くことを確認する
3. `isOutsideScreen()`を作り、画面外の弾が削除されることを確認する

課題へ進む前に、まず次の「動作確認用の土台」まで全員で実装してください。

## ファイルを追加する

`GameObject`フォルダへ、次の2ファイルを追加します。

```text
GameObject/
├── GameObject.h
├── HitBox.h
├── HitBox.cpp
├── Player.h
├── Player.cpp
├── PlayerBullet.h    // 今回追加
└── PlayerBullet.cpp  // 今回追加
```

Visual Studioからファイルを作成すれば、通常はプロジェクトにも追加されます。エクスプローラーなどから作成した場合は、「既存の項目の追加」を使ってプロジェクトへ登録してください。

## 動作確認用のPlayerBulletを用意する

最初に、穴埋め状態の`PlayerBullet`を用意します。

弾もプレイヤーと同じゲーム内オブジェクトです。そのため、前回作った`GameObject`を継承し、座標、オブジェクトの種類、HitBoxなどの共通機能を利用します。

`PlayerBullet.h`は最初から次のコードを使用してください。

```cpp:GameObject/PlayerBullet.h
#pragma once
#include "GameObject.h"

// プレイヤーが発射し、画面上方向へ進む弾
class PlayerBullet : public GameObject {
private:
	static constexpr double MoveSpeed = 500.0;
	static constexpr double HitBoxRadius = 6.0;

public:
	PlayerBullet(const Vec2& pos);

	// 弾を1フレーム分、上方向へ移動する
	void update() override;

	// 弾を円として画面に描く
	void draw() const override;

	// 弾全体が画面上端を越えたかを調べる
	bool isOutsideScreen() const;
};
```

今回は次の情報と関数を用意します。

- `MoveSpeed`：弾が1秒間に進む距離
- `HitBoxRadius`：円形HitBoxの半径
- `update()`：弾を上方向へ移動する
- `draw()`：弾を画面へ描く
- `isOutsideScreen()`：弾が画面上端を越えたか調べる

`PlayerBullet`も`GameObject`を継承しているため、`update()`と`draw()`を実装する必要があります。

次に`PlayerBullet.cpp`へ、次の穴埋め用コードを書きます。

```cpp:GameObject/PlayerBullet.cpp
#include "PlayerBullet.h"

// 初期位置と、プレイヤー弾用の円形HitBoxを設定する
PlayerBullet::PlayerBullet(const Vec2& pos)
	: GameObject{ ObjectType::PlayerBullet, pos,
		std::make_shared<CircleHitBox>(pos, HitBoxRadius) } {
}

// 課題2で、弾を上方向へ動かす処理を書く
void PlayerBullet::update() {
}

// 課題1で、弾を描く処理を書く
void PlayerBullet::draw() const {
}

// 課題3で、画面外へ出たか調べる処理を書く
bool PlayerBullet::isOutsideScreen() const {
	return false;
}
```

この時点では3つの関数が未完成です。ただし、関数自体は定義されているため、ビルド時に「関数が見つからない」というエラーにはなりません。

`isOutsideScreen()`が一時的に`false`を返しているのは、戻り値のある関数をコンパイルできる状態にするためです。

## Playerへ動作確認用の土台を追加する

続いて、Zキーで弾を作り、作った弾の関数を毎フレーム呼ぶ処理を先に追加します。

ここは課題ではありません。`PlayerBullet`の各課題をすぐ実行確認するためのコードとして、全員で同じものを実装します。

### Player.hを変更する

`Player.h`で`PlayerBullet`を使用するため、先頭でヘッダを読み込みます。

```diff_cpp:GameObject/Player.h
 #pragma once
 #include "GameObject.h"
+#include "PlayerBullet.h"
```

`Player`の`private`へ、弾の配列と2つの関数を追加します。

```diff_cpp:GameObject/Player.h
 class Player : public GameObject {
 private:
 	static constexpr double MoveSpeed = 300.0;
 	static constexpr double HitBoxRadius = 18.0;
+	Array<PlayerBullet> m_bullets;

 	Vec2 getMoveDirection() const;
 	void keepInsideScreen();

+	// Zキーが押されたら、プレイヤーの正面に弾を作る
+	void shoot();

+	// 発射済みの弾を移動し、画面外の弾を削除する
+	void updateBullets();
```

`Array`は、複数の値を順番に保存できるSiv3Dの配列です。今回は`PlayerBullet`を保存するため、`Array<PlayerBullet>`と書きます。

https://siv3d.github.io/ja-jp/tutorial2/array/

### Zキーで弾を追加する

`Player.cpp`へ`shoot()`を追加します。

```cpp
// Zキーを押した瞬間に、プレイヤーの上端から弾を1発発射する
void Player::shoot() {
	if (KeyZ.down()) {
		m_bullets.emplace_back(
			m_pos + Vec2{ 0.0, -HitBoxRadius });
	}
}
```

`KeyZ.down()`は、Zキーを押した瞬間だけ`true`になります。

`emplace_back()`は、渡した引数を使って配列の末尾へ新しい要素を直接作る関数です。今回の`PlayerBullet`はコンストラクタで初期位置を受け取るため、`emplace_back(初期位置)`で新しい弾を作れます。

弾の初期位置には、プレイヤーの座標から上方向へ`HitBoxRadius`だけずらした位置を指定しています。

### すべての弾を更新し、画面外の弾を削除する

続けて`updateBullets()`を追加します。

```cpp
// すべての弾を更新し、画面上端を越えた弾を配列から取り除く
void Player::updateBullets() {
	for (auto& bullet : m_bullets) {
		bullet.update();
	}

	m_bullets.remove_if([](const PlayerBullet& bullet) {
		return bullet.isOutsideScreen();
	});
}
```

範囲for文を使い、配列内のすべての弾に対して`update()`を呼びます。

`auto& bullet`の`&`は参照を表します。配列に入っている弾そのものを更新するために必要です。

その後、`remove_if()`を使い、`isOutsideScreen()`が`true`になった弾を配列から削除します。今は穴埋め用コードが必ず`false`を返すため、まだ弾は削除されません。

### Player::update()から呼び出す

`Player::update()`の最後で`shoot()`と`updateBullets()`を呼びます。

```diff_cpp
 void Player::update() {
 	Vec2 direction = getMoveDirection();

 	if (not direction.isZero()) {
 		direction.normalize();
 	}

 	setPos(m_pos + direction * MoveSpeed * Scene::DeltaTime());
 	keepInsideScreen();
+	shoot();
+	updateBullets();
 }
```

### すべての弾を描画する

`Player::draw()`の先頭で、配列内のすべての弾に対して`draw()`を呼びます。

```diff_cpp
 void Player::draw() const {
+	for (const auto& bullet : m_bullets) {
+		bullet.draw();
+	}

 	Triangle{
 		m_pos + Vec2{ 0, -18 },
```

描画では弾の情報を変更しないため、`const auto&`で受け取ります。

### 弾の数を一時的に表示する

最後に、課題の成功を確認できるよう、現在の弾数を画面へ表示します。

`Player::draw()`の末尾へ、次のコードを一時的に追加してください。

```cpp
// 課題の動作確認用。すべて完成したら削除してよい
FontAsset(U"Guide")(U"Bullets: {}"_fmt(m_bullets.size()))
	.draw(20, 50, Palette::Black);
```

`m_bullets.size()`で、配列へ保存されている弾の数を取得できます。

### 土台の動作を確認する

ここまで書いたら、一度ビルドして実行してください。

ゲーム画面でZキーを押すと、画面左上の`Bullets`の数字が増えれば成功です。

まだ`draw()`が空なので、弾そのものは表示されません。また、`update()`も空なので移動しません。これは正しい状態です。

もし数字が増えない場合は、次の点を確認してください。

- `Player::update()`から`shoot()`を呼んでいるか
- `Player.h`へ`Array<PlayerBullet> m_bullets;`を追加したか
- `PlayerBullet.cpp`をVisual Studioのプロジェクトへ追加したか

これで、課題のコードがゲームから呼ばれる準備ができました。

## 課題1：弾を描画する

最初の課題では、弾の`draw()`を完成させます。

次の条件を満たすコードを書いてください。

- 円の中心には弾の現在位置`m_pos`を使う
- 円の半径には`HitBoxRadius`を使う
- 色は`ColorF{ 0.3, 0.9, 1.0 }`にする

```cpp
void PlayerBullet::draw() const {
	// ここに弾を描く処理を書く
}
```

<details>
<summary>ヒント</summary>

円は次の形で描けます。

```cpp
Circle{ 中心座標, 半径 }.draw(色);
```

</details>

<details>
<summary>解答例</summary>

```cpp
void PlayerBullet::draw() const {
	Circle{ m_pos, HitBoxRadius }.draw(ColorF{ 0.3, 0.9, 1.0 });
}
```

</details>

### 課題1の動作確認

ビルドして実行し、Zキーを押してください。

次の2点を確認できれば成功です。

- プレイヤーの上側に水色の円が表示される
- Zキーを押すたびに円と`Bullets`の数字が増える

まだ`update()`が空なので、表示された弾はその場から動きません。止まったままで正解です。

見た目とHitBoxを同じ半径にしておくと、画面で見た位置と実際に当たる位置を合わせやすくなります。

## 課題2：弾を上方向へ移動する

次は、表示できた弾を上方向へ移動させます。

Siv3Dの画面座標は、左上が`(0, 0)`です。

- x座標は右へ進むほど大きくなる
- y座標は下へ進むほど大きくなる

つまり、弾を上へ移動させるには、y座標を小さくします。

次の条件を満たす`update()`を書いてください。

1. x方向には移動しない
2. y方向には`-MoveSpeed`の速さで移動する
3. フレームレートによって速度が変わらないよう、`Scene::DeltaTime()`を使う
4. `setPos()`で座標とHitBoxを一緒に更新する

```cpp
void PlayerBullet::update() {
	// ここに上方向の移動処理を書く
}
```

<details>
<summary>ヒント</summary>

1フレーム分の移動量は次のように表せます。

```cpp
Vec2{ 0.0, -MoveSpeed * Scene::DeltaTime() }
```

この値を現在位置の`m_pos`へ足します。

</details>

<details>
<summary>解答例</summary>

```cpp
void PlayerBullet::update() {
	setPos(m_pos + Vec2{ 0.0, -MoveSpeed * Scene::DeltaTime() });
}
```

</details>

`setPos()`は前回`GameObject`へ作った関数です。弾の座標だけでなくHitBoxの座標も一緒に移動するため、見た目と当たり判定がずれません。

### 課題2の動作確認

ビルドして実行し、Zキーを何度か押してください。

次の3点を確認できれば成功です。

- 発射した弾が上方向へまっすぐ進む
- プレイヤーを移動してから撃つと、その位置から弾が出る
- 弾が画面上端を越えると見えなくなる

ただし、画面外へ出た弾はまだ配列に残っています。弾が見えなくなった後も、左上の`Bullets`の数字が減らないことを確認してください。

この「見えないけれど配列には残っている弾」を、次の課題で削除します。

## 課題3：画面外へ出た弾を削除できるようにする

発射した弾を配列へ残し続けると、見えなくなった弾もメモリ上に増え続けます。

そこで、弾全体が画面上端を越えたかを判定する`isOutsideScreen()`を完成させます。

弾の中心のy座標が`0`より小さくなっただけでは、円の一部がまだ画面内に残っている場合があります。

弾の中心を`m_pos.y`、半径を`HitBoxRadius`とすると、円の下端はどのような式で求められるでしょうか。

また、下端がいくつより小さくなれば、弾全体が画面上端を越えたと言えるでしょうか。

考えた条件を使い、次の関数を完成させてください。

```cpp
bool PlayerBullet::isOutsideScreen() const {
	return // ここに条件式を書く //;
}
```

<details>
<summary>ヒント</summary>

円の下端は「中心のy座標 + 半径」です。画面上端のy座標は`0.0`です。

</details>

<details>
<summary>解答例</summary>

```cpp
bool PlayerBullet::isOutsideScreen() const {
	return (m_pos.y + HitBoxRadius) < 0.0;
}
```

</details>

戻り値が`bool`なので、画面外なら`true`、まだ画面内なら`false`を返します。

`updateBullets()`では、`isOutsideScreen()`が`true`になった弾を`remove_if()`で削除するコードをすでに用意しています。そのため、この関数を完成させるだけで削除処理がゲームへ反映されます。

### 課題3の動作確認

ビルドして実行し、Zキーを何度か押してください。

次の3点を確認できれば成功です。

- 発射直後は`Bullets`の数字が増える
- 弾が画面上端を完全に越えると`Bullets`の数字が減る
- すべての弾が画面外へ出た後は`Bullets: 0`へ戻る

これで、画面外へ出た弾が配列から削除されていることを目で確認できます。

課題がすべて終わったら、動作確認用に追加した弾数表示は削除して構いません。

```cpp
// このコードを削除する
FontAsset(U"Guide")(U"Bullets: {}"_fmt(m_bullets.size()))
	.draw(20, 50, Palette::Black);
```

## PlayerBulletのコンストラクタ

課題では穴埋めに集中できるよう、コンストラクタを最初から用意しました。ここで内容を確認します。

```cpp
PlayerBullet::PlayerBullet(const Vec2& pos)
	: GameObject{ ObjectType::PlayerBullet, pos,
		std::make_shared<CircleHitBox>(pos, HitBoxRadius) } {
}
```

親クラスの`GameObject`へ次の3つを渡しています。

- オブジェクトの種類：`ObjectType::PlayerBullet`
- 初期位置：`pos`
- 円形HitBox：`std::make_shared<CircleHitBox>(pos, HitBoxRadius)`

`ObjectType::PlayerBullet`を設定しておくことで、今後敵との当たり判定を追加したときに「これはプレイヤーが撃った弾である」と判別できます。

## 弾を管理するコードの解説

ここからは、課題を動かすために先に追加した`Player`側のコードを詳しく見ていきます。

### Arrayで複数の弾を保存する

```cpp
Array<PlayerBullet> m_bullets;
```

弾を1発だけ作るのではなく、発射した複数の弾を保存しておくため、`Array`を使用しています。

### emplace_back()で弾を作る

```cpp
m_bullets.emplace_back(
	m_pos + Vec2{ 0.0, -HitBoxRadius });
```

`emplace_back()`は、渡した値をコンストラクタの引数にして、配列の末尾へ新しい要素を作ります。

今回は弾の初期位置を渡しているため、その位置に新しい`PlayerBullet`が作られます。

### 範囲for文で弾を更新する

```cpp
for (auto& bullet : m_bullets) {
	bullet.update();
}
```

配列に保存されたすべての弾を、1つずつ`bullet`という名前で取り出して更新しています。

`auto&`の`&`を付けることで、コピーではなく配列内の弾そのものを更新できます。

### remove_if()で不要な弾を削除する

```cpp
m_bullets.remove_if([](const PlayerBullet& bullet) {
	return bullet.isOutsideScreen();
});
```

Siv3Dの`Array`が持つ`remove_if()`は、条件が`true`になった要素を削除します。

渡している`[](...) { ... }`はラムダ式です。その場で作る、名前のない小さな関数と考えると分かりやすいです。

- `true`を返した弾：配列から削除される
- `false`を返した弾：配列に残る

今回は`bullet.isOutsideScreen()`の結果をそのまま返すため、画面外へ出た弾だけが削除されます。

## PlayerBullet.cppの完成コード

3つの課題が終わった`PlayerBullet.cpp`は、次のようになります。

```cpp:GameObject/PlayerBullet.cpp
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
```

## Player.hの完成コード

今回の変更を含む`Player.h`全体は次のようになります。

```cpp:GameObject/Player.h
#pragma once
#include "GameObject.h"
#include "PlayerBullet.h"

// キー操作で上下左右に移動するプレイヤー
class Player : public GameObject {
private:
	static constexpr double MoveSpeed = 300.0;
	static constexpr double HitBoxRadius = 18.0;
	Array<PlayerBullet> m_bullets;

	// 押されているキーから移動する方向を作る
	Vec2 getMoveDirection() const;

	// プレイヤーの中心が画面外へ出ない位置に直す
	void keepInsideScreen();

	// Zキーが押されたら、プレイヤーの正面に弾を作る
	void shoot();

	// 発射済みの弾を移動し、画面外の弾を削除する
	void updateBullets();

public:
	Player(const Vec2& pos);

	// キー入力を読み取り、プレイヤーを移動する
	void update() override;

	// プレイヤーを簡単な宇宙船の形で描く
	void draw() const override;
};
```

## Player.cppの完成コード

今回の変更を含む`Player.cpp`全体は次のようになります。

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
	if (KeyLeft.pressed() || KeyA.pressed()) direction.x -= 1.0;
	if (KeyRight.pressed() || KeyD.pressed()) direction.x += 1.0;
	if (KeyUp.pressed() || KeyW.pressed()) direction.y -= 1.0;
	if (KeyDown.pressed() || KeyS.pressed()) direction.y += 1.0;
	return direction;
}

// 入力方向へ移動し、最後に画面外へ出ていないかを確認する
void Player::update() {
	Vec2 direction = getMoveDirection();

	if (not direction.isZero()) {
		direction.normalize();
	}

	setPos(m_pos + direction * MoveSpeed * Scene::DeltaTime());
	keepInsideScreen();
	shoot();
	updateBullets();
}

// Clampでx座標とy座標を画面内の範囲に収める
void Player::keepInsideScreen() {
	const double x = Clamp(
		m_pos.x, HitBoxRadius, Scene::Width() - HitBoxRadius);
	const double y = Clamp(
		m_pos.y, HitBoxRadius, Scene::Height() - HitBoxRadius);
	setPos(Vec2{ x, y });
}

// Zキーを押した瞬間に、プレイヤーの上端から弾を1発発射する
void Player::shoot() {
	if (KeyZ.down()) {
		m_bullets.emplace_back(
			m_pos + Vec2{ 0.0, -HitBoxRadius });
	}
}

// すべての弾を更新し、画面上端を越えた弾を配列から取り除く
void Player::updateBullets() {
	for (auto& bullet : m_bullets) {
		bullet.update();
	}

	m_bullets.remove_if([](const PlayerBullet& bullet) {
		return bullet.isOutsideScreen();
	});
}

// 三角形と円を組み合わせ、上向きの宇宙船として描く
void Player::draw() const {
	for (const auto& bullet : m_bullets) {
		bullet.draw();
	}

	Triangle{
		m_pos + Vec2{ 0, -18 },
		m_pos + Vec2{ 16, 16 },
		m_pos + Vec2{ -16, 16 }
	}.draw(ColorF{ 0.2, 0.55, 1.0 });
	Circle{ m_pos, 7 }.draw(ColorF{ 0.8, 0.95, 1.0 });
}
```

## 操作説明を変更する

最後に、`InGame.cpp`の操作説明へZキーを追加します。

```cpp
FontAsset(U"Guide")(
	U"Move: Arrow keys / WASD   Shot: Z"
).draw(20, 20, Palette::Black);
```

今回の実装では、`Player`が弾の更新と描画も行うため、`InGame`側へ弾専用の処理を追加する必要はありません。

## 最終確認

動作確認用の弾数表示を削除した状態でビルドし、次の動作を確認します。

1. スペースキーでゲーム画面へ移動する
2. Zキーを押すと、プレイヤーの上側から水色の弾が1発出る
3. プレイヤーを動かしてから撃つと、その位置から弾が出る
4. 弾が上方向へまっすぐ移動する
5. Zキーを押し続けても、押した瞬間の1発だけが出る
6. 複数回押すと、複数の弾が同時に移動する

## 余裕がある人向けの追加課題

### 弾の速さや大きさを変える

`PlayerBullet.h`の`MoveSpeed`や`HitBoxRadius`を変更し、見た目や操作感の違いを確認してみてください。

### 弾を2列同時に発射する

プレイヤーの中心から左右へ少しずらした位置へ、弾を2つ追加してみてください。

```cpp
m_bullets.emplace_back(m_pos + Vec2{ -8.0, -HitBoxRadius });
m_bullets.emplace_back(m_pos + Vec2{  8.0, -HitBoxRadius });
```

### Zキーを押している間、一定間隔で連射する

`KeyZ.pressed()`だけへ変更すると毎フレーム発射されてしまいます。

「最後に発射してから何秒経過したか」を保存し、例えば0.15秒経過した場合だけ次の弾を作る処理を考えてみてください。

必要になりそうな情報は次の2つです。

- 次に発射できるまでの待ち時間
- 前回の発射から経過した時間

## まとめ

今回は、Zキーでプレイヤーの弾を発射する処理を作りました。

実装した内容は次の通りです。

- `PlayerBullet`も`GameObject`を継承させた
- 弾へ円形HitBoxを持たせた
- y座標を小さくして、弾を上方向へ移動させた
- `Array<PlayerBullet>`で複数の弾を管理した
- `emplace_back()`で新しい弾を追加した
- 範囲for文ですべての弾を更新・描画した
- `remove_if()`とラムダ式で画面外の弾を削除した

今回は、課題ごとに実行結果を確認できるよう、最初に動作確認用の土台を用意しました。

- 課題1の後：弾が表示される
- 課題2の後：弾が上へ動く
- 課題3の後：画面外へ出た弾数が減る
- 

このように一つずつ動作を増やすと、うまく動かなかった場合も、どの関数に原因があるか探しやすくなります。

次回は、ボスとなる敵を画面へ追加し、プレイヤーの弾が当たったか判定できるようにする予定です。

## 参考

- [Siv3D公式サイト](https://siv3d.github.io/ja-jp/)
- [Siv3Dチュートリアル：配列](https://siv3d.github.io/ja-jp/tutorial2/array/)
