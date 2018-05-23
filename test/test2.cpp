#include"../header/classes.h"

#include<stdio.h>
#include<time.h>

int main(int argc, char *argv[])
{

	if(argc != 2)
		return -1;

	CEnvironment *fld;
	try
	{
		fld = CEnvironment::StartCondFromFile(argv[1], 5);
	}catch( const char *s)
	{
		puts(s);
		return -1;
	}
	fld->Print();
	srand(time(NULL));
	for(int i = 0; i < 5; i++)
	{
		fld->Iteration();
		printf("%d iterations:\n", i + 1);
		fld->Print();
	}
//	fld->DumpAllCells();
	return 0;
}
