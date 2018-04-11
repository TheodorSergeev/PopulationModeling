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
	FOOD = 'F', 
	POISON = 'P',
	BIOCELL = 'B', 
	WALL,
	ALLY = 'A',
	HOSTILE = 'H'
};
typedef pair <int, int> coord_t;

enum ERRORS
{
	OUT_OF_FIELD = -100,
	OCCUPIED,
	NEGATIVE,
	BAD_RANGE
};

//-------------------------------------------------------------------------------------------------
//
//-----Application-----
//
//-------------------------------------------------------------------------------------------------


// A cell of the 2d Environment array. Either empty/cell/food/poison/...
class CEnvironmentArea
{
private:
	AREA_TYPE area_type;
protected:
	int hp; // food - hp_value > 0; posion - hp_value < 0
public:
	CEnvironmentArea(AREA_TYPE);
	~CEnvironmentArea(){};
	bool isDead(){ if(hp == 0) return true; return false; }
	AREA_TYPE type(){ return area_type; }
	int Bite(int);

};
//-------------------------------------------------------------------------------------------------
typedef vector <vector<CEnvironmentArea*> >  envmap_t;
struct CView
{
	int x;
	int y;
	AREA_TYPE type;
};

typedef vector<CView> sur_t;
//-------------------------------------------------------------------------------------------------

class CEnvironment
{

private:

	envmap_t field;

public:
	CEnvironment(int, int);
	~CEnvironment();

	coord_t GetBounds();
	void DumpASCII(FILE *);
	void Print() {this->DumpASCII(stdout);}
	
	int SpawnFood();						// somehow add food on the field
	int DumbSpawnFood();
	int PlantObject(CEnvironmentArea *, int, int);	 		// puts object in certain place
	int PlantObject(CEnvironmentArea *, coord_t);
	int WipeOut(); 							// deletes every object in field
	int CleanUp(); 							// deletes all objects with 0 hp (eaten food, dead cells)
	sur_t *GetSurroundings(int, int, int);
};

//-------------------------------------------------------------------------------------------------

class CFood: public CEnvironmentArea
{

private:

	static const int DEF_FOOD_VAL;

public:

	CFood(int);
	~CFood(){};

	int HpValue(){ return hp; }

	bool isPoison (){ if(hp <  0) return true; return false; }
	bool isFood   (){ if(hp >  0) return true; return false; }

};
//---------------------------------------------------------------------------------------------------

// Cell "species" type; values that are common for all instances
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

	int cooldown;				// time since the cell moved
	CCellType *cell_type;

public:
	static struct CCellType default_cell_type;

	CCell(CCellType *);
	CCell();
	~CCell(){};

	int type_id()
	{
		return this->cell_type->type_id;
	}
	
	void Dump(FILE* );
	void Print() { this->Dump(stdout); }
	virtual coord_t Action(sur_t *) = 0; 


};
//-----------------------------------------------------------------------------------------------------

class CBacterium: public CCell
{  
public:
	CBacterium(CCellType *type = &CCell::default_cell_type):CCell(type){};
	~CBacterium(){};
	
	coord_t Action(sur_t *);
};


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

