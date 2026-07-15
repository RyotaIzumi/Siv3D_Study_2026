# 目次
https://qiita.com/Ryota123/private/ad363a4778e02b4035a2
***
<!-- Qiita投稿時に目次リンクを追加する -->

前回は、敵の移動、敵弾の弾幕、プレイヤーの被弾判定を作りました。

<!-- Part 5公開後、ここへリンクを追加する -->

今回は、プロジェクトに入っている画像と音をゲーム内へ反映します。

これまで図形で描いていたボスや弾幕を画像に置き換え、プレイヤーのショット音、ステージBGM、GameOver時の効果音も再生できるようにします。

使用するものは、Siv3D v0.6.16、Visual Studio 2026です。

## 今回作るもの

今回の完成条件は次の通りです。

- `sprCherry`をボス本体として表示する
- `sprCherryWhite`をボスの弾幕として表示する
- `sndSample`をステージBGMとして再生する
- `sndShoot`をプレイヤーが弾を撃ったときに再生する
- `sndDeath`をGameOverになったときに再生する
- 画像や音を直接ファイルパスで使わず、Asset名から呼び出す

今回使用する素材は、次の場所に入っています。

```txt
App/Texture/sprCherry.png
App/Texture/sprCherryWhite.png
App/Sound/Bgm/sndSample.mp3
App/Sound/Se/sndShoot.wav
App/Sound/Se/sndDeath.wav
```

## 今回の実装順

今回は、次の順番で実装します。

1. 画像素材を登録する
2. 音素材を登録する
3. `Main.cpp`で素材を読み込む
4. ボス本体へ`sprCherry`を反映する
5. ボスの弾幕へ`sprCherryWhite`を反映する
6. プレイヤーのショット音を鳴らす
7. ステージBGMを鳴らす
8. GameOver時の効果音を鳴らす

今回は課題形式ではなく、完成コードを順番に確認していきます。

## TextureAssetを用意する

まず、画像素材をまとめて登録するためのファイルを用意します。

今回のプロジェクトでは、次の2ファイルを使います。

- `Texture/TextureAsset.h`
- `Texture/TextureAsset.cpp`

画像を使うたびにファイルパスを書くのではなく、最初にAssetとして登録しておくと、ゲーム中では名前だけで画像を呼び出せます。

例えば、次のように使えるようになります。

```cpp
TextureAsset(U"sprCherry")
```

## TextureAsset.h

`TextureAsset.h`では、画像登録に使う関数を宣言します。

```cpp:Texture/TextureAsset.h
#pragma once
#include<Siv3D.hpp>

void registerTextures();
void registerTexture(const String& name, const String& path);
void registerTexturesSync();
void loadTexturesSync();
```

それぞれの役割は次の通りです。

- `registerTextures()`：今回使う画像をまとめて登録リストへ追加する
- `registerTexture()`：画像名とファイルパスを1つ登録する
- `registerTexturesSync()`：Siv3Dの`TextureAsset`へ登録する
- `loadTexturesSync()`：登録した画像を先に読み込んでおく

## TextureAsset.cpp

`TextureAsset.cpp`では、実際に画像素材を登録します。

```cpp:Texture/TextureAsset.cpp
#include "TextureAsset.h"

Array<std::pair<String, String>> registry;

void registerTexture(const String& name, const String& path) {
	registry << std::pair<String, String>(name, path);
}

void registerTextures() {
	// 弾幕テクスチャ登録
	static const FilePath CherryPath = U"Texture/";
	registerTexture(U"sprCherry", CherryPath + U"sprCherry.png");
	registerTexture(U"sprCherryWhite", CherryPath + U"sprCherryWhite.png");
}

void registerTexturesSync() {
	for (auto& p : registry) {
		TextureAsset::Register(p.first, p.second);
	}
}

void loadTexturesSync() {
	for (auto& p : registry) {
		TextureAsset::Load(p.first);
	}
}
```

`registry`には、画像の名前とパスを保存しています。

```cpp
Array<std::pair<String, String>> registry;
```

`std::pair<String, String>`は、2つの値をセットで持つための型です。

今回は、

- 1つ目：Asset名
- 2つ目：ファイルパス

として使っています。

```cpp
registerTexture(U"sprCherry", CherryPath + U"sprCherry.png");
```

これにより、`sprCherry`という名前で`Texture/sprCherry.png`を使えるようになります。

## AudioAssetを用意する

次に、音素材をまとめて登録するためのファイルを用意します。

今回のプロジェクトでは、次の2ファイルを使います。

- `Audio/AudioAsset.h`
- `Audio/AudioAsset.cpp`

画像と同じように、音も最初にAssetとして登録しておくと、ゲーム中では名前だけで呼び出せます。

```cpp
AudioAsset(Sound::SHOOT).playOneShot();
```

## AudioAsset.h

`AudioAsset.h`では、音素材のAsset名とファイルパスを定数として用意します。

```cpp:Audio/AudioAsset.h
# pragma once
# include <Siv3d.hpp>

namespace Sound {

	const String MAIN{ U"sndSample" };
	const FilePath Path_MAIN{ U"Sound/Bgm/sndSample.mp3" };

	// SEの名前とパスの定数
	static const String SE_PATH = U"Sound/Se/";

	static const String SHOOT = U"sndShoot";
	static const String SHOOT_PATH = SE_PATH + SHOOT + U".wav";
	static const String DEATH = U"sndDeath";
	static const String DEATH_PATH = SE_PATH + DEATH + U".wav";

	void registerBGMs();
	void registerSEs();
	void registerAudio(const String& name, const String& path, const Loop loop = Loop::No);
	void registerAudiosSync();
	void loadAudiosSync();
}
```

今回使う音は3つです。

| Asset名 | ファイル | 使う場面 |
|---|---|---|
| `sndSample` | `Sound/Bgm/sndSample.mp3` | ステージBGM |
| `sndShoot` | `Sound/Se/sndShoot.wav` | プレイヤーが弾を撃つとき |
| `sndDeath` | `Sound/Se/sndDeath.wav` | GameOverになったとき |

`namespace Sound`で囲むことで、音素材に関係する名前をまとめています。

```cpp
Sound::SHOOT
Sound::DEATH
Sound::MAIN
```

このように書けるため、「これは音素材の名前だ」と分かりやすくなります。

## AudioAsset.cpp

`AudioAsset.cpp`では、実際に音素材を登録します。

```cpp:Audio/AudioAsset.cpp
# include "AudioAsset.h"
# include <cassert>

namespace Sound {

	struct SoundRegistry {
		String name;
		String path;
		Loop loop = Loop::No;
	};

	Array<SoundRegistry> registry;

	// BGMを登録する
	void registerBGMs() {
		registerAudio(MAIN, Path_MAIN, Loop::Yes);
	}

	// SEを登録する
	void registerSEs() {
		registerAudio(SHOOT, SHOOT_PATH);
		registerAudio(DEATH, DEATH_PATH);
	}

	/**
	 * @brief オーディオを登録する
	 * @param name オーディオ名
	 * @param path オーディオファイルのパス
	 * @param loop ループ設定
	 */
	void registerAudio(const String& name, const String& path, const Loop loop) {
		registry << SoundRegistry{ name, path, loop };
	}

	// オーディオを同期的に登録する
	void registerAudiosSync() {
		for (auto& r : registry) {
			AudioAsset::Register(r.name, r.path, r.loop);
		}
	}

	// オーディオを同期的にロードする
	void loadAudiosSync() {
		for (auto& r : registry) {
			AudioAsset::Load(r.name);
		}
	}
}
```

音素材では、画像と違って`loop`の情報も持たせています。

```cpp
struct SoundRegistry {
	String name;
	String path;
	Loop loop = Loop::No;
};
```

BGMは繰り返し流したいので、`Loop::Yes`を指定します。

```cpp
registerAudio(MAIN, Path_MAIN, Loop::Yes);
```

一方、ショット音やGameOver音は一度だけ鳴ればよいので、ループしません。

```cpp
registerAudio(SHOOT, SHOOT_PATH);
registerAudio(DEATH, DEATH_PATH);
```

`registerAudio()`の引数にはデフォルト値として`Loop::No`を指定しているため、何も書かなければループしない音として登録されます。

## Main.cppで素材を読み込む

作成した`TextureAsset`と`AudioAsset`を`Main.cpp`で読み込みます。

```diff_cpp:Main.cpp
 #include <Siv3D.hpp> // Siv3D v0.6.16
 #include "Scene/Scene.h"
+#include "Texture/TextureAsset.h"
+#include "Audio/AudioAsset.h"
```

次に、シーンを作る前に画像と音を登録・ロードします。

```cpp:Main.cpp
registerTextures();
registerTexturesSync();
loadTexturesSync();

Sound::registerBGMs();
Sound::registerSEs();
Sound::registerAudiosSync();
Sound::loadAudiosSync();
```

この処理は、ゲームが始まる前の準備です。

シーン内で画像や音を使う前に登録しておく必要があります。

`Main.cpp`全体では、次のようになります。

```cpp:Main.cpp
#include <Siv3D.hpp> // Siv3D v0.6.16
#include "Scene/Scene.h"
#include "Texture/TextureAsset.h"
#include "Audio/AudioAsset.h"

void Main()
{
	Window::SetTitle(U"Sample");

	// フォントの宣言
	FontAsset::Register(U"Title", 60, Typeface::Regular);
	FontAsset::Register(U"Guide", 24, Typeface::Regular);

	registerTextures();
	registerTexturesSync();
	loadTexturesSync();

	Sound::registerBGMs();
	Sound::registerSEs();
	Sound::registerAudiosSync();
	Sound::loadAudiosSync();

	// シーン管理クラスのインスタンスを生成
	SceneManager<SceneType> app;
	app.add<Title>(SceneType::Title);
	app.add<InGame>(SceneType::IN_GAME);
	app.init(SceneType::Title, 0s);

	// メインループ
	while (System::Update()) {
		if (not app.update()) {
			break;
		}
	}
}
```

## ボス本体を画像に置き換える

前回までは、ボス本体を赤い円で描いていました。

```cpp
Circle{ m_pos, HitBoxRadius }.draw(ColorF{ 0.9, 0.2, 0.2 });
```

今回は、これを`sprCherry`の画像に置き換えます。

```cpp:GameObject/Enemy.cpp
const ScopedRenderStates2D rs{ SamplerState::ClampNearest };
TextureAsset(U"sprCherry").scaled(4.0).drawAt(m_pos);
```

`TextureAsset(U"sprCherry")`で、登録済みの画像を呼び出します。

`drawAt(m_pos)`は、画像の中心が`m_pos`に来るように描画します。

ボスの座標`m_pos`は中心座標として扱っているため、今回は`drawAt()`を使うと自然です。

### scaled(4.0)について

元画像の`sprCherry.png`は小さめの画像です。

そのまま描くとボスとしては小さいため、次のように4倍に拡大しています。

```cpp
scaled(4.0)
```

### ScopedRenderStates2DとClampNearestについて

画像を拡大するとき、描画設定によっては画像がぼやけたり、逆に中途半端に粗く見えたりします。

今回の素材はドット絵のような見た目なので、ピクセルの形を残して拡大するために`SamplerState::ClampNearest`を使っています。

```cpp
const ScopedRenderStates2D rs{ SamplerState::ClampNearest };
```

`ScopedRenderStates2D`は、「このスコープ内だけ描画設定を変更する」ための仕組みです。

この行を書いた後に描画される`sprCherry`は、`ClampNearest`の設定で描画されます。

関数を抜けると設定は元に戻るため、他の描画へ影響しにくくなります。

完成した`Enemy::draw()`の一部は次のようになります。

```cpp:GameObject/Enemy.cpp
// 敵を赤い円で描き、その上に残りHPを表すバーを描く
void Enemy::draw() const {
	for (const auto& bullet : m_bullets) {
		bullet.draw();
	}

	if (isDead()) {
		return;
	}

	const ScopedRenderStates2D rs{ SamplerState::ClampNearest };
	TextureAsset(U"sprCherry").scaled(4.0).drawAt(m_pos);

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

見た目は画像に変わりましたが、当たり判定はこれまで通り円形の`HitBox`を使っています。

そのため、プレイヤー弾との当たり判定処理は変更しなくて大丈夫です。

## ボス弾幕を画像に置き換える

次に、敵弾の見た目を`sprCherryWhite`へ置き換えます。

前回までは、紫色の円で描いていました。

```cpp
Circle{ m_pos, HitBoxRadius }.draw(ColorF{ 0.85, 0.25, 1.0 });
```

今回は、次のように変更します。

```cpp:GameObject/EnemyBullet.cpp
// 敵弾を紫色の円で描く
void EnemyBullet::draw() const {
	TextureAsset(U"sprCherryWhite").scaled(0.8).drawAt(m_pos);
}
```

敵弾も中心座標`m_pos`を基準にしているため、`drawAt(m_pos)`を使います。

`sprCherryWhite`も少し大きいため、敵弾としてちょうどよいサイズになるように`scaled(0.8)`で描画しています。

こちらも見た目だけを変更しています。

敵弾の当たり判定は、これまで通り`EnemyBullet`が持っている円形`HitBox`で行います。

## プレイヤーのショット音を鳴らす

次に、プレイヤーが弾を撃ったときに`sndShoot`を鳴らします。

`Player.cpp`で`AudioAsset.h`を読み込みます。

```diff_cpp:GameObject/Player.cpp
 #include "Player.h"
+#include "../Audio/AudioAsset.h"
```

`Player::shoot()`で、弾を作った直後に音を再生します。

```cpp:GameObject/Player.cpp
// Zキーを押した瞬間に、プレイヤーの上端から弾を1発発射する
void Player::shoot() {
	if (KeyZ.down()) {
		m_bullets.emplace_back(m_pos + Vec2{ 0.0, -HitBoxRadius });
		AudioAsset(Sound::SHOOT).playOneShot();
	}
}
```

`playOneShot()`は、効果音のように「その場で一度だけ鳴らしたい音」に使いやすい関数です。

ショット音は、何度も連続で鳴る可能性があります。

`playOneShot()`を使うことで、すでに鳴っている音があっても、効果音として自然に再生できます。

## ステージBGMを鳴らす

ステージBGMは、`InGame`シーンに入ったときに再生します。

`InGame.cpp`で`AudioAsset.h`を読み込みます。

```diff_cpp:Scene/InGame.cpp
 #include "Scene.h"
+#include "../Audio/AudioAsset.h"
```

`InGame`のコンストラクタでBGMを再生します。

```cpp:Scene/InGame.cpp
// インゲームシーンのコンストラクタ
InGame::InGame(const InitData& data)
	: IScene(data)
	, m_player{ Scene::Center() }
	, m_enemy{ Vec2{ Scene::Center().x, 150.0 } } {
	AudioAsset(Sound::MAIN).stop();
	AudioAsset(Sound::MAIN).play();
}
```

最初に`stop()`してから`play()`している理由は、シーンに入り直したときにBGMを最初から再生するためです。

```cpp
AudioAsset(Sound::MAIN).stop();
AudioAsset(Sound::MAIN).play();
```

また、`sndSample`は登録時に`Loop::Yes`を指定しているため、曲が終わると自動でループします。

```cpp
registerAudio(MAIN, Path_MAIN, Loop::Yes);
```

## リスタート時にもBGMを鳴らし直す

前回、GameOver後に`Rキー`でリスタートできるようにしました。

リスタート時にはゲーム状態を初期化するため、BGMも最初から鳴らし直します。

```cpp:Scene/InGame.cpp
// プレイヤーと敵を作り直し、ゲームオーバー状態を解除する
void InGame::resetGame() {
	m_player = Player{ Scene::Center() };
	m_enemy = Enemy{ Vec2{ Scene::Center().x, 150.0 } };
	m_isGameOver = false;
	AudioAsset(Sound::MAIN).stop();
	AudioAsset(Sound::MAIN).play();
}
```

これにより、GameOver後に`Rキー`を押すと、ゲーム本体だけでなくBGMも最初から再開します。

## GameOver時に効果音を鳴らす

敵弾がプレイヤーに当たったら、GameOverにします。

その瞬間にBGMを止め、`sndDeath`を再生します。

```cpp:Scene/InGame.cpp
// 敵弾が1発でもプレイヤーに当たったらGameOverにする
if (0 < m_enemy.countBulletHits(m_player)) {
	m_isGameOver = true;
	AudioAsset(Sound::MAIN).stop();
	AudioAsset(Sound::DEATH).playOneShot();
}
```

ここでは、`m_isGameOver`が`true`になった瞬間だけこの処理が通ります。

そのため、GameOver音が毎フレーム鳴り続けることはありません。

## タイトルへ戻るときにBGMを止める

`Qキー`でタイトルシーンへ戻るときは、ステージBGMを止めてからシーンを変更します。

```cpp:Scene/InGame.cpp
// Qキーが押されたら、タイトルシーンに遷移する
if (KeyQ.down()) {
	AudioAsset(Sound::MAIN).stop();
	changeScene(SceneType::Title, TRANSITION_DURATION);
}
```

これを入れておかないと、タイトルへ戻ったあともステージBGMが流れ続けてしまいます。

シーンをまたぐ音は、どのタイミングで再生し、どのタイミングで止めるかを決めておくことが大切です。

## InGame.cppの変更後コード

今回の変更を含む`InGame.cpp`は次のようになります。

```cpp:Scene/InGame.cpp
#include "Scene.h"
#include "../Audio/AudioAsset.h"

// インゲームシーンのコンストラクタ
InGame::InGame(const InitData& data)
	: IScene(data)
	, m_player{ Scene::Center() }
	, m_enemy{ Vec2{ Scene::Center().x, 150.0 } } {
	AudioAsset(Sound::MAIN).stop();
	AudioAsset(Sound::MAIN).play();
}

// プレイヤーと敵を作り直し、ゲームオーバー状態を解除する
void InGame::resetGame() {
	m_player = Player{ Scene::Center() };
	m_enemy = Enemy{ Vec2{ Scene::Center().x, 150.0 } };
	m_isGameOver = false;
	AudioAsset(Sound::MAIN).stop();
	AudioAsset(Sound::MAIN).play();
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
			AudioAsset(Sound::MAIN).stop();
			AudioAsset(Sound::DEATH).playOneShot();
		}
	}

	// Qキーが押されたら、タイトルシーンに遷移する
	if (KeyQ.down()) {
		AudioAsset(Sound::MAIN).stop();
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
	FontAsset(U"Guide")(U"Move: Arrow keys / WASD   Shot: Z").draw(20, 20, Palette::Black);

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

## 最終確認

プロジェクトをビルドして実行し、次の動作を確認します。

1. インゲームに入ると`sndSample`がBGMとして流れる
2. ボス本体が`sprCherry`で表示される
3. ボスの弾幕が`sprCherryWhite`で表示される
4. Zキーで弾を撃つと`sndShoot`が鳴る
5. 敵弾に当たるとBGMが止まり、`sndDeath`が鳴る
6. GameOver中にRキーを押すと、ゲームとBGMが最初から再開する
7. Qキーでタイトルへ戻ると、ステージBGMが止まる

## まとめ

今回は、ゲーム内に画像と音を反映しました。

実装した内容は次の通りです。

- `TextureAsset`で画像素材を登録した
- `AudioAsset`で音素材を登録した
- `Main.cpp`で素材をゲーム開始前に読み込んだ
- ボス本体を`sprCherry`で描画した
- ボス弾幕を`sprCherryWhite`で描画した
- `ScopedRenderStates2D`と`SamplerState::ClampNearest`で画像をドット絵らしく拡大した
- プレイヤーのショット時に`sndShoot`を再生した
- ステージBGMとして`sndSample`をループ再生した
- GameOver時にBGMを止め、`sndDeath`を再生した
- リスタート時にBGMも最初から再生し直した

図形だけで作っていた状態から、画像と音が入ることでゲームらしさがかなり増しました。

今後さらに素材を追加したい場合も、今回作った`TextureAsset`や`AudioAsset`へ登録してから、ゲーム中でAsset名を使って呼び出せば反映できます。

## 次回

次回は、ゲームクリアやリザルト、演出の追加などを行い、より完成形に近づけていく予定です。

## 参考

- [Siv3D公式サイト](https://siv3d.github.io/ja-jp/)
- [Siv3D アセット管理](https://siv3d.github.io/ja-jp/tutorial4/asset/)
