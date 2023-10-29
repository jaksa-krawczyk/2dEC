#include "World.hpp"

int main()
{
	World<800, 640> world;

	if (world.initializeWorld())
	{
		world.run();
	}

	return 0;
}