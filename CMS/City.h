#ifndef CITY_H
#define CITY_H


class City {
public:
	City();
	~City();
	int getPopulation();
	int getDevelopment();
	void setPopulation(int p);
	void setDevelopment(int d);
	int** getGrid(int sizeX, int sizeY);
private:
	int population;
	int development;
};

#endif