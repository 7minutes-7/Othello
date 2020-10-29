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

void setState(int x, int y, State s) {  //state �ٲ��ְ� �׸��� �ٲ��ְ�
	switch (s) {
		case State::BLANK: board[y][x]->setImage("Images/blank.png"); break;
		case State::POSSIBLE: board[y][x]->setImage(turn==Turn::BLACK?"Images/black possible.png": "Images/white possible.png"); break;
		case State::BLACK: board[y][x]->setImage("Images/black.png"); break;
		case State::WHITE: board[y][x]->setImage("Images/white.png"); break;
	}
	state[y][x] = s;
}
bool checkPossible(int x, int y, int dx, int dy) { //dx,dy ������ ���󰡸鼭 Ȯ��
	State self = turn == Turn::BLACK ? State::BLACK : State::WHITE;
	State other=turn == Turn::BLACK ? State::WHITE : State::BLACK;

	bool possible = false;
	for (x += dx, y += dy; x >= 0 && x < 8 && y >= 0 && y < 8; x += dx, y += dy) {
		if (state[y][x] == other) {
			possible = true;
		}
		else if (state[y][x] == self) { //�ڱ� �Ű� ������ ��: ó�� üũ�ߴ��� �ȵǸ� �ٷ� false, �������� true
			return possible;
		}
		else { //��ĭ
			return false;
		}
	}

	return false;
}

bool checkPossible(int x, int y) { //x,y��ġ�� ���ؼ� stone�� ���� �� �ִ���
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

	if (state[y][x] == State::BLACK || state[y][x] == State::WHITE) return false; //���� ������ ������ �Ұ�
	setState(x, y, State::BLANK);  //���� �� ������ ������ �ϴ� BLANK�� �ٲ�

	bool possible = false;
	for (auto d : delta) {
		if (checkPossible(x, y, d[0], d[1])) possible = true;  //���⺰�� Ȯ�� �� �����ϸ� possible==true 
	}
	return possible; //8���� �߿� �ϳ��� �����ϸ� POSSIBLE�� �ٲ��ֱ�
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
		else if (state[y][x] == self) { //�ڱ� �Ű� ������ ��: ó�� üũ�ߴ��� �ȵǸ� �ٷ� false, �������� true
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
		else { //��ĭ
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
	// 8*8 ��� ��ġ�� ���ؼ�
	// ������ �� ������ ���¸� POSSIBLE�� �ٲ۴�.
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
	for (int j = 0; j < 8; j++) { //POSSIBLE ���� Ȯ���ϸ鼭 ���� ���� �� ã��
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
		for (int x = 0; x < 8; x++) { //blank�� ���� �� ����ڰ� ���� ������ �̹��� �ٲ��ֱ�
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


					if (!setPossible()) {  //��ǻ�� ���� �� �� ���� ���� ���ʷ� �ٲ���
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


							if (!setPossible()) { //���� ���˵� �� �� ������ ��ǻ�ͷ� �Ѿ��
								turn = turn == Turn::BLACK ? Turn::WHITE : Turn::BLACK;
								if (!setPossible()) { //��ǻ�͵� �� �� ����.
									if (black_score > white_score) showMessage("Black Wins");
									else if (black_score < white_score) showMessage("White Wins");
									else showMessage("It's a tie");
								}
								else t->set(1.0f); t->start(); //��ǻ�� �� �� ����
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