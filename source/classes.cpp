// Author: Vitaly Axenov
// 2018 --- \infty
#include"../header/classes.h"

#include <time.h>
#include <stdio.h>
#include <cmath>

#define Complexity_Fraction 3 	// needed in CCell::Complexity function which determines if the cell can duplicate
#define KUS_SIZE 6		// default size of bite of each cell (hp cell can take from food/other cell per turn)
const size_t TIMESTAMP_LEN = 50;
const int DEF_FOOD_VAL = 12;
const coord_t INCORRECT = coord_t(-1, -1);	// reserved-values for errors in functions that return coord_t
const coord_t NO_SPACE = coord_t(-2, -2);

CCellType CCell::default_cell_type = {0, "your mom", 5, 5, 2};
const int CFood::DEF_FOOD_VAL = 5;


//-------------Utility---------------------------------------------------------------------------

void PrintTimestamp(FILE *output)
{
	struct timespec t;                                      // making timestamp
	clock_gettime(CLOCK_REALTIME, &t);
	struct tm *tmp = localtime(&(t.tv_sec));
	char timestamp[TIMESTAMP_LEN];
	strftime(timestamp, TIMESTAMP_LEN, "%F %T", tmp);
	fprintf(output, "\n%s.%09ld\n", timestamp, t.tv_nsec);
	return;
}
//-----------------------------------------------------------------------------------------------
//-------------CEnvironmentArea------------------------------------------------------------------

CEnvironmentArea::CEnvironmentArea(AREA_TYPE type)	// constructor
{
	area_type = type;
}

int CEnvironmentArea::Bite(int dmg)			// determines how much hp can be taken from food/cell
							// decrements hp by dmg, to a minimum of zero
							// returns difference which is max(dmg, current_hp)
							// in case of poison negative val is returned
{
	if(dmg <= 0)
		return 0;
	if(hp > 0)
	{
		int old_hp = hp;
		hp = (old_hp - dmg > 0) ? old_hp - dmg : 0;
		return (old_hp - dmg > 0) ? dmg : old_hp;
	}
	else
	{
		int old_hp = hp;
		hp = (old_hp + dmg > 0) ? 0 : old_hp + dmg;
		return (-old_hp > dmg) ? -dmg : old_hp;
	}

}

int CEnvironmentArea::ChangeHP(int delta)		// changes hp by delta to a minimum of zero
{
	if(hp >= 0)
		hp = (hp + delta > 0) ? hp + delta : 0;
	else
		hp = (hp + delta < 0) ? hp + delta : 0;
	return hp;
}


int CEnvironmentArea::ResetHP()				// resets hp to cell's default hp in case of BIOCELL
							// ---//---  to zero in case of food
{
	if(area_type == BIOCELL)
		hp = ((CCell *) this)->default_hp();
	if(area_type == FOOD)
		hp = DEF_FOOD_VAL;
	return hp;
}

//------------------------------------------------------------------------------------------------
//-------------CCell------------------------------------------------------------------------------

CCell::CCell(CCellType *type):CEnvironmentArea(BIOCELL)	// constructor
{

	cell_type = type;
	hp = cell_type->default_hp;
	cooldown = 0;
}

CCell::CCell():CEnvironmentArea(BIOCELL)		// default cell constructor
{
	cell_type = &CCell::default_cell_type;
	hp = cell_type->default_hp;
	cooldown = 0;
}

void CCell::Dump(FILE* output)				// dumps cell into file & checks some errors;
							//  needed for testing
{
	PrintTimestamp(output);
	fprintf(output,	"CCell[%08X]:\n"
			"{\n"
			"\ttype_id = %d;\n"
			"\ttype_name = \"%s\";",
			this, cell_type->type_id, cell_type->type_name.c_str());
	fprintf(output, "\n\tdefault_hp = %d;", cell_type->default_hp);
	if(cell_type->default_hp < 1)
		fprintf(output, "//\t\t\tERROR: NEGATIVE default_hp !!!");

	fprintf(output, "\n\tspeed = %d;", cell_type->speed);
	if(cell_type->speed < 1)
		fprintf(output, "//\t\t\tERROR: NEGATIVE speed !!!");

	fprintf(output, "\n\tview_range = %d;", cell_type->view_range);
	if(cell_type->view_range < 1)
		fprintf(output, "//\t\t\tERROR: NEGATIVE view_range !!!");

	fprintf(output, "\n\thp = %d;", hp);
	if(hp < 1)
		fprintf(output, "//\t\t\tERROR: NEGATIVE hp !!!");

	fprintf(output, "\n\tcooldown = %d;", cooldown);
	if(cooldown < 0)
		fprintf(output, "//\t\t\tERROR: NEGATIVE cooldown !!!");
	if(cooldown >= cell_type->speed)
		fprintf(output, "//\t\t\tERROR: cooldown SHOULD NOT BE MORE THAN speed !!!");
	fputs("\n};", output);
}

// 3 following functions return cell's parameters 

int CCell::type_id()
{
	return this->cell_type->type_id;
}

int CCell::view_range()
{
	return cell_type->view_range;
}

int CCell::default_hp()
{
	return cell_type->default_hp;
}

int CCell::Complexity()				// value needed to determine if cell can duplicate.
						// Cell duplicates if hp > 2*default_hp + complexity
{
	return (cell_type->speed + cell_type->default_hp + cell_type->view_range)/Complexity_Fraction;
}

void CCell::SetCooldown()			// sets cooldown to maximum. Cooldown decrements each iteration. 
						// Cell moves only when cooldown = 0
{
	cooldown = cell_type->speed;
}

void CCell::DecCooldown()			// decrements cooldown to a minimum of zero
{
	cooldown = (cooldown - 1 > 0) ? cooldown - 1 : 0;
}

bool CCell::CanMove()
{
	if(cooldown == 0) return true;
	return false;
}

bool CCell::CanDivide()
{
	return (this->Complexity() + 2*cell_type->default_hp < hp);
}



//--------------------------------------------------------------------------------------------
//-------------CBacterium--------------------------------------------------------------------------

coord_t CBacterium::Direction(sur_t *s) // Minimal example; only move where there is more food;
{
	float ix = 0, iy = 0;
	float x = 0, y = 0;
	int num = s->size();
	for(int i = 0; i < num; i++)
	{
		if((*s)[i].type == FOOD)
		{
			ix = (float)((*s)[i].x);
			iy = (float)((*s)[i].y);
			float dist = ix + iy; 			//'manhattan' distance
			x += ix/dist/dist;
			y += iy/dist/dist;
		}
	}
	if(fabs(x) > fabs(y))
	{
		if(x > 0) return coord_t(1, 0);
		else return coord_t(-1, 0);
	}
	else
	{
		if(y > 0) return coord_t(0, 1);
		else return coord_t(0, -1);
	}
}

CEnvironmentArea *CBacterium::GetCopy()
{
	CBacterium *copy = new CBacterium(this->cell_type);
	return (CEnvironmentArea *)copy;
}
//--------------------------------------------------------------------------------------------
//-------------CFood--------------------------------------------------------------------------


CFood::CFood(int hp_val = DEF_FOOD_VAL):CEnvironmentArea(FOOD)
{
	hp = hp_val;
}

CEnvironmentArea *CFood::GetCopy()
{
	return (CEnvironmentArea *)(new CFood(hp));
}

//------------------------------------------------------------------------------------------------
//-------------CEnvironment------------------------------------------------------------------------------

const int DEF_XSZ = 10;
const int DEF_YSZ = 10;


CEnvironment::CEnvironment(int x = DEF_XSZ, int y = DEF_YSZ)
{
	if(x <= 0) x = DEF_XSZ;
	if(y <= 0) y = DEF_YSZ;

	vector<CEnvironmentArea *> row(x, NULL);
	row.shrink_to_fit();
	field = envmap_t(y, row);
	field.shrink_to_fit();
}
CEnvironment::~CEnvironment()
{
	this->WipeOut();				// frees all non-NULL pointers stored in field
}

coord_t CEnvironment::GetBounds()			// returns (X, Y) field size
{
	int Y = field.size();
	int X = field[0].size();
	coord_t res(X, Y);
	return res;
}

void CEnvironment::DumpASCII(FILE *output)	// dumps field in ASCII pseudo-graphics
						// I needed this function to test model when window app was not yet 
						// written
{

	int Y = field.size();
	int X = field[0].size();
	#define HLINE();\
	fputc('+', output);\
	for(int i = 0; i < X; i++)\
	{\
		fputs("-+", output);\
	}\
	fputc('\n', output);

	HLINE();
	for(int i = 0; i < Y; i++)
	{
		fputc('|', output);
		for(int j = 0; j < X; j++)
		{
			char c = ' ';
			AREA_TYPE t = this->What(j, i);

			switch(t)
			{
				case FOOD:
					c = 'F';
					break;
				case POISON:
					c = 'P';
					break;
				case BIOCELL:
					c = (((CCell *)field[i][j])->type_id())%10 + '0';
					break;
				case EMPTY:
					c = ' ';
					break;
				default:
					c = '!';

			}
			fputc(c, output);
			fputc('|', output);
		}
		fputc('\n', output);
		HLINE();
	}
	#undef HLINE();
}

int CEnvironment::PlantObject(CEnvironmentArea *obj, int x, int y) 	// Puts a COPY of your 
									// object to (x, y) if cell
									// is free
									// PLEASE MAKE SURE YOU
									// FREE obj pointer
									// AFTER USING 
{
	AREA_TYPE check = this->What(x, y);	
	if(check == OUT)

		return OUT_OF_FIELD;
	if(check != EMPTY)
		return OCCUPIED;
	field[y][x] = obj->GetCopy();
	return 0;
}

int CEnvironment::PlantObject(CEnvironmentArea *obj, coord_t coord)
{
	return this->PlantObject(obj, std::get<0>(coord), std::get<1>(coord));
}

int CEnvironment::DumbSpawnFood()					// food source appears in each free cell with
									// 50% chance
{
	srand(time(NULL));
	int Y = field.size();
	int X = field[0].size();
	int planted = 0;
	for(int i = 0; i < Y; i++)
	{
		for(int j = 0; j < X; j++)
		{
			if(rand()%2 && field[i][j] == NULL)
			{
				CFood *portion = new CFood();
				int err = this->PlantObject(portion, j, i);
				if(!err)
					planted++;
				else
					return err;
			}
		}
	}
	return planted;
}

int CEnvironment::WipeOut()						// frees EACH non-NULL pointer in field
									// replaces with NULL ptr ( = empty cells) 
{
	int Y = field.size();
	int X = field[0].size();
	int deleted = 0;


	for(int i = 0; i < Y; i++)
	{
		for(int j = 0; j < X; j++)
		{
			if(field[i][j] != NULL)
			{
				delete field[i][j];
				field[i][j] = NULL;
				deleted++;
			}
		}
	}
	return deleted;
}

int CEnvironment::CleanUp()						// frees & replaces with zeroes 
									// ONLY those pointer that point on
									// objects with zero hp (dead cells, eaten
									// food/poison sources)
{
	int Y = field.size();
	int X = field[0].size();
	int deleted = 0;

	for(int i = 0; i < Y; i++)
	{
		for(int j = 0; j < X; j++)
		{
			if((field[i][j] != NULL)&&((field[i][j])->isDead()))
			{
				delete field[i][j];
				field[i][j] = NULL;
				deleted++;
			}
		}
	}
	return deleted;
}

sur_t *CEnvironment::GetSurroundings(int x0, int y0, int range) 	// Scans area around cell 
									// returns array of (dx, dy, TYPE)
									// dx, dy --- relative coords of objects
									// TYPE = {FOOD, ALLY, HOSTILE}
									// objects are in view range rng
									// meaning that |dx| + |dy| <= rng
									// (so-called 'manhattan' distance)
{
	if(range <= 0)
		return NULL;
	AREA_TYPE tp0 = this->What(x0, y0);
	if(tp0 == OUT)
		return NULL;

	sur_t *res = new sur_t();
	int x, y;

	for(int i = -range; i <= range; i++)
	{
		for(int j = -range + abs(i); j <= range - abs(i); j++)
		{
			x = x0 + j;
			y = y0 + i;
			AREA_TYPE tp = this->What(x, y);
			if((tp == OUT) || (tp == EMPTY))
				continue;
			CView *v = new CView;
			v->x = j;
			v->y = i;
			v->type = tp;
			if((v->type == BIOCELL) && (tp0 == BIOCELL))
			{
				int id1 = ((CCell *) field[y0][x0])->type_id();
				int id2 = ((CCell *) field[y][x])->type_id();
				v->type = (id1 == id2) ? ALLY : HOSTILE;
			}
			if((v->type == FOOD))
			{
				if(((CFood *) field[y][x])->isPoison())
					v->type = POISON;
			}
			res->push_back(*v);
			delete v;
		}
	}
	return res;
}

bool CEnvironment::InField(int x, int y)
{
	int Y = field.size();
	int X = field[0].size();
	if((x < 0)||(y < 0)||(x >= X)||(y >= Y))
		return false;
	return true;
}

AREA_TYPE CEnvironment::What(int x, int y)					// returns type of object in (x, y)
										// returns OUT if x, y out of range,
										// EMPTY if cell is empty 
										// (fld[y][x] == NULL)
										// returns FOOD, POISON, BIOCELL else
{
	if(!this->InField(x, y))
		return OUT;
	if(field[y][x] == NULL)
		return EMPTY;
	if(field[y][x]->type() == FOOD && ((CFood *)field[y][x])->isPoison())
		return POISON;
	return field[y][x]->type();	

}

int CEnvironment::CellAction(int x, int y)					// processes action of biocell
										// CCell::Direction returns adjacent 
										// cell's relative coords:
										// (1, 0), (-1, 0), (0, 1), (0, -1)
										// if that cell is free and biocell
										// can move, it moves there
										// if there is food or other type of 
										// cell, then it bites that cell 
{
	if(this->What(x, y) != BIOCELL)
		return 0;

	CCell *curr = ((CCell *) field[y][x]);
	int rng = curr->view_range();
	sur_t *s = this->GetSurroundings(x, y, rng);
	coord_t dir = curr->Direction(s);
	s->clear();
	delete s;

	int x1 = x + std::get<0>(dir);
	int y1 = y + std::get<1>(dir);
	AREA_TYPE tp = this->What(x1, y1);

	if(tp == OUT)
	{
		return -1;					//TODO: this part)
	}
	if(curr->CanMove() && tp == EMPTY)		// cell moves
	{
		field[y1][x1] = field[y][x];
		field[y][x] = NULL;
		curr->SetCooldown();
		return 0;
	}

	if(tp == FOOD || tp == BIOCELL)
	{
		if(tp == BIOCELL)
		{
			CCell *that = (CCell *)field[y1][x1];
			if(that->type_id() == curr->type_id())
				return 1;
		}
		int delta = field[y1][x1]->Bite(KUS_SIZE);	//TODO: different value for different cells?
		field[y][x]->ChangeHP(delta);			//TODO: cells from same colony interaction?
		return 0;
	}
	return -100;
}

coord_t CEnvironment::GetFreeAdj(int x, int y)			// returns (x1, y1) coords of free cell, 
								// adjacent to (x, y); returns NO_SPACE 
								// if there is none 
{
	if(!this->InField(x, y))
		return INCORRECT;
	coord_t adj[] = { coord_t(x + 1, y),
				coord_t(x - 1, y),
				coord_t(x, y + 1),
				coord_t(x, y - 1) };
	for(int i = 0; i < 4; i++)
	{
		if(this->What(adj[i]) == EMPTY)
			return adj[i];
	}
	return NO_SPACE;
}

coord_t CEnvironment::Divide(int x, int y)			// Checks if biocell at (x, y) can divide
								// and if there is a free adjacent cell 
								// if OK, creates copy, puts it into free cell ,
								// resets parent cell's hp to default
								// else returns error values
{
	if(this->What(x, y) != BIOCELL)
		return INCORRECT;
	CCell *curr = (CCell *) field[y][x];
	if(!curr->CanDivide())
		return INCORRECT;
	coord_t pos = this->GetFreeAdj(x, y);
	if(pos == INCORRECT || pos == NO_SPACE)
		return pos;
	this->PlantObject(curr->GetCopy(), pos);	

	curr->ResetHP();
	return pos;
}

int CEnvironment::Iteration()					//will move to CExperiment later
								//TODO call cells in random order
								// basic iteration, 
{
	int Y = field.size();
	int X = field[0].size();
	for(int i = 0; i < Y; i++)
	{
		for(int j = 0; j < X; j++)
		{
			if(field[i][j] == NULL)
				continue;
			field[i][j]->Bite(1);
			if(field[i][j]->type() != BIOCELL)
				continue;
			CCell *curr = ((CCell *) field[i][j]);
			curr->DecCooldown();
			this->Divide(j, i);				//TODO cell division
		}
	}

	for(int i = 0; i < Y; i++)
	{
		for(int j = 0; j < X; j++)
		{
			this->CellAction(j, i);
		}
	}
	this->CleanUp();
	return 0;
}


CEnvironment *CEnvironment::StartCondFromFile(string path, int fld_size)
{

	if(fld_size < 1)
		throw("BAD_SIZE");
	
	FILE *fp = fopen(path.c_str(), "r");
	if(fp == NULL)
		throw("CANT_OPEN");
	int res = 0;
	
//	int X = 0, Y = 0;
//	res = fscanf("(%dx%d)\n", &X, &Y);
//	if(res != 2)
//		throw("FMT_ERR");

	CEnvironment *fld = new CEnvironment(fld_size, fld_size);

	for(int j = 0; j < 2; j++)
	{
		CCellType *col1 = new CCellType;	
		char *name = new char[50];
		
		int def_hp = 0, speed = 0, rng = 0;
		res = fscanf(fp, "%s\n", name);
		if(res != 1)
			throw("FMT_ERR");
		col1->type_name = name;
		
		res = fscanf(fp, "(%d,%d,%d)\n", &def_hp, &speed, &rng);
		if(res != 3)
			throw("FMT_ERR COLONY PARAM");
		
		if(def_hp < 1 || speed < 1 || rng < 1)
			throw("BAD_DATA COLONY PARAM");
		col1->default_hp = def_hp;
		col1->speed = speed;
		col1->view_range = rng;
		col1->type_id = j;

		CBacterium *inst = new CBacterium(col1);

		int Ncells = 0, x = -1, y = -1;
		res = fscanf(fp, "%d\n", &Ncells);
		if(res != 1)
			throw("FMT_ERR NCELLS");
		if(Ncells < 1)
			throw("BAD_DATA NCELLS");
		for(int i = 0; i < Ncells; i++)
		{
			res = fscanf(fp, "%d %d\n", &x, &y);
			if(res != 2)
				throw("FMT_ERR COORD");
			res = fld->PlantObject((CEnvironmentArea *)inst, x, y);
			if(res == OUT_OF_FIELD)
				throw("BAD_DATA COORD");
			
		}
	}
	int NFood = -1, food_val = -1;
	res = fscanf(fp, "%d%d", &NFood, &food_val);
	if(res != 2)
		throw("FMT_ERR FOOD PARAM");
	if(NFood < 1 || food_val < 1)
		throw("BAD_DATA FOOD PARAM");
	CFood *food_inst = new CFood(food_val);
	for(int i = 0; i < NFood; i++)
	{
		int x = -1, y = -1;
		res = fscanf(fp, "%d %d\n", &x, &y);
		if(res != 2)
			throw("FMT_ERR FOOD");
		res = fld->PlantObject((CEnvironmentArea *)food_inst, x, y);
		if(res == OUT_OF_FIELD)
			throw("BAD_DATA FOOD");
	}

	return fld;
		
}






