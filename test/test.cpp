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
	CCell *eva = new CBacterium(&test_type);

	FILE *fp = fopen("dumps.txt", "a");
	mom->Dump(fp);

	CFood *food_p = new CFood(1);
	CFood *poison_p = new CFood(-1);
//	printf("food at %08X\n", food_p);
//	printf("poison at %08X\n", poison_p);
//	printf("bacterium at %08X\n", mom);

	
	CEnvironment fld(5, 5);
	
	int x = 2, y = 1, rng = 2;
	fld.PlantObject(mom, x, y);

	fld.PlantObject(food_p, x - 1, y);
	fld.PlantObject(poison_p, 1, 0);
	fld.PlantObject(eva, 3, 2);
	fld.Print();
	//int portion = evangelion->Bite(9000);
	

	sur_t *s = fld.GetSurroundings(x, y, rng);
	printf("Cell at (%2d, %2d) sees in range %d:\n", x, y, rng);
	for(int i = 0; i < s->size(); i++)
	{
		printf("%2d %2d %c\n", (*s)[i].x, (*s)[i].y,(*s)[i].type);
	}
	coord_t dir = mom->Action(s);
	printf("YOUR MOM MOVES (%2d,%2d)\n", std::get<1>(dir), std::get<0>(dir));
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
