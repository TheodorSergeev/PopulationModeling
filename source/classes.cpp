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









