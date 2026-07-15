# pragma once
# include <Siv3d.hpp>

	namespace Sound {

		const String MAIN{ U"sndSample" };
		const FilePath Path_MAIN{ U"Sound/Bgm/sndSample.mp3" };

		//SEの名前とパスの定数
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
