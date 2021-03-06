// SpaceFinder.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <vector>
#include <fstream>
#include <iostream>
#include <locale>
#include <locale.h>
#include <Windows.h>
#define WIDTH 25
#define LENGTH 80
#define EMPTY_CHAR 9617
#ifndef MS_STDLIB_BUGS
#  if ( _MSC_VER || __MINGW32__ || __MSVCRT__ )
#    define MS_STDLIB_BUGS 1
#  else
#    define MS_STDLIB_BUGS 0
#  endif
#endif

#if MS_STDLIB_BUGS
#  include <io.h>
#  include <fcntl.h>
#endif

void init_locale(void)
{
#if MS_STDLIB_BUGS
	constexpr char cp_utf16le[] = ".1200";
	setlocale(LC_ALL, cp_utf16le);
	_setmode(_fileno(stdout), _O_WTEXT);
#else
	// The correct locale name may vary by OS, e.g., "en_US.utf8".
	constexpr char locale_name[] = "";
	setlocale(LC_ALL, locale_name);
	std::locale::global(std::locale(locale_name));
	std::wcin.imbue(std::locale())
		std::wcout.imbue(std::locale());
#endif
}

using namespace std;
struct coord {
	uint16_t x;
	uint16_t y;
};
void read_file(uint16_t p_x, uint16_t p_y, ifstream& file, vector<vector<wchar_t>>& characters); //Read in a file
bool is_within(uint16_t val, uint16_t lower, uint16_t upper);
void fill_space(uint16_t x_pos, uint16_t y_pos, const vector<vector<wchar_t>>& file, vector<coord>& points, vector<vector<coord>>& spaces, wchar_t empty);
bool contains_coord(const uint16_t x_pos, const uint16_t y_pos, const vector<coord>& points);
int main(int argc, char *argv[])
{ 
	init_locale(); //Magic Stackover flow code https://stackoverflow.com/questions/22950412/c-cant-get-wcout-to-print-unicode-and-leave-cout-working 
	vector<coord> points; //Points searched
	vector<vector<coord>> spaces; //Seperate spaces filled
	vector<vector<wchar_t>> file;
	string file_name;
	ifstream open;
	if (argc != 2) {
		cout << "Enter name of file...";
		cin >> file_name;
		open.open(file_name);
	}
	else {
		open.open(argv[1]);
	}
	read_file(LENGTH, WIDTH, open, file);
	wchar_t empty = EMPTY_CHAR; //Denotes character that is empty space all others are walls
	for (size_t y = 0; y < WIDTH; y++) {
		for (size_t x = 0; x < LENGTH; x++) {
			if (!contains_coord(x, y, points) && file[y][x] == empty) {
				fill_space(x, y, file, points, spaces, empty);
			}
		}
	}
	open.close();
	ofstream out("out.txt");
	int count = 0;
	for (auto row : spaces) {
		out << count << "\n\n";
		for (auto cell : row) {
			out << "(" << cell.x << "," << cell.y << "),";
		}
		out << "\n\n";
		count++;
	}
	out.close();
    return 0;
}
bool contains_coord(const uint16_t x_pos, const uint16_t y_pos, const vector<coord>& points) {
	for (auto x : points)
		if (x.x == x_pos && x.y == y_pos)
			return true;
	return false;
}
bool is_within(uint16_t val, uint16_t lower, uint16_t upper) {
	return val >= lower && val < upper;
}
void read_file(uint16_t p_x, uint16_t p_y, ifstream& file, vector<vector<wchar_t>>& characters) {
	if (file.is_open()) {
		for (size_t y = 0; y < p_y; y++) {
			vector<wchar_t> temp_chars;
			file.get(); 
			file.get();
			for (size_t x = 0; x < p_x; x++) {
				if (!file.eof()) {
					char bit1 = file.get();
					char bit2 = file.get();
					temp_chars.push_back((bit1 << 8) | (bit2 & 0xff));
				}
				else
					return;

			}
			characters.push_back(temp_chars);
			file.get();
			file.get();
		}
	}
}
void fill_space(uint16_t x_pos, uint16_t y_pos, const vector<vector<wchar_t>>& file, vector<coord>& points,vector<vector<coord>>& spaces, wchar_t empty) {
	//Searched is defined as follows
	//Checking all 8 points around the single point
	vector<coord> searching; //Points needing to be searched
	vector<coord> searched; //Points searched
	do {
		for (int off_y = -1; off_y < 2;off_y++) {
			for (int off_x = -1; off_x < 2; off_x++) {
				if (is_within(off_y + y_pos, 0, file.size()) && is_within(off_x + x_pos, 0, file[0].size()) && !contains_coord(off_x + x_pos, off_y + y_pos, searched) && !contains_coord(off_x + x_pos, off_y + y_pos, searching) && file[y_pos + off_y][x_pos + off_x] == empty) { //Making sure our current coordinate is within bounds
					searching.push_back({ (uint16_t)(x_pos + off_x), (uint16_t)(y_pos + off_y) });
				}
			}
		}
		if (searching.size() == 0) return;
		for (uint16_t s_pos = 0; s_pos < searching.size(); s_pos++) {
			if (searching[s_pos].x == x_pos && searching[s_pos].y == y_pos) {
				searched.push_back(searching[s_pos]);
				points.push_back(searching[s_pos]);
				SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { (SHORT)searched.back().x,(SHORT)searched.back().y });
				wcout << (wchar_t)9619;
				searching.erase(searching.begin() + s_pos);
				if (!searching.empty()) {
					x_pos = searching[0].x;
					y_pos = searching[0].y;
				}
				break;
			}
		}
	} while (!searching.empty());
	spaces.push_back(searched);
}