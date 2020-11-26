#include "tiles.h"
#include "tile.h"


#include "core/core.h"
#include "core/engine.h"
#include "core/graphics/shaders.h"
#include "core/graphics/textures.h"
#include "core/input/inputs.h"
#include "core/graphics/sprite.h"
#include "core/graphics/animation.h"
#include "core/graphics/window.h"
//#include "core/files.h"
//#include "core/stringops.h"
//#include "core/filesystem.h"
#include <ios>


using namespace dagger;

void TilemapLoadingSystem::OnLoadAsset(TilemapLoadRequest request_)
{

    assert(request_.legend != nullptr);
    Tilemap* tilemap = new Tilemap();

    Char ch;
    UInt32 x = 0, y = 0;

    FileInputStream input{request_.path};

    while (input >> std::noskipws >> ch) 
    {
        std::printf("its working");
        if(ch == '\n')
        {
            x = 0; y++;
        }
        else
        {
            assert(request_.legend->contains(ch));
           tilemap->tiles.push_back(request_.legend->at(ch)(Engine::Registry(), x, y));
           
        }
    }
     Engine::Res<Tilemap>()[request_.path] = tilemap;
    
    
   
}

void TilemapLoadingSystem::SpinUp()
{
    Engine::Dispatcher().sink<TilemapLoadRequest>().connect<&TilemapLoadingSystem::OnLoadAsset>(this);

}

void TilemapLoadingSystem::WindDown()
{
    Engine::Dispatcher().sink<TilemapLoadRequest>().disconnect<&TilemapLoadingSystem::OnLoadAsset>(this);
}

