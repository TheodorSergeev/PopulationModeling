#include "../header/classes.h"

#include<stdio.h>
#include<string>

CCellType CCell::default_cell_type = {0, "your mom", 5, 5, 2};
const int CFood::DEF_FOOD_VAL = 5;

int main()
{
	CEnvironment *fld;
	try
	{
		fld = CEnvironment::StartCondFromFile("./config", 5);
	}catch(const char* err)
	{
		puts(err);
		return -1;
	}
	fld->Print(); 
	return 0;
}

