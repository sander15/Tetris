#include <Windows.h>
#include <math.h>
#include <time.h>
#include <chrono>  
#include <iostream>
#include <map>
#include <sstream>
#include <stdlib.h>
#include <vector>
#define MATH_PI 3.14

using namespace std;

// Console Configs //

static const int CONSOLE_W = 44;
static const int CONSOLE_H = 44;
static HANDLE console_screen;
static DWORD chars_written = 0;
static char* buf_display = 0;
static map<char, vector<string>> tetromino_repo;

// Game Configs //

enum TetroType { I, O, Z, T, L, S, J, Savior };
static const int NUM_TETRO_TYPE = 8;
static const int TETRO_W = 10;
static const int TETRO_H = 4;
static const int SLEEP_TIME = 30; 
static const int SPEED_UP_NUM = 20; 

static BOOL* buf_relics = 0; 
static double game_speed = 20;
static int speed_count = 0;
static int score = 0;
static int num_pieces = 0;  
static int tetro_x = CONSOLE_W / 2 - TETRO_W / 2;
static int tetro_y = 2;
static int cur_tetro_type = 0;
static int cur_tetro_orientation = 0;
static BOOL vk_up_down = FALSE;  


void BufCleanUp() {
	if (buf_display) delete[] buf_display;
	if (buf_relics) delete[] buf_relics;
}

void HandleNormalExit() {
	BufCleanUp();
	exit(0);
}

void HandleErrorExit(string msg, DWORD err) {
	SetConsoleActiveScreenBuffer(GetStdHandle(STD_OUTPUT_HANDLE));
	cerr << msg << endl;
	cerr << "Error code: " << err << endl;
	cout << "Enter any key to exit.." << endl;
	cin.ignore();
	cin.get();

	BufCleanUp();
	exit(1);
}


void ClearRelicsBuf() {
	for (int i = 0; i < CONSOLE_W * CONSOLE_H; i++) {
		buf_relics[i] = FALSE;
	}
}


void ClearDisplay() {
	for (int y = 2; y < CONSOLE_H - 2; y++) {
		for (int x = 2; x < CONSOLE_W - 2; x++) {
			if (buf_relics[y * CONSOLE_W + x] != TRUE) {
				buf_display[y * CONSOLE_W + x] = ' ';
			}
		}
	}
}


void ClearDisplayBuf() {
	for (int i = 0; i < CONSOLE_W * CONSOLE_H; i++) {
		buf_display[i] = ' ';
	}
}


// Make all the figures
void InitTetrominoes() {
	vector<string> Is(4);
	vector<string> Os(4);
	vector<string> Zs(4);
	vector<string> Ts(4);
	vector<string> Ls(4);
	vector<string> Ss(4);
	vector<string> Js(4);
	vector<string> Saviors(4);


	string tmp = "";

	// I 0 2 
	tmp += "....\xDB\xDB....";
	tmp += "....\xDB\xDB....";
	tmp += "....\xDB\xDB....";
	tmp += "....\xDB\xDB....";
	Is[0] = tmp;
	Is[2] = tmp;
	tmp = "";

	// I 1 3
	tmp += "..........";
	tmp += ".\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB.";
	tmp += "..........";
	tmp += "..........";
	Is[1] = tmp;
	Is[3] = tmp;
	tmp = "";

	// O 0 1 2 3
	tmp += "...\xDB\xDB\xDB\xDB...";
	tmp += "...\xDB\xDB\xDB\xDB...";
	tmp += "..........";
	tmp += "..........";
	Os[0] = tmp;
	Os[1] = tmp;
	Os[2] = tmp;
	Os[3] = tmp;
	tmp = "";

	// Z 0 2 
	tmp += "..\xDB\xDB\xDB\xDB....";
	tmp += "....\xDB\xDB\xDB\xDB..";
	tmp += "..........";
	tmp += "..........";
	Zs[0] = tmp;
	Zs[2] = tmp;
	tmp = "";
	// Z 1 3 
	tmp += ".....\xDB\xDB...";
	tmp += "...\xDB\xDB\xDB\xDB...";
	tmp += "...\xDB\xDB.....";
	tmp += "..........";
	Zs[1] = tmp;
	Zs[3] = tmp;
	tmp = "";

	// T0
	tmp += "..\xDB\xDB\xDB\xDB\xDB\xDB..";
	tmp += "....\xDB\xDB....";
	tmp += "..........";
	tmp += "..........";
	Ts[0] = tmp;
	tmp = "";
	// T1
	tmp += ".....\xDB\xDB...";
	tmp += "...\xDB\xDB\xDB\xDB...";
	tmp += ".....\xDB\xDB...";
	tmp += "..........";
	Ts[1] = tmp;
	tmp = "";
	// T2
	tmp += "....\xDB\xDB....";
	tmp += "..\xDB\xDB\xDB\xDB\xDB\xDB..";
	tmp += "..........";
	tmp += "..........";
	Ts[2] = tmp;
	tmp = "";
	// T3
	tmp += "...\xDB\xDB.....";
	tmp += "...\xDB\xDB\xDB\xDB...";
	tmp += "...\xDB\xDB.....";
	tmp += "..........";
	Ts[3] = tmp;
	tmp = "";

	// L0
	tmp += "..\xDB\xDB\xDB\xDB\xDB\xDB..";
	tmp += "..\xDB\xDB......";
	tmp += "..........";
	tmp += "..........";
	Ls[0] = tmp;
	tmp = "";
	// L1
	tmp += "...\xDB\xDB\xDB\xDB...";
	tmp += ".....\xDB\xDB...";
	tmp += ".....\xDB\xDB...";
	tmp += "..........";
	Ls[1] = tmp;
	tmp = "";
	// L2
	tmp += "......\xDB\xDB..";
	tmp += "..\xDB\xDB\xDB\xDB\xDB\xDB..";
	tmp += "..........";
	tmp += "..........";
	Ls[2] = tmp;
	tmp = "";
	// L3
	tmp += "...\xDB\xDB.....";
	tmp += "...\xDB\xDB.....";
	tmp += "...\xDB\xDB\xDB\xDB...";
	tmp += "..........";
	Ls[3] = tmp;
	tmp = "";

	// S 0 2 
	tmp += "....\xDB\xDB\xDB\xDB..";
	tmp += "..\xDB\xDB\xDB\xDB....";
	tmp += "..........";
	tmp += "..........";
	Ss[0] = tmp;
	Ss[2] = tmp;
	tmp = "";
	// S 1 3 
	tmp += "...\xDB\xDB.....";
	tmp += "...\xDB\xDB\xDB\xDB...";
	tmp += ".....\xDB\xDB...";
	tmp += "..........";
	Ss[1] = tmp;
	Ss[3] = tmp;
	tmp = "";

	// J0
	tmp += "..\xDB\xDB\xDB\xDB\xDB\xDB..";
	tmp += "......\xDB\xDB..";
	tmp += "..........";
	tmp += "..........";
	Js[0] = tmp;
	tmp = "";
	// J1
	tmp += ".....\xDB\xDB...";
	tmp += ".....\xDB\xDB...";
	tmp += "...\xDB\xDB\xDB\xDB...";
	tmp += "..........";
	Js[1] = tmp;
	tmp = "";
	// J2
	tmp += "..\xDB\xDB......";
	tmp += "..\xDB\xDB\xDB\xDB\xDB\xDB..";
	tmp += "..........";
	tmp += "..........";
	Js[2] = tmp;
	tmp = "";
	// J3
	tmp += "...\xDB\xDB\xDB\xDB...";
	tmp += "...\xDB\xDB.....";
	tmp += "...\xDB\xDB.....";
	tmp += "..........";
	Js[3] = tmp;
	tmp = "";

	// Savior Blocks
	tmp += ".....\xDB....";
	tmp += "..........";
	tmp += "..........";
	tmp += "..........";
	Saviors[0] = tmp;
	Saviors[1] = tmp;
	Saviors[2] = tmp;
	Saviors[3] = tmp;
	tmp = "";


	tetromino_repo[TetroType::I] = Is;
	tetromino_repo[TetroType::O] = Os;
	tetromino_repo[TetroType::Z] = Zs;
	tetromino_repo[TetroType::T] = Ts;
	tetromino_repo[TetroType::L] = Ls;
	tetromino_repo[TetroType::S] = Ss;
	tetromino_repo[TetroType::J] = Js;
	tetromino_repo[TetroType::Savior] = Saviors;

}


void InitPlayField() {
	// Screen Settings
	string  window_title = "TetrisCMD - Sander15";
	SMALL_RECT min_window_size = { 0, 0, (short)1, (short)1 };
	SMALL_RECT window_size = { 0, 0, (short)(CONSOLE_W - 1),
		(short)(CONSOLE_H - 1) };
	buf_display = new char[CONSOLE_W * CONSOLE_H];
	buf_relics = new BOOL[CONSOLE_W * CONSOLE_H];
	for (int i = 0; i < CONSOLE_W * CONSOLE_H; i++) {
		buf_display[i] = ' ';
		buf_relics[i] = FALSE;
	}

	InitTetrominoes();


	console_screen = CreateConsoleScreenBuffer(
		GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleTitleA(window_title.c_str());

	if (!SetConsoleWindowInfo(console_screen, TRUE, &min_window_size))
		HandleErrorExit("Failed resizing window(1st)", GetLastError());
	if (!SetConsoleScreenBufferSize(console_screen,
		{ (short)CONSOLE_W, (short)CONSOLE_H }))
		HandleErrorExit("Failed resizing screen buffer", GetLastError());
	if (!SetConsoleWindowInfo(console_screen, TRUE, &window_size))
		HandleErrorExit("Failed resizing window(2nd)", GetLastError());


	HWND console_window = FindWindowA(NULL, window_title.c_str());
	SetWindowPos(console_window, HWND_TOPMOST, 10, 10, 0, 0, SWP_NOSIZE);
	SetWindowLong(console_window, GWL_STYLE,
		GetWindowLong(console_window, GWL_STYLE) & ~WS_MAXIMIZEBOX &
		~WS_SIZEBOX);
	CONSOLE_CURSOR_INFO cci;
	GetConsoleCursorInfo(console_screen, &cci);
	cci.bVisible = FALSE;
	SetConsoleCursorInfo(console_screen, &cci);
	SetConsoleActiveScreenBuffer(console_screen);
}

void DrawBoundaries() {
	for (int y = 0; y < CONSOLE_H; y++) {
		for (int x = 0; x < CONSOLE_W; x++) {
			if (x > 0 && x < CONSOLE_W - 1 && y > 0 && y < CONSOLE_H - 1) {
				if (x == 1 || x == CONSOLE_W - 2)
					buf_display[y * CONSOLE_W + x] = 186;
				if (y == CONSOLE_H - 2)
					buf_display[y * CONSOLE_W + x] = 207;
				if (y == 1)
					buf_display[y * CONSOLE_W + x] = '_';
			}
		}
	}
	buf_display[1 * CONSOLE_W + 1] = '_';
	buf_display[1 * CONSOLE_W + (CONSOLE_W - 2)] = '_';
	buf_display[(CONSOLE_H - 2) * CONSOLE_W + 1] = 200;
	buf_display[(CONSOLE_H - 2) * CONSOLE_W + (CONSOLE_W - 2)] = 188;
}

void ShowMsg(stringstream const& t, int pos = (CONSOLE_H - 1) * CONSOLE_W + 5) {
	memcpy(&buf_display[pos], t.str().c_str(), t.str().length());
}

// Draw the falling tetromino
void DrawTetromino() {
	string tetromino = tetromino_repo[cur_tetro_type][cur_tetro_orientation];
	for (int y = 0; y < TETRO_H; y++) {
		for (int x = 0; x < TETRO_W; x++) {
			char pixel = tetromino[y * TETRO_W + x];
			if (pixel != '.') {
				buf_display[(tetro_y + y) * CONSOLE_W + (tetro_x + x)] = pixel;
			}
		}
	}
}

BOOL NoCollision(int dx, int dy, int dr) {
	int cur_x = tetro_x + dx;
	int cur_y = tetro_y + dy;
	int cur_orit = cur_tetro_orientation + dr;
	cur_orit = cur_orit >= 4 ? 0 : cur_orit;

	for (int y = 0; y < TETRO_H; y++) {
		for (int x = 0; x < TETRO_W; x++) {
			string tetro = tetromino_repo[cur_tetro_type][cur_orit];
			if (tetro[y * TETRO_W + x] == '\xDB')
				if (buf_display[(cur_y + y) * CONSOLE_W + (cur_x + x)] != ' ') {
					return FALSE;
				}
		}
	}
	return TRUE;
}

void HandleKeyPress() {
	if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
		HandleNormalExit();
	}
	if (GetAsyncKeyState(VK_LEFT) & 0x8000 && NoCollision(-1, 0, 0)) {
		tetro_x -= 1;
		return;
	}
	if (GetAsyncKeyState(VK_RIGHT) & 0x8000 && NoCollision(1, 0, 0)) {
		tetro_x += 1;
		return;
	}
	if (GetAsyncKeyState(VK_DOWN) & 0x8000 && NoCollision(0, 1, 0)) {
		tetro_y += 1;
		return;
	}
	if (GetAsyncKeyState(VK_UP) & 0x8000 && NoCollision(0, 0, 1) && !vk_up_down) {
		vk_up_down = TRUE;
		cur_tetro_orientation++;
		if (cur_tetro_orientation == 4) {
			cur_tetro_orientation = 0;
		}
		return;
	}
	else if (!(GetAsyncKeyState(VK_UP) & 0x8000)) {
		vk_up_down = FALSE;
	}
}

void UpdateTopInfo() {
	stringstream msg_score;
	msg_score << "Score: " << score;
	msg_score << "      ";
	msg_score << "Pieces: " << num_pieces;
	msg_score << "      ";
	int gs = 41 - game_speed / 0.5;
	msg_score << "Speed: " << gs << "  ";
	ShowMsg(msg_score, 2);
}


void UpdateBottomInfo() {
	stringstream msg_score;
	msg_score << "Press [ESC] to exit.";
	ShowMsg(msg_score, (CONSOLE_H - 1) * CONSOLE_W + 2);
}

void RenderDidplay() {
	WriteConsoleOutputCharacterA(console_screen, buf_display,
		CONSOLE_W * CONSOLE_H, COORD({ 0, 0 }),
		&chars_written);
}

void GenerateNewTetromino() {
	tetro_x = CONSOLE_W / 2 - TETRO_W / 2;
	tetro_y = 2;
	srand(time(NULL));
	cur_tetro_type = rand() % NUM_TETRO_TYPE;
	cur_tetro_orientation = rand() % 4;
	num_pieces++;
}

void GameOver() {
	const int PROMPT_W = CONSOLE_W / 2 + 12;
	const int PROMPT_H = 10;
	const int PROMPT_POS_X = CONSOLE_W / 2 - PROMPT_W / 2;
	const int PROMPT_POS_Y = CONSOLE_H / 2 - PROMPT_H / 2;

	string t(PROMPT_H * PROMPT_W, ' ');
	for (int y = 0; y < PROMPT_H; y++) {
		for (int x = 0; x < PROMPT_W; x++) {
			if (x == 0 || x == PROMPT_W - 1 || y == 0 || y == PROMPT_H - 1)
				t[y * PROMPT_W + x] = 178;
			else
				t[y * PROMPT_W + x] = ' ';
		}
	}
	stringstream str1;
	str1 << "YOU LOST! - GAME OVER!";
	stringstream str2;
	str2 << "Your Score: ";
	str2 << score;
	stringstream str3;
	str3 << "Press SPACE to continue...";


	t.replace(2 * PROMPT_W + (PROMPT_W / 2 - str1.str().length() / 2),
		str1.str().length(), str1.str());
	t.replace(5 * PROMPT_W + 3, str2.str().length(), str2.str());
	t.replace(7 * PROMPT_W + 3, str3.str().length(), str3.str());


	const char* msg = t.c_str();
	for (int y = 0; y < PROMPT_H; y++) {
		int pos = (y + PROMPT_POS_Y) * CONSOLE_W + PROMPT_POS_X;
		memcpy(&buf_display[pos], &msg[y * PROMPT_W], PROMPT_W);
	}
	RenderDidplay();

	while (1) {
		if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
			HandleNormalExit();
		if ((GetAsyncKeyState(VK_SPACE) & 0x8000) || (GetAsyncKeyState(VK_RETURN) & 0x8000))
			return;
	}
}

void SaveTetrominoRelics() {
	for (int y = 0; y < TETRO_H; y++) {
		for (int x = 0; x < TETRO_W; x++) {
			string tetro = tetromino_repo[cur_tetro_type][cur_tetro_orientation];
			if (tetro[y * TETRO_W + x] == '\xDB') {
				buf_relics[(tetro_y + y) * CONSOLE_W + (tetro_x + x)] = TRUE;
				buf_display[(tetro_y + y) * CONSOLE_W + (tetro_x + x)] = '\xDB';
			}
		}
	}
}

void RelicsRowClearAnimation(int y) {
	for (int i = 0; i < 3; i++) {
		for (int x = CONSOLE_W - 3; x > 1; x--) {
			buf_display[y * CONSOLE_W + x] = ' ';
		}
		RenderDidplay();
		Sleep(10);
		for (int x = CONSOLE_W - 3; x > 1; x--) {
			buf_display[y * CONSOLE_W + x] = '\xDB';
		}
		RenderDidplay();
		Sleep(10);
	}
}

void RelicsRowShift(int row) {
	for (int y = row; y > 1; y--) {
		for (int x = CONSOLE_W - 3; x > 1; x--) {
			if (y == 2) {  
				buf_relics[y * CONSOLE_W + x] = FALSE;
				buf_display[y * CONSOLE_W + x] = ' ';
			}
			else {
				buf_relics[y * CONSOLE_W + x] =
					buf_relics[(y - 1) * CONSOLE_W + x];
				buf_display[y * CONSOLE_W + x] =
					buf_display[(y - 1) * CONSOLE_W + x];
			}
		}
	}
}


void TryClearFullLineRelics() {
	for (int y = CONSOLE_H - 3; y > 1; y--) {
		BOOL is_full = TRUE;
		for (int x = CONSOLE_W - 3; x > 1; x--) {
			is_full &= buf_relics[y * CONSOLE_W + x];
		}
		if (is_full) {
			RelicsRowClearAnimation(y);
			RelicsRowShift(y);
			score += int(25 - game_speed);
			y++;  
		}
	}
}


void GameSpeedAdjuster() {
	if (num_pieces % SPEED_UP_NUM == 0)
		game_speed = game_speed - 0.5 > 2 ? game_speed - 0.5 : 2;
}


void TryLowerTetromino() {
	speed_count++;
	if (speed_count >= game_speed) {
		speed_count = 0;
		if (!NoCollision(0, 1, 0)) {  
			SaveTetrominoRelics();
			TryClearFullLineRelics();
			GenerateNewTetromino();
			GameSpeedAdjuster();

		}
		else {  
			tetro_y++;
		}
	}
}

void InitGameStat() {
	score = 0;
	game_speed = 20;
	num_pieces = 0;
	ClearRelicsBuf();
	ClearDisplay();
	GenerateNewTetromino();  
	DrawBoundaries();
}



void SplashScreen() {
	int dx = 2;
	int dy = 1;

	stringstream m;
	for (int i = 0; i < 5; i++) {
		m << "                                            ";
	}
	m << "     _____          _            _          ";
	m << "    |_   _|        | |          (_)         ";
	m << "      | |     ___  | |_   _ __   _   ___    ";
	m << "      | |    / _ \\ | __| | ___| | | / __\\    ";
	m << "     | |   |  __/ | |_  | |    | | \\__ |   ";
	m << "      \\_/    \\___|  \\__| |_|    |_| |___/  ";
	for (int i = 0; i < 25; i++) {
		m << "                                            ";
	}
	m << "        Press SPACE to start...          ";


	
	while (1) {
		memcpy(buf_display, m.str().c_str(), m.str().length());

		
		RenderDidplay();
		ClearDisplayBuf();
		if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
			HandleNormalExit();
		if ((GetAsyncKeyState(VK_SPACE) & 0x8000) || (GetAsyncKeyState(VK_RETURN) & 0x8000))
			break;
		Sleep(150);
	}
}

void StartGameLoop() {
	InitGameStat();
	while (1) {
		UpdateTopInfo();
		UpdateBottomInfo();
		
		if (!NoCollision(0, 0, 0)) {
			DrawTetromino();
			RenderDidplay();
			GameOver();
			InitGameStat();
			continue;
		}
		HandleKeyPress();
		DrawTetromino();
		RenderDidplay();

		ClearDisplay();
		TryLowerTetromino();

		Sleep(SLEEP_TIME);
	}
}

int main(int argc, char* argv[]) {
	InitPlayField();
	SplashScreen();
	StartGameLoop();
	return 0;
}
