#include <bangtal>
#include<iostream>

using namespace bangtal;
using namespace std;
ObjectPtr board[8][8];

enum class State {
	BLANK,
	POSSIBLE,
	BLACK,
	WHITE
};
State state[8][8];

enum class Turn {
	BLACK, 
	WHITE
};
Turn turn = Turn::BLACK;

void setState(int x, int y, State s) {  //state 바꿔주고 그림도 바꿔주고
	switch (s) {
		case State::BLANK: board[y][x]->setImage("Images/blank.png"); break;
		case State::POSSIBLE: board[y][x]->setImage(turn==Turn::BLACK?"Images/black possible.png": "Images/white possible.png"); break;
		case State::BLACK: board[y][x]->setImage("Images/black.png"); break;
		case State::WHITE: board[y][x]->setImage("Images/white.png"); break;
	}
	state[y][x] = s;
}
bool checkPossible(int x, int y, int dx, int dy) { //dx,dy 방향대로 따라가면서 확인
	State self = turn == Turn::BLACK ? State::BLACK : State::WHITE;
	State other=turn == Turn::BLACK ? State::WHITE : State::BLACK;

	bool possible = false;
	for (x += dx, y += dy; x >= 0 && x < 8 && y >= 0 && y < 8; x += dx, y += dy) {
		if (state[y][x] == other) {
			possible = true;
		}
		else if (state[y][x] == self) { //자기 거가 나왔을 때: 처음 체크했더니 안되면 바로 false, 나머지는 true
			return possible;
		}
		else { //빈칸
			return false;
		}
	}

	return false;
}

bool checkPossible(int x, int y) { //x,y위치에 대해서 stone을 놓을 수 있는지
	int delta[8][2] = {
		{0,1},
		{1,1},
		{1,0},
		{1,-1},
		{0,-1},
		{-1,-1},
		{-1,0},
		{-1,1}
	};

	if (state[y][x] == State::BLACK || state[y][x] == State::WHITE) return false; //돌이 놓여져 있으면 불가
	setState(x, y, State::BLANK);  //돌이 안 놓여져 있으면 일단 BLANK로 바꿈

	bool possible = false;
	for (auto d : delta) {
		if (checkPossible(x, y, d[0], d[1])) possible = true;  //방향별로 확인 후 가능하면 possible==true 
	}
	return possible; //8방향 중에 하나라도 가능하면 POSSIBLE로 바꿔주기
}


int reverse(int x, int y, int dx, int dy, bool change) {
	State self = turn == Turn::BLACK ? State::BLACK : State::WHITE;
	State other = turn == Turn::BLACK ? State::WHITE : State::BLACK;

	int _changed = 0;
	bool possible = false;
	for (x += dx, y += dy; x >= 0 && x < 8 && y >= 0 && y < 8; x += dx, y += dy) {
		if (state[y][x] == other) {
			possible = true;
		}
		else if (state[y][x] == self) { //자기 거가 나왔을 때: 처음 체크했더니 안되면 바로 false, 나머지는 true
			if (possible) {
				for (x -= dx, y -= dy; x >= 0 && x < 8 && y >= 0 && y < 8; x -= dx, y -= dy) {
					if (state[y][x] == other) {
						if(change) setState(x, y, self);
						_changed++;
					}
					else return _changed;
				}
			}
		}
		else { //빈칸
			return 0;
		}
		
	}

}

int reverse(int x, int y,bool change) {
	int changed = 0;
	int delta[8][2] = {
		{0,1},
		{1,1},
		{1,0},
		{1,-1},
		{0,-1},
		{-1,-1},
		{-1,0},
		{-1,1}
	};

	bool possible = false;
	for (auto d : delta) {
		changed+=reverse(x, y, d[0], d[1],change);
	}
	return changed;
}


bool setPossible() { 
	// 8*8 모든 위치에 대해서
	// 놓여질 수 있으면 상태를 POSSIBLE로 바꾼다.
	bool possible=false;
	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) {
			if (checkPossible(x, y)) {
				setState(x, y, State::POSSIBLE);
				possible = true;
			}
		}
	}
	return possible;
}

void move_ai() {
	int max_x = 0, max_y = 0;
	int max_changed = 0;
	for (int j = 0; j < 8; j++) { //POSSIBLE 상태 확인하면서 가장 좋은 수 찾기
		for (int i = 0; i < 8; i++) {
			if (state[j][i] == State::POSSIBLE) {
				if (reverse(i, j, false) > max_changed) {
					max_changed = reverse(i, j, false);
					max_x = i;
					max_y = j;
				}
			}
		}
	}
	setState(max_x, max_y, State::WHITE);
	reverse(max_x, max_y, true);
	turn = Turn::BLACK;
}

int main() 
{
	setGameOption(GameOption::GAME_OPTION_ROOM_TITLE, false);
	setGameOption(GameOption::GAME_OPTION_INVENTORY_BUTTON, false);
	setGameOption(GameOption::GAME_OPTION_MESSAGE_BOX_BUTTON, false);
	auto timer_ai = Timer::create(1.0f);

	auto scene = Scene::create("", "Images/background.png");
	
	auto b_number0 = Object::create("Images/L2.png", scene, 830, 220);
	auto b_number10 = Object::create("Images/L0.png", scene,750, 220);

	auto w_number0 = Object::create("Images/L2.png", scene, 1150, 220);
	auto w_number10 = Object::create("Images/L0.png", scene, 1070, 220);

	
	int start_x = 40, start_y = 40, diff = 80;
	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) { //blank로 설정 후 사용자가 누를 때마다 이미지 바꿔주기
			board[y][x] = Object::create("Images/blank.png", scene, start_x + diff * x, start_y + diff * y); state[y][x] = State::BLANK;
			board[y][x]->setOnMouseCallback([&,x,y](ObjectPtr object, int , int , MouseAction)->bool {
				int black_score = 0, white_score = 0;

				if (turn==Turn::BLACK && state[y][x] == State::POSSIBLE) {
					setState(x, y, State::BLACK);  
					reverse(x, y,true);
					turn = Turn::WHITE;

					for (int j = 0; j < 8; j++) {
						for (int i = 0; i < 8; i++) {
							if (state[j][i] == State::BLACK) black_score++;
							if (state[j][i] == State::WHITE) white_score++;
						}
					}
					
					string filename = "Images/L" + to_string(black_score/10) + ".png";
					b_number10->setImage(filename);
					filename = "Images/L" + to_string(black_score%10) + ".png";
					b_number0->setImage(filename);
					filename = "Images/L" + to_string(white_score/10) + ".png";
					w_number10->setImage(filename);
					filename = "Images/L" + to_string(white_score % 10) + ".png";
					w_number0->setImage(filename);


					if (!setPossible()) {  //컴퓨터 차례 할 거 없음 검정 차례로 바꿔줌
						turn = turn == Turn::BLACK ? Turn::WHITE : Turn::BLACK;
						if (!setPossible()) {
							if (black_score > white_score) showMessage("Black Wins");
							else if (black_score < white_score) showMessage("White Wins");
							else showMessage("It's a tie");
						}
					}

					if (turn == Turn::WHITE) {
						timer_ai->start();
						timer_ai->setOnTimerCallback([&](TimerPtr t)->bool {
							move_ai();

							black_score = 0; white_score = 0;
							for (int j = 0; j < 8; j++) {
								for (int i = 0; i < 8; i++) {
									if (state[j][i] == State::BLACK) black_score++;
									if (state[j][i] == State::WHITE) white_score++;
								}
							}

							string filename = "Images/L" + to_string(black_score / 10) + ".png";
							b_number10->setImage(filename);
							filename = "Images/L" + to_string(black_score % 10) + ".png";
							b_number0->setImage(filename);
							filename = "Images/L" + to_string(white_score / 10) + ".png";
							w_number10->setImage(filename);
							filename = "Images/L" + to_string(white_score % 10) + ".png";
							w_number0->setImage(filename);


							if (!setPossible()) { //검정 차롄데 할 게 없으면 컴퓨터로 넘어가자
								turn = turn == Turn::BLACK ? Turn::WHITE : Turn::BLACK;
								if (!setPossible()) { //컴퓨터도 할 게 없음.
									if (black_score > white_score) showMessage("Black Wins");
									else if (black_score < white_score) showMessage("White Wins");
									else showMessage("It's a tie");
								}
								else t->set(1.0f); t->start(); //컴퓨터 할 거 있음
							}
							t->set(1.0f);

							return true;
						});

					}
				}
				return true;
			});
			
		}     
	}


	setState(3, 3, State::BLACK);
	setState(4, 4, State::BLACK);
	setState(3, 4, State::WHITE);
	setState(4, 3, State::WHITE);


	setPossible();

	startGame(scene);

}