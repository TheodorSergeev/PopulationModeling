// Author: Vitaly Axenov
// 2018 --- \infty
#include <stdio.h>

#include <utility>
#include <vector>
#include <string>

using std::pair;
using std::vector;
using std::string;

//-------------------------------------------------------------------------------------------------
//
//-----Global-----
//
//-------------------------------------------------------------------------------------------------

#define PURE

enum AREA_TYPE
{
	EMPTY,
	FOOD,
	POISON,
	BIOCELL,
	WALL,
	ALLY,
	HOSTILE,
	OUT         // type for returning errors if (x,y) is out of range

};

typedef pair <int, int> coord_t;    // for passing (x,y) coordinates
extern const coord_t INCORRECT;     // for passing errors

enum ERRORS
{

	OUT_OF_FIELD = -100,
	OCCUPIED,
	NEGATIVE,
	BAD_RANGE

};


// A cell of the 2d Environment array. Either empty/cell/food/poison/...
class CEnvironmentArea
{

private:
	AREA_TYPE area_type;

protected:
	int hp; // food - hp_value > 0; posion - hp_value < 0

public:
	CEnvironmentArea(AREA_TYPE);
	virtual	~CEnvironmentArea(){};

	bool isDead   (){ if(hp == 0) return true; return false; }  // checks if hp == 0
	AREA_TYPE type(){ return area_type; }
	int HpValue   (){ return hp; }                              // returns hp value

	int Bite(int);                                              // function for modelling cells eating food and each other
                                                                // decrements hp by argument to a minimum of current hp;
                                                                // returns decremented value; in case of poison returned value
                                                                // is negative
	int ChangeHP(int);                                          // just changes hp by arg
	int ResetHP();
	virtual CEnvironmentArea* GetCopy(){ return NULL; };

};

typedef vector <vector<CEnvironmentArea*> >  envmap_t;
struct CView
{

	int x;
	int y;
	AREA_TYPE type;

};

typedef vector <CView> sur_t;

struct FoodInfo				// describes food allocation
					// if N = 0 no food is automatically allocated
					// if N > 0 allocates N sources with hp size in random places each period iterations
{
	int N;
	int hp;
	int period;
	int timer;
};

class CEnvironment
{

private:
	envmap_t field;
	FoodInfo fd;

public:
	CEnvironment(int, int);
	~CEnvironment();

	coord_t GetBounds();
	void DumpASCII(FILE *);
	void Print() { this->DumpASCII(stdout); }

	bool InField(int, int);

	bool InField(coord_t p)
	{

		return this->InField(std::get<0>(p), std::get<1>(p));

	}

	AREA_TYPE What(int, int);

	AREA_TYPE What(coord_t p)
	{

		return this->What(std::get<0>(p), std::get<1>(p));

	}

	int WhatBactType(int, int);

	int SpawnFood();						// somehow add food on the field
									// NOT IMPLEMENTED, USE NEXT ONE
	int DumbSpawnFood();						// Adds food in free cell with 50% chance
	int RandSpawnFood(int,int);
	int SetFoodInfo(int N, int hp, int period);
	int PlantObject(CEnvironmentArea *, int, int);	 		// puts object in certain place
	int PlantObject(CEnvironmentArea *, coord_t);
	int WipeOut(); 							// deletes every object in field
	int CleanUp(); 							// deletes all objects with 0 hp (eaten food, dead cells)
	sur_t *GetSurroundings(int, int, int);
	int CellAction(int, int);

	coord_t GetFreeAdj(int, int);
	coord_t Divide(int, int);

	int Iteration();						// will move to CExperiment later
	static CEnvironment *StartCondFromFile(string, int);
};

//-------------------------------------------------------------------------------------------------

class CFood: public CEnvironmentArea					// Class for food/poison source (hp > 0 / hp < 0)
{

private:
	static const int DEF_FOOD_VAL;

public:
	CFood(int);
	virtual	~CFood(){};

	bool isPoison (){ if(hp <  0) return true; return false; }
	bool isFood   (){ if(hp >  0) return true; return false; }

	CEnvironmentArea *GetCopy();

};
//---------------------------------------------------------------------------------------------------

// Cell "species" type; values that are common for all instances
struct CCellType
{

	int type_id;

	string type_name;

	int default_hp;					// hp value with which new biocell is created
	int speed;						// bad name actually as the biocell is SLOWER when speed is BIGGER
									// when cell moves its cooldown is set to speed
									// it is decremented each iteration and biocell moves only when cooldown is 0
	int view_range;

};

// Basic thin class-parent for cell types of the colonies
// WARNING! This is an abstract class, Direction is not implemented!!!
// To create a working biocell you need either use CBacterium
// or write your own CCell heir with custom algorithm of behaviour
class CCell: public CEnvironmentArea
{

private:

	int cooldown;				// time since the cell moved
protected:
	CCellType *cell_type;

public:
	static struct CCellType default_cell_type;

	CCell(CCellType *);
	CCell();
	virtual	~CCell(){};

	virtual CEnvironmentArea *GetCopy(){ return NULL; };

	int type_id();
	int view_range();
	int default_hp();
	int Complexity();
	void SetCooldown();
	void DecCooldown();
	bool CanMove();
	bool CanDivide();

	void Dump(FILE* );
	void Print() { this->Dump(stdout); }
	virtual coord_t Direction(sur_t *) = 0;


};


//-----------------------------------------------------------------------------------------------------

class CBacterium: public CCell
{

public:
	CBacterium(CCellType *type = &CCell::default_cell_type):CCell(type){};
	virtual	~CBacterium(){};

	coord_t Direction(sur_t *);
	CEnvironmentArea *GetCopy();
};

//
// VITALY'S NOTE: Following code was written by Fedor, I didn't use it but I'll leave it as having great historical and cultural value
//
// Deals with iterations, colonies and statistics
class CExperiment
{

private:

	int elapsed_time;
	vector <int> colon_pop;   // colonies population statistic

	const vector <int> 	  COLONY_COLOR; // colors of colonies - position == cell type id
	const vector <string> COLONY_NAMES; // names  of colonies - position == cell type id

public:

	void Iteration()
	{



	}

	int ElapsedTime()
	{

		//assert(elapsed_time >= 0);
		return elapsed_time;

	}

	// statistic functions

};



