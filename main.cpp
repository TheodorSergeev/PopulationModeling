#include "winapp.h"

IMPLEMENT_APP(CWinApplication)


CEnvironment* SetInitialConditions()
{

    CCellType test_type;
	test_type.type_id    = 1;
	test_type.type_name  = "eva 01";
	test_type.default_hp = 9;
	test_type.speed      = 3;
	test_type.view_range = 3;

	CCell *mom = new CBacterium();
	CCell *eva = new CBacterium(&test_type);
    CFood *food_p2 = new CFood(100);

    CEnvironment* experiment = new CEnvironment(CELLS_NUM_X, CELLS_NUM_Y);
	int x = 2, y = 1, rng = 2;
	experiment->PlantObject(mom, x, y);
	experiment->PlantObject(food_p2, x, y - 1);

    return experiment;

}

