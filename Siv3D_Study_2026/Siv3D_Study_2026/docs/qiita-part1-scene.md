<!--
タイトル案:
Siv3Dでシューティングゲームを作る Part 1 - タイトルとゲーム画面を用意する

タグ案:
C++, Siv3D, ゲーム制作, 初心者, シューティングゲーム
-->

この記事では、Siv3Dを使って簡単なシューティングゲームを作るための下準備をします。

最終的には、上下左右に動くプレイヤーがボス1体と戦うゲームを作る予定です。さっそくプレイヤーを作りたいところですが、まずはプレイヤーを表示して動かすためのゲーム画面が必要です。

今回は、次の2つのシーンを作ります。

- ゲーム開始前に表示する`Title`シーン
- 実際にゲームを動かす`InGame`シーン

完成すると、タイトル画面でスペースキーを押してゲーム画面へ移動し、ゲーム画面でQキーを押してタイトルへ戻れるようになります。

使用するものは、Siv3D v0.6.16、Visual Studio 2026です。

## シーンとは

ゲームでは、タイトル画面、ゲーム画面、リザルト画面など、役割の異なる複数の画面を扱うことがあります。

これらの画面をすべて`Main.cpp`へ書くこともできます。しかし、ゲームが大きくなるにつれて「今はどの画面なのか」「どの処理がどの画面のものなのか」が分かりにくくなります。

そこで今回は、Siv3Dの`SceneManager`を使います。各画面を別のクラスとして分け、現在必要なシーンだけを更新・描画できる仕組みです。

GameMakerを触ったことがある人なら、ルームを切り替える仕組みに少し近いかもしれません。

Siv3Dのシーン管理について詳しく知りたい人は、公式チュートリアルもご覧ください。

- [シーン管理｜Siv3D公式チュートリアル](https://siv3d.github.io/ja-jp/tutorial3/scene-manager/)

## プロジェクトを準備する

まだSiv3Dのプロジェクトを作成していない場合は、公式サイトを参考に作成してください。

- [Siv3D プロジェクト作成（Windows）](https://siv3d.github.io/ja-jp/download/windows/)

今回は、作成したプロジェクトに次の3ファイルを追加します。

`Main.cpp`と同じ階層に`Scene`フォルダを作り、その中へ配置してください。

```text
Scene/
├── Scene.h
├── Title.cpp
└── InGame.cpp
```

Visual Studioでは、プロジェクトを右クリックして「追加」からフィルターやファイルを作成できます。

最終的なファイル構成は次のようになります。Siv3Dのプロジェクトには他にもファイルがありますが、今回主に使用する部分だけを載せています。

```text
プロジェクト/
├── Scene/
│   ├── Scene.h
│   ├── Title.cpp
│   └── InGame.cpp
├── Main.cpp
├── stdafx.cpp
└── stdafx.h
```

> Visual Studioのソリューションエクスプローラー画像をここに追加予定

## Scene.hを作る

まず、シーンの種類と各シーンのクラスを宣言する`Scene.h`を作ります。

```cpp:Scene/Scene.h
#pragma once

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
public:
	InGame(const InitData& data);

	void update() override;
	void draw() const override;
};
```

このプロジェクトでは、`stdafx.h`からSiv3Dのヘッダが全ファイルへ読み込まれる設定になっています。別の構成のプロジェクトへコードを移す場合は、`Scene.h`の先頭へ`#include <Siv3D.hpp>`を追加してください。

順番に中身を見ていきます。

### シーン移動にかかる時間

```cpp
static const Duration TRANSITION_DURATION = 0.50s;
```

シーンを切り替えるときのフェード時間を、`TRANSITION_DURATION`という名前で用意しています。

タイトルからゲームへ移動するときと、ゲームからタイトルへ戻るときの両方で同じ値を使います。後からフェード時間を変更したくなった場合も、この1か所を直すだけで済みます。

`0.50s`は0.5秒を表します。

### シーンの種類

```cpp
enum class SceneType {
	Title,
	IN_GAME
};
```

`enum class`を使い、ゲームに存在するシーンの種類を一覧にしています。

今回はタイトル画面を表す`Title`と、ゲーム画面を表す`IN_GAME`の2種類です。リザルト画面などを追加する場合は、今後この一覧へ種類を増やします。

### SceneManagerへ名前を付ける

```cpp
using App = SceneManager<SceneType>;
```

`SceneManager<SceneType>`をコード中で何度も書かなくてよいように、`App`という短い別名を付けています。

この`App`を使い、各シーンは`App::Scene`を継承します。

### 各シーンのクラス

```cpp
class Title : public App::Scene {
```

この部分で、`Title`クラスがSiv3Dのシーンクラスを継承しています。`InGame`クラスも同じです。

それぞれのクラスには次の関数を用意します。

- コンストラクタ：シーンが作られたときに呼ばれる
- `update()`：キー入力や移動などを毎フレーム更新する
- `draw()`：図形や文字を毎フレーム描画する

`override`は、親クラスに用意されている関数を子クラス側で上書きすることを表します。

今の段階では`private`の中身は空ですが、Part 2でプレイヤーを追加するときに使用します。

## Titleシーンを作る

次に、ゲーム開始前に表示するタイトル画面を作ります。

`Title.cpp`を次のようにします。

```cpp:Scene/Title.cpp
#include "Scene.h"

// タイトルシーンのコンストラクタ
Title::Title(const InitData& data) : IScene(data) {
}

// タイトルシーンの更新処理
void Title::update() {

	// スペースキーが押されたら、ゲームシーンに遷移する
	if (KeySpace.down()) {
		changeScene(SceneType::IN_GAME, TRANSITION_DURATION);
	}

}

// タイトルシーンの描画処理
void Title::draw() const {

	// 黒の背景を描画
	Scene::SetBackground(ColorF{ Palette::Black });
	// 文字表示
	FontAsset(U"Title")(U"Press space to start").drawAt(400, 304);

}
```

### コンストラクタ

```cpp
Title::Title(const InitData& data) : IScene(data) {
}
```

シーンの初期化に必要なデータを、親クラスの`IScene`へ渡しています。

今回はタイトルシーン独自の初期化処理がないため、波括弧の中は空です。今後、タイトル用の画像や音楽を準備する場合は、ここへ処理を追加できます。

### スペースキーでシーンを移動する

```cpp
if (KeySpace.down()) {
	changeScene(SceneType::IN_GAME, TRANSITION_DURATION);
}
```

`KeySpace.down()`は、スペースキーが押された瞬間だけ`true`になります。

キーが押されたら`changeScene()`を呼び、`IN_GAME`シーンへ移動します。第2引数には、先ほど用意したフェード時間を渡しています。

`pressed()`ではなく`down()`を使っているのは、キーを押し続けている間ではなく、押した瞬間だけ処理したいためです。

### 背景と文字を描く

```cpp
Scene::SetBackground(ColorF{ Palette::Black });
FontAsset(U"Title")(U"Press space to start").drawAt(400, 304);
```

背景色を黒に設定し、画面中央付近へ「Press space to start」と表示しています。

ここで使っている`Title`という名前のフォントは、後ほど`Main.cpp`で登録します。

## InGameシーンを作る

続いて、実際のゲーム画面になる`InGame`シーンを作ります。

まだプレイヤーや敵は作っていないため、今回は背景色の表示とタイトルへ戻る処理だけです。

```cpp:Scene/InGame.cpp
#include "Scene.h"

// インゲームシーンのコンストラクタ
InGame::InGame(const InitData& data) : IScene(data) {
}

// インゲームシーンの更新処理
void InGame::update() {
	// Qキーが押されたら、タイトルシーンに遷移する
	if (KeyQ.down()) {
		changeScene(SceneType::Title, TRANSITION_DURATION);
	}
}

// インゲームシーンの描画処理
void InGame::draw() const {
	// 薄緑の背景を描画
	Scene::SetBackground(ColorF{ 0.6, 0.8, 0.7 });
}
```

`Title`シーンと構造はほとんど同じです。

`update()`では、Qキーが押されたら`Title`シーンへ戻ります。

```cpp
if (KeyQ.down()) {
	changeScene(SceneType::Title, TRANSITION_DURATION);
}
```

`draw()`では、ゲーム画面だと分かるように背景を薄緑色へ設定しています。

```cpp
Scene::SetBackground(ColorF{ 0.6, 0.8, 0.7 });
```

`ColorF`では、赤・緑・青の強さを`0.0`から`1.0`で指定できます。

今は背景しか表示されませんが、これは正しい動作です。Part 2で、この画面へプレイヤーを追加します。

## Main.cppでシーンを登録する

最後に`Main.cpp`を変更し、作成したシーンをゲームへ登録します。

```cpp:Main.cpp
#include <Siv3D.hpp> // Siv3D v0.6.16
#include "Scene/Scene.h"

using App = SceneManager<SceneType>;

void Main()
{

	Window::SetTitle(U"Sample");

	// フォントの宣言
	FontAsset::Register(U"Title", 60, Typeface::Regular);

	// シーン管理クラスのインスタンスを生成
	App app;
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

### Scene.hを読み込む

```cpp
#include "Scene/Scene.h"
```

先ほど作った`Scene.h`を読み込み、`SceneType`、`Title`、`InGame`を`Main.cpp`から使えるようにします。

### フォントを登録する

```cpp
FontAsset::Register(U"Title", 60, Typeface::Regular);
```

`Title.cpp`で使用するフォントを`Title`という名前で登録しています。

第2引数の`60`はフォントサイズ、第3引数の`Typeface::Regular`は標準的な太さの書体を表します。

### シーンを追加する

```cpp
App app;
app.add<Title>(SceneType::Title);
app.add<InGame>(SceneType::IN_GAME);
```

まず`SceneManager`の実体である`app`を作り、タイトルシーンとゲームシーンを追加します。

`add<Title>()`の`Title`は作成するクラス、`SceneType::Title`はそのシーンを呼び出すときの種類です。

### 最初のシーンを決める

```cpp
app.init(SceneType::Title, 0s);
```

ゲーム起動時に、最初に表示するシーンを`Title`へ設定します。

第2引数を`0s`にしているため、起動時はフェードを待たずにタイトル画面が表示されます。

### メインループでシーンを更新する

```cpp
while (System::Update()) {
	if (not app.update()) {
		break;
	}
}
```

Siv3Dのメインループ内で`app.update()`を呼びます。

これにより、現在表示しているシーンの`update()`と`draw()`が毎フレーム呼ばれます。シーンが変わった場合も、`SceneManager`が更新・描画する対象を切り替えてくれます。

## 実行して確認する

ここまで書けたら、プロジェクトをビルドして実行します。

次の順番で動作すれば成功です。

1. 起動すると、黒い背景に「Press space to start」と表示される
2. スペースキーを押すと、フェードして薄緑色のゲーム画面へ移動する
3. Qキーを押すと、フェードしてタイトル画面へ戻る

> タイトル画面の画像をここに追加予定

> InGame画面の画像をここに追加予定

ゲーム画面には背景しか表示されません。プレイヤーをまだ作っていないので、何もいないのが正しい状態です。真っさらな画面を見ると少し不安になりますが、今回はそれで完成です。

## エラーが出たときの確認場所

うまく実行できない場合は、次の点を確認してみてください。

### Scene.hが見つからない

`Main.cpp`から見て`Scene/Scene.h`という位置にファイルがあるか確認します。

```cpp
#include "Scene/Scene.h"
```

`Title.cpp`と`InGame.cpp`は`Scene.h`と同じフォルダにあるため、こちらは次の書き方です。

```cpp
#include "Scene.h"
```

### 画面に文字が表示されない

`Main.cpp`でフォントを登録した名前と、`Title.cpp`で使う名前が同じか確認します。

```cpp
FontAsset::Register(U"Title", 60, Typeface::Regular);
FontAsset(U"Title")(U"Press space to start").drawAt(400, 304);
```

どちらも`U"Title"`になっている必要があります。

### シーンが切り替わらない

`Main.cpp`で両方のシーンを`app`へ追加しているか確認します。

```cpp
app.add<Title>(SceneType::Title);
app.add<InGame>(SceneType::IN_GAME);
```

また、`changeScene()`へ渡している`SceneType`が、移動したいシーンと一致しているかも確認してください。

## まとめ

今回は、シューティングゲームを作るための下準備として、タイトル画面とゲーム画面を用意しました。

実装した内容は次の通りです。

- `SceneType`でシーンの種類を定義した
- `Title`と`InGame`を別々のクラスとして作成した
- `SceneManager`へ2つのシーンを登録した
- スペースキーでゲーム画面へ移動できるようにした
- Qキーでタイトル画面へ戻れるようにした
- `update()`と`draw()`へ更新処理と描画処理を分けた

まだゲームらしいものは表示されませんが、これでプレイヤーや敵を動かす場所ができました。

Part 2では、`GameObject`を継承し、`HitBox`を持つプレイヤーを作ります。矢印キーまたはWASDキーで上下左右に動かし、画面外へ出ないようにします。

<!-- Part 2をQiitaへ投稿した後、ここへ記事URLを追加する -->

## 参考

- [Siv3D公式サイト](https://siv3d.github.io/ja-jp/)
- [シーン管理｜Siv3D公式チュートリアル](https://siv3d.github.io/ja-jp/tutorial3/scene-manager/)
- [筆者の過去記事：Siv3Dで耐久アイワナを作ってみる その1](https://qiita.com/Ryota123/items/a63acdcdfc3987ca0acf)
