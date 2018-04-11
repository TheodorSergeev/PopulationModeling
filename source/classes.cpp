#include"../header/classes.h"

#include<time.h>
#include<stdio.h>
#include<math.h>
const size_t TIMESTAMP_LEN = 50;



//-------------Utility---------------------------------------------------------------------------

void PrintTimestamp(FILE *output)
{
	struct timespec t;                                      //making timestamp
	clock_gettime(CLOCK_REALTIME, &t);
	struct tm *tmp = localtime(&(t.tv_sec));
	char timestamp[TIMESTAMP_LEN];
	strftime(timestamp, TIMESTAMP_LEN, "%F %T", tmp);
	fprintf(output, "\n%s.%09ld\n", timestamp, t.tv_nsec);
	return;
}
//-----------------------------------------------------------------------------------------------
//-------------CEnvironmentArea------------------------------------------------------------------

CEnvironmentArea::CEnvironmentArea(AREA_TYPE type)
{
	area_type = type;
}

int CEnvironmentArea::Bite(int dmg)
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

//------------------------------------------------------------------------------------------------
//-------------CCell------------------------------------------------------------------------------

CCell::CCell(CCellType *type):CEnvironmentArea(BIOCELL)
{
	
	cell_type = type;
	hp = cell_type->default_hp;
	cooldown = 0;
}

CCell::CCell():CEnvironmentArea(BIOCELL)
{
	cell_type = &CCell::default_cell_type;
	hp = cell_type->default_hp;
	cooldown = 0;
}

void CCell::Dump(FILE* output)
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

//--------------------------------------------------------------------------------------------
//-------------CBacterium--------------------------------------------------------------------------

coord_t CBacterium::Action(sur_t *s) // Minimal example; only move where there is more food;
{
	float ix = 0, iy = 0;
	float x = 0, y = 0;
	int num = s->size();
	for(int i = 0; i < num; i++)
	{
		if((*s)[i].type == FOOD)
		{
			ix = (float)(*s)[i].x;
			iy = (float)(*s)[i].y;
			x += 1.0/ix;
			y += 1.0/iy;
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
//--------------------------------------------------------------------------------------------
//-------------CFood--------------------------------------------------------------------------

const int DEF_FOOD_VAL = 12;

CFood::CFood(int hp_val = DEF_FOOD_VAL):CEnvironmentArea(FOOD)
{
	hp = hp_val;
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
	this->WipeOut();
}

coord_t CEnvironment::GetBounds()
{
	int Y = field.size();
	int X = field[0].size();	
	coord_t res(X, Y);
	return res;
}

void CEnvironment::DumpASCII(FILE *output)
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
			AREA_TYPE t = EMPTY;

			if(field[i][j] != NULL)
				t = field[i][j]->type();

			switch(t)
			{
				case FOOD:
					if(((CFood *)(field[i][j]))->isPoison()) c = 'P';
					else c = 'F';
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

int CEnvironment::PlantObject(CEnvironmentArea *obj, int x, int y)
{
	if((y >= field.size()) || (x >= field[0].size()))
		return OUT_OF_FIELD;
	if(field[y][x] != NULL)
		return OCCUPIED;
	field[y][x] = obj;
		printf("PlantObject: x = %2d y = %2d %08X ?= %08X\n", x, y, obj, field[y][x]);
	return 0;
}

int CEnvironment::PlantObject(CEnvironmentArea *obj, coord_t coord)
{
	return this->PlantObject(obj, std::get<0>(coord), std::get<1>(coord));
}

int CEnvironment::DumbSpawnFood()
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

int CEnvironment::WipeOut()
{
	int Y = field.size();
	int X = field[0].size();	
	int deleted = 0;
	
	FILE *f = fopen("WOlog.txt", "a");
	fputs("x, y, address\n", f);
	
	for(int i = 0; i < Y; i++)
	{
		for(int j = 0; j < X; j++)
		{
			fprintf(f, "%2d %2d %08X ", j, i, field[i][j]);
			if(field[i][j] != NULL)
			{
				fputs("deleting", f);
				delete field[i][j];
				field[i][j] = NULL;
				deleted++;
				fputs(" OK", f);
			}
			fputs("\n", f);
		}
	}
	fclose(f);
	return deleted;
}

int CEnvironment::CleanUp()
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

sur_t *CEnvironment::GetSurroundings(int x0, int y0, int range)
{
	if(x0 < 0 || y0 < 0)
		return NULL;

	int Y = field.size();
	int X = field[0].size();	
	if((x0 >= X) || (y0 >= Y))
		return NULL;
	
	if(range <= 0)
		return NULL;
	
	sur_t *res = new sur_t();
	int x, y;

	for(int i = -range; i <= range; i++)
	{
		for(int j = -range + abs(i); j <= range - abs(i); j++)
		{
			x = x0 + j;
			y = y0 + i;
			if((x < 0)||(y < 0)||(x >= X)||(y >= Y)||(field[y][x] == NULL))
				continue;
			CView *v = new CView;
			v->x = j;
			v->y = i;
			v->type = field[y][x]->type();
			AREA_TYPE t;
			if((v->type == BIOCELL) && (field[y0][x0] != NULL) && ((t = (field[y0][x0])->type()) == BIOCELL))
			{
				int id1 = ((CCell *) field[y0][x0])->type_id();
				int id2 = ((CCell *) field[y][x])->type_id();
				v->type = (id1 == id2) ? ALLY : HOSTILE;
			}
			if((v->type == FOOD) && (field[y0][x0] != NULL) && ((t = (field[y0][x0])->type()) == BIOCELL))
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


