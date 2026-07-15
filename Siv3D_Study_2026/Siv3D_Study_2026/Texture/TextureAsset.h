#pragma once
#include<Siv3D.hpp>

	void registerTextures();
	void registerTexture(const String& name, const String& path);
	void registerTexturesSync();
	void loadTexturesSync();
