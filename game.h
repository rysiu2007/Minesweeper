#pragma once
#include <Windows.h>

class Button;
enum FaceState
{
	HAPPY,
	EXCITED,
	DEAD,
	WINNER
};
class GameData {
public: 
	static FaceState state;
	static UINT minefield_x_size, minefield_y_size, clicks, mine_percentage, mine_count, flag_count, starting_mines_count;
	static Button** minefield;
	static BOOL isPlaying;
	static HBRUSH primary_background, secondary_background, accent;
	static UINT time_100ms;

	static COLORREF GetMinesweeperColor(int neighbors);
	static void InitGame(HWND, UINT = 0, UINT = 0, UINT = 0);
//	static bool CheckIfWon();

};