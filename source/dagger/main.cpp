
#include "core/engine.h"
#include "gameplay/ping_pong/ping_pong_main.h"
#include "gameplay/platformer/platformer.h"

int main(int argc_, char** argv_)
{
	dagger::Engine engine;
	return engine.Run<PingPongGame>();
}
