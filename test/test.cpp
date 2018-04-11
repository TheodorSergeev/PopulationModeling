#include"../header/classes.h"

#include<stdio.h>


CCellType CCell::default_cell_type = {0, "your mom", 150, 2, 2};
const int CFood::DEF_FOOD_VAL = 1;

int main()
{
	CCellType test_type;
	test_type.type_id = 1;
	test_type.type_name = "eva 01";
	test_type.default_hp = 9000;
	test_type.speed = 15;
	test_type.view_range = 14;

	
	CCell *mom = new CBacterium();

	FILE *fp = fopen("dumps.txt", "a");
	mom->Dump(fp);

	CFood *food_p = new CFood(1);
	CFood *poison_p = new CFood(-1);
	printf("food at %08X\n", food_p);
	printf("poison at %08X\n", poison_p);
	printf("bacterium at %08X\n", mom);

	
	CEnvironment fld(2, 2);
	
	fld.PlantObject(mom, 1, 1);

	fld.PlantObject(food_p, 0, 1);
	fld.PlantObject(poison_p, 1, 0);
	fld.Print();

	//int portion = evangelion->Bite(9000);
	
	int x = 0, y = 0, rng = 3;

	sur_t *s = fld.GetSurroundings(x, y, rng);
	printf("Cell at (%2d, %2d) sees in range %d:\n", x, y, rng);
	for(int i = 0; i < s->size(); i++)
	{
		printf("%2d %2d %c\n", (*s)[i].x, (*s)[i].y,(*s)[i].type);
	}
	mom->Action(s);
	s->clear();
	delete s;
	
//	int deleted = fld.CleanUp();
	int deleted = fld.WipeOut();
	fld.Print();
//	printf("Bite sz %d\n", portion);
	printf("%d objects deleted\n\n", deleted);

	fclose(fp);	
//	delete evangelion;

	return 0;
}
