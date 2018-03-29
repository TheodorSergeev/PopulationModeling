#include"../header/classes.h"

#include<time.h>
#include<stdio.h>
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
//-------------CFOOD--------------------------------------------------------------------------

const int DEF_FOOD_VAL = 12;

CFood::CFood(int hp_val = DEF_FOOD_VAL):CEnvironmentArea(FOOD)
{
	hp_value = hp_val;
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
	if(y >= field.size())
		return -3;
	if(x >= field[0].size())
		return -2;
	if(field[y][x] != NULL)
		return -1;
	field[y][x] = obj;
	return 0;
}

int CEnvironment::PlantObject(CEnvironmentArea *obj, coord_t coord)
{
	return this->PlantObject(obj, std::get<0>(coord), std::get<1>(coord));
}


