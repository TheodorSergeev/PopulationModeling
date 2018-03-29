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

enum AREA_TYPE{EMPTY, FOOD, BIOCELL, WALL};
typedef pair <int, int> coord_t;

//-------------------------------------------------------------------------------------------------
//
//-----Application-----
//
//-------------------------------------------------------------------------------------------------


// A cell of the 2d Environment array. Either empty/cell/food/poison/...
class CEnvironmentArea
{

protected:

	AREA_TYPE area_type;

public:
	CEnvironmentArea(AREA_TYPE);
	AREA_TYPE type(){ return area_type; }

};

typedef vector <vector<CEnvironmentArea*> >  envmap_t;

class CEnvironment
{

private:

	envmap_t field;

public:
	CEnvironment(int, int);

	coord_t GetBounds();
	void DumpASCII(FILE *);
	void Print() {this->DumpASCII(stdout);}
	
	int SpawnFood();		// somehow add food on the field
	int DumbSpawnFood();
	int PlantObject(CEnvironmentArea *, int, int);
	int PlantObject(CEnvironmentArea *, coord_t);
//	const envmap_t& ViewField(int x, int y, int range)
//	
//	{
//
//
//
//	}
	// returns [x+-range][y+-range] 2d array for the cell to know its surroindings
	//
	//  |N|N|N|
	//  | |c| |
	//  | | | |
	//
	// N == NULL - out of bounds
	// c == cell, coordinates (x,y)

};


class CFood: public CEnvironmentArea
{

private:

	static const int DEF_FOOD_VAL;
	int hp_value; // food - hp_value > 0; posion - hp_value < 0

public:

	CFood(int);

	int HpValue(){ return hp_value; }

	bool isPoison (){ if(hp_value <  0) return true; return false; }
	bool isFood   (){ if(hp_value >  0) return true; return false; }
	bool isCorrect(){ if(hp_value != 0) return true; return false; }

};


// Cell "genus" type; values that are common
struct CCellType
{
	int type_id;
	string type_name;
	int default_hp;
	int speed;
	int view_range;
};

// Basic thin class-parent for cell types of the colonies
class CCell: public CEnvironmentArea
{

private:

	int hp;					// cell's current hp	
	int cooldown;				// time since the cell moved
	CCellType *cell_type;

public:
	static struct CCellType default_cell_type;

	CCell(CCellType *type);
	CCell();

	int type_id()
	{
		return this->cell_type->type_id;
	}
	
	void Dump(FILE* );
	void Print() { this->Dump(stdout); }
//	virtual coord_t Action(){} // food does not take any action

//	virtual envmap_t View() const       // Get surroundings to decide upon the action
//	{
//
//
//
//	}
//
//	virtual coord_t Action() const      // Action (movement) and impact - e.t. increae hp on food consumption
//	{
//
//        envmap_t surroundings = View();
//
//        if(surroundings[0][1]->type() != WALL)
//            return coord_t(0, 1);
//
//	}

};

// Child class - specific cell type, specific colony. Reimplement virtual functions
class CBacterium: public CCell
{  };


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


//-------------------------------------------------------------------------------------------------
//
//-----Application-----
//
//-------------------------------------------------------------------------------------------------


// Deals with window refreshment, button interaction and painting
class CApp
{

	float speed;     		// turns per second


	void RunExperiment(float speed, int time_limit)
	{

		for(int t = 0;  t <= time_limit; ++t)
		{

			// iteration
			// field visualisation
			// data collection
			// data visualisaton
			// pause check
			// save check

		}

	}

};

/*
field = Environment.GetSurroundings(); // 3x3

if(field[0][1].type == FOOD)
{

	hp += field[0][1].HpValue();

	fiels[0][1] = this;
	field[0][0] = NULL;

}*/


//To do:
// 1) Deal with synchronisation issues - which cell does it's action first in conflict situation, for example if two cells are to consume same food at the same type
// 2) Deal with speed issues
// 3) Movement
// 4)

