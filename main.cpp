#include "World.hpp"

int main()
{
	World<800, 640> world;

	world.initializeWorld();
	world.run();

	return 0;
}