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
typedef vector < vector <CEnvironmentArea*> >  envmap_t;

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

    AREA_TYPE type(){ return area_type; }
	virtual coord_t Action() PURE;

};


class CEnvironment
{

private:

	envmap_t field;

public:

	int SpawnFood();						            // somehow add food on the field
	const envmap_t& ViewField(int x, int y, int range)
	{



	}
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

	CFood(int hp_val = DEF_FOOD_VAL)
	{

        hp_value = hp_val;
        area_type = FOOD;

	}

	virtual coord_t Action(){} // food does not take any action

	int HpValue(){ return hp_value; }

	bool isPoison (){ if(hp_value <  0) return true; return false; }
	bool isFood   (){ if(hp_value >  0) return true; return false; }
    bool isCorrect(){ if(hp_value != 0) return true; return false; }

};

// in .cpp file
// constants definitions
const int CFood::DEF_FOOD_VAL = 12;


// Basic thin class-parent for cell types of the colonies
class CCell: public CEnvironmentArea
{

private:

	int type_id;
	string type_name;

	float hp;
	float speed;
	float view_range;

public:

	CCell()
	{

        type_id    = 0;
        type_name  = "default";
        hp         = 3;
        speed      = 1;
        view_range = 1;
        area_type  = BIOCELL;

	}

    void Dump()
    {

        printf("id(%d) name(%s) hp(%g) speed(%g) view(%g)\n", type_id, type_name.c_str(), hp, speed, view_range);

    }

	virtual envmap_t View() const       // Get surroundings to decide upon the action
	{



	}

	virtual coord_t Action() const      // Action (movement) and impact - e.t. increae hp on food consumption
	{

        envmap_t surroundings = View();

        if(surroundings[0][1]->type() != WALL)
            return coord_t(0, 1);

	}

};

// Child class - specific cell type, specific colony. Reimplement virtual functions
class CBactterium: public CCell
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

int main()
{

    return 0;

}
