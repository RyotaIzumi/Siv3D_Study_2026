#include "TextureAsset.h"

	Array<std::pair<String, String>> registry;

	void registerTexture(const String& name, const String& path) {
		registry << std::pair<String, String>(name, path);
	}

	void registerTextures() {
		//弾幕テクスチャ登録
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
