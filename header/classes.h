//author: viviaxenov
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

};

typedef vector <vector<CEnvironmentArea*> >  envmap_t;
struct CView
{

	int x;
	int y;
	AREA_TYPE type;

};

typedef vector <CView> sur_t;


class CEnvironment
{

private:
	envmap_t field;

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

	int SpawnFood();						// somehow add food on the field
	int DumbSpawnFood();
	int PlantObject(CEnvironmentArea *, int, int);	 		// puts object in certain place
	int PlantObject(CEnvironmentArea *, coord_t);
	int WipeOut(); 							// deletes every object in field
	int CleanUp(); 							// deletes all objects with 0 hp (eaten food, dead cells)
	sur_t *GetSurroundings(int, int, int);
	int CellAction(int, int);

	coord_t GetFreeAdj(int, int);
	coord_t Divide(int, int);

	int Iteration();					//will move to CExperiment later

};

//-------------------------------------------------------------------------------------------------

class CFood: public CEnvironmentArea
{

private:
	static const int DEF_FOOD_VAL;

public:
	CFood(int);
	virtual	~CFood(){};

	bool isPoison (){ if(hp < 0) return true; return false; }
	bool isFood   (){ if(hp > 0) return true; return false; }

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
protected:
	CCellType *cell_type;

public:
	static struct CCellType default_cell_type;

	CCell(CCellType *);
	CCell();
	virtual	~CCell(){};
	virtual CCell* GetCopy(){ return NULL; };

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
	CCell *GetCopy();

};



