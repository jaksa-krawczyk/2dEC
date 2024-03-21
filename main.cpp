#include "World.hpp"

int main()
{
	World<1600, 900> world;

	if (world.initializeWorld())
	{
		world.run();
	}

	return 0;
}