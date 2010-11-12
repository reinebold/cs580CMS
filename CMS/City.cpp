#include "City.h"


/**
 * City()
 * Create a city with default population and development levels.
 */
City::City() {
	population = 5;
	development = 5;
}

City::~City() {

}

int City::getDevelopment() {
	return development;
}

int City::getPopulation() {
	return population;
}

void City::setPopulation(int p) {
	population = p;
}

void City::setDevelopment(int d) {
	development = d;
}


/**
 * getGrid(int, int)
 * It is the responsibility of the caller to delete the array returned by this function.
 */
int** City::getGrid(int sizeX, int sizeY) {
    int** grid = new int*[sizeX];
	for(int i=0; i < sizeX; i++) {
		grid[i] = new int[sizeY];
	}

	for(int i=0; i < sizeX; i++) {
		for (int j=0; j < sizeY; j++) {
			grid[i][j] = 1;
		}
	}
	return grid;
}