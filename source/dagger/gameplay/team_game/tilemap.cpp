#include "tilemap.h"

#include "core/engine.h"

#include <filesystem>

void TilemapSystem::SpinUp()
{
	Engine::Dispatcher().sink<TilemapLoadRequest>().connect<&TilemapSystem::OnLoadAsset>(this);
}

void TilemapSystem::WindDown()
{
	Engine::Dispatcher().sink<TilemapLoadRequest>().disconnect<&TilemapSystem::OnLoadAsset>(this);
}

void TilemapSystem::OnLoadAsset(TilemapLoadRequest request_)
{
	assert(request_.legend != nullptr);
	Tilemap* tilemap = new Tilemap();

	Char ch;
	UInt32 x = 0, y = 0;

	FileInputStream input{ request_.path };

	while (input >> std::noskipws >> ch)
	{
		if (ch == '\n')
		{
			x = 0;
			y++;
		}
		else
		{
			assert(request_.legend->contains(ch));
			tilemap->tiles.push_back((request_.legend->at(ch))(Engine::Registry(), x, y));
			x++;
		}
	}

	Engine::Res<Tilemap>()[std::filesystem::path(request_.path).filename().string()] = tilemap;
}
