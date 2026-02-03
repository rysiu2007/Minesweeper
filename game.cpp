#include "game.h"
#include "Button.h"
UINT GameData::minefield_x_size = 8, GameData::minefield_y_size = 8, GameData::clicks = 0, GameData::mine_percentage = 10, GameData::mine_count = 0, GameData::flag_count = 0, GameData::time_100ms = 0, GameData::starting_mines_count = 10;
Button** GameData::minefield = NULL;
BOOL GameData::isPlaying = true;
HBRUSH GameData::primary_background = NULL, GameData::secondary_background = NULL, GameData::accent = NULL;
FaceState GameData::state = HAPPY;

COLORREF GameData::GetMinesweeperColor(int neighbors) {
    switch (neighbors) {
    case 1: return RGB(0, 0, 255);      // Blue
    case 2: return RGB(0, 128, 0);      // Green
    case 3: return RGB(255, 0, 0);      // Red
    case 4: return RGB(0, 0, 128);      // Dark Blue
    case 5: return RGB(128, 0, 0);      // Maroon
    case 6: return RGB(0, 128, 128);    // Teal
    case 7: return RGB(0, 0, 0);        // Black
    case 8: return RGB(128, 128, 128);  // Grey
    default: return RGB(0, 0, 0);
    }
}
void GameData::InitGame(HWND hwnd, UINT y_size, UINT x_size, UINT number_mines)
{
	if ((minefield_x_size!=x_size || minefield_y_size != y_size) && (y_size || x_size)) {
		
		for (int i = 0; i < GameData::minefield_y_size; i++)
		{
			delete[] minefield[i];
		}
		delete[] minefield;
		minefield = nullptr;
	}

	if(number_mines)
		GameData::starting_mines_count = number_mines;
	if (x_size)
		minefield_x_size = x_size;
	if (y_size)
		minefield_y_size = y_size;
	mine_count = 0;
	flag_count = 0;
	time_100ms = 0;
	clicks = 0;
	state = HAPPY;

	if (!minefield) {
		minefield = new Button * [GameData::minefield_y_size];

		for (int i = 0; i < GameData::minefield_y_size; i++)
		{
			minefield[i] = new Button[GameData::minefield_x_size]{};
		}
	}
	for (int i = 0; i < GameData::minefield_y_size; i++)
	{
		for (int j = 0; j < GameData::minefield_x_size; j++)
		{
			minefield[i][j].Init(hwnd);
			if (i >= 1)
			{
				if (j >= 1)
					minefield[i][j].neighbours_list[0] = &minefield[i - 1][j - 1];
				minefield[i][j].neighbours_list[1] = &minefield[i - 1][j];
				if (j < GameData::minefield_x_size - 1)
					minefield[i][j].neighbours_list[2] = &minefield[i - 1][j + 1];
			}
			if (j >= 1)
				minefield[i][j].neighbours_list[3] = &minefield[i][j - 1];
			if (j < GameData::minefield_x_size - 1)
				minefield[i][j].neighbours_list[4] = &minefield[i][j + 1];

			if (i < GameData::minefield_y_size - 1)
			{
				if (j >= 1)
					minefield[i][j].neighbours_list[5] = &minefield[i + 1][j - 1];
				minefield[i][j].neighbours_list[6] = &minefield[i + 1][j];
				if (j < GameData::minefield_x_size - 1)
					minefield[i][j].neighbours_list[7] = &minefield[i + 1][j + 1];
			}

		}
	}
	while (starting_mines_count > 0) {
		//mine setting pass
		for (int i = 0; i < minefield_y_size; i++)
		{
			for (int j = 0; j < minefield_x_size; j++)
			{
				if (rand() % 1000 <= mine_count && starting_mines_count > 0&& minefield[i][j].neighbours!= -1) {
					minefield[i][j].neighbours = -1;
					GameData::mine_count++;
					starting_mines_count--;

				}
			}
		}
	}
	starting_mines_count = mine_count;
	for (int i = 0; i < GameData::minefield_y_size; i++)
	{
		for (int j = 0; j < GameData::minefield_x_size; j++)
		{
			//Count mines nearby
			if (minefield[i][j].neighbours >= 0) {
				INT n = 0;
				for (int m = 0; m < 8; m++) {
					if (minefield[i][j].neighbours_list[m] &&
						minefield[i][j].neighbours_list[m]->neighbours == -1) {
						n++;
					}
				}
				minefield[i][j].neighbours = n;
			}
		}
	}
	isPlaying = true;
}
