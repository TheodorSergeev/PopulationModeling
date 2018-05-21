#include "../header/classes.h"

#include<stdio.h>
#include<string>


int main()
{
	FILE *dmp = fopen("dumps", "w");

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
	fputs("INIT:\n", dmp);
	fld->AllCellsDump(dmp);

	fld->Iteration();

	fputs("\n\nONE ITER::\n", dmp);
	fld->AllCellsDump(dmp);
	fld->Print();

	fclose(dmp);

	return 0;
}

