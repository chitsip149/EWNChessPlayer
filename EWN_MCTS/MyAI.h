#ifndef MYAI_INCLUDED
#define MYAI_INCLUDED 

#include <bits/stdc++.h>

using namespace std;

#define RED 0
#define BLUE 1
#define BOARD_SIZE 5
#define PIECE_NUM 6
#define COMMAND_NUM 7

struct Node {
	Node *parent;
	vector<Node*> children;
	int visits;
	double wins;
	int move[3];
	int player;
	
	Node(Node* parent = nullptr, int movePiece = -1, int moveSrc = -1, int moveDst = -1, int player = -1)
	: parent(parent), visits(0), wins(0), player(player) {
	    move[0] = movePiece;
	    move[1] = moveSrc;
	    move[2] = moveDst;
	}

	
	~Node(){
		for (Node* child: children){
			delete child;
		}
	}
};

class MyAI  
{
	const char* commands_name[COMMAND_NUM] = {
		"name",
		"version",
		"time_setting",
		"board_setting",
		"ini",
		"get",
		"exit"
	};
public:
	MyAI(void);
	~MyAI(void);

	// commands
	void Name(const char* data[], char* response);
	void Version(const char* data[], char* response);
	void Time_setting(const char* data[], char* response);
	void Board_setting(const char* data[], char* response);
	void Ini(const char* data[], char* response);
	void Get(const char* data[], char* response);
	void Exit(const char* data[], char* response);

private:
	bool red_exist[PIECE_NUM], blue_exist[PIECE_NUM];
	int color;
	int red_time, blue_time;
	int board_size;
	int dice;
	int board[BOARD_SIZE][BOARD_SIZE];
	int red_piece_num, blue_piece_num;
	
	//MCTS helper functions
	Node* Select(Node* root);
	void Expand(Node* node, int* legalMoves, int moveCount, int player);
	int Simulate(Node *node, MyAI &ai);
	double EvaluateMove(int piece, int start, int end, int board[BOARD_SIZE][BOARD_SIZE], int player);
	void Backpropagate(Node *node, int result);

	// Board
	void Init_board_state(char* position);
	void Set_board(char* position);
	void Print_chessboard();
	void Generate_move(char* move);
	void Make_move(const int piece, const int start_point, const int end_point);
	int get_legal_move(int* result); 
	int get_legal_move(int* result, int currentPlayer);

	int referee(int piece, int* src, int* dst);
	
};



#endif
