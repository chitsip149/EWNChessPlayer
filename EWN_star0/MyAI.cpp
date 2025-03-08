#include "float.h"
#include "MyAI.h"
#include <algorithm>

MyAI::MyAI(void) {}

MyAI::~MyAI(void) {}

void MyAI::Name(const char* data[], char* response) {
    strcpy(response, "MyAI");
}

void MyAI::Version(const char* data[], char* response) {
    strcpy(response, "1.0.0");
}

void MyAI::Time_setting(const char* data[], char* response) {
    this->red_time = stoi(data[1]);
    this->blue_time = stoi(data[1]);
    strcpy(response, "1");
}

void MyAI::Board_setting(const char* data[], char* response) {
    this->board_size = stoi(data[1]);
    this->red_piece_num = stoi(data[2]);
    this->blue_piece_num = stoi(data[2]);
    strcpy(response, "1");
}

void MyAI::Ini(const char* data[], char* response) {
    if (!strcmp(data[1], "R")) this->color = RED;
    else if (!strcmp(data[1], "B")) this->color = BLUE;

    char position[15];
    this->Init_board_state(position);

    sprintf(response, "%c%c %c%c %c%c %c%c %c%c %c%c", position[0], position[1], position[2], position[3],
            position[4], position[5], position[6], position[7],
            position[8], position[9], position[10], position[11]);
}

void MyAI::Get(const char* data[], char* response) {
    if (!strcmp(data[1], "R")) this->color = RED;
    else if (!strcmp(data[1], "B")) this->color = BLUE;

    this->dice = stoi(data[2]);
    char position[25];
    sprintf(position, "%s%s%s%s%s%s%s%s%s%s%s%s", data[3], data[4], data[5], data[6], data[7], data[8],
            data[9], data[10], data[11], data[12], data[13], data[14]);
    this->Set_board(position);

    char move[4];
    this->Generate_move(move);
    sprintf(response, "%c%c %c%c", move[0], move[1], move[2], move[3]);
}

void MyAI::Exit(const char* data[], char* response) {
    fprintf(stderr, "Bye~\n");
}

void MyAI::Init_board_state(char* position) {
    int order[PIECE_NUM] = {0, 1, 2, 3, 4, 5};
	string red_init_position = "A1B1C1A2B2A3";
	string blue_init_position = "E3D4E4C5D5E5";

	// assign the initial positions of your pieces in random order
	for(int i = 0; i < PIECE_NUM; i++)
	{
		int j = rand() % (PIECE_NUM - i) + i;
		int tmp = order[i];
		order[i] = order[j];
		order[j] = tmp;
	}

	for(int i = 0; i < PIECE_NUM; i++)
	{
		if(this->color == RED)
		{
			position[order[i] * 2] = red_init_position[i * 2];
			position[order[i] * 2 + 1] = red_init_position[i * 2 + 1];
		}
		else if(this->color == BLUE)
		{
			position[order[i] * 2] = blue_init_position[i * 2];
			position[order[i] * 2 + 1] = blue_init_position[i * 2 + 1];
		}
	}
}

void MyAI::Set_board(char* position) {
    memset(this->board, 0, sizeof(this->board));
	memset(this->blue_exist, 1, sizeof(this->blue_exist));
	memset(this->red_exist, 1, sizeof(this->red_exist));
	this->blue_piece_num = PIECE_NUM;
	this->red_piece_num = PIECE_NUM;

	int lost_piece_num = 0;
	for(int i = 0; i < PIECE_NUM * 2; i++)
	{
		int index = i * 2 - lost_piece_num;

		// the piece does not exist
		while(position[index] == '0')
		{
			index = i * 2 - lost_piece_num + 1;
			lost_piece_num++;
			// blue
			if(i < PIECE_NUM) 
			{
				this->blue_piece_num --;
				this->blue_exist[i] = 0;
			}
			// red
			else 
			{
				this->red_piece_num --;
				this->red_exist[i - PIECE_NUM] = 0;
			}
			i += 1;
		}
		// 1~6: blue pieces; 7~12: red pieces
		if (i < PIECE_NUM * 2)
		{
			this->board[position[index + 1] - '1'][position[index] - 'A'] = i + 1;
		}
	}
	fprintf(stderr, "\nThe current board:\n");
	this->Print_chessboard();
}

void MyAI::Print_chessboard() {
    fprintf(stderr, "\n");
	// 1~6 represent blue pieces; A~F represent red pieces
	for(int i = 0;i < BOARD_SIZE; i++)
	{
		fprintf(stderr, "<%d>   ", i + 1);
		for(int j = 0;j < BOARD_SIZE; j++)
		{
			if(this->board[i][j] <= PIECE_NUM) fprintf(stderr, "%d  ", this->board[i][j]);
			else fprintf(stderr, "%c  ", 'A' + (this->board[i][j] - 7)); 
		}
		fprintf(stderr, "\n");
	}
	fprintf(stderr, "\n     ");
	for (int i = 0;i < BOARD_SIZE; i++)
	{
		fprintf(stderr, "<%c>", 'A' + i);
	}
	fprintf(stderr, "\n\n");
	fprintf(stderr, "The number of blue pieces: %d\nThe number of red pieces: %d\n", this->blue_piece_num, this->red_piece_num);
}

void MyAI::Generate_move(char* move) {
    int best_move[3];
    double best_score;
    if (this->color==RED) best_score=-DBL_MAX;
    else best_score = DBL_MAX;

    int legal_moves[100];
    int move_count = this->get_legal_move(legal_moves);

    for (int i = 0; i < move_count; i++) {
        MyAI next_state = *this;
        next_state.Make_move(legal_moves[i * 3], legal_moves[i * 3 + 1], legal_moves[i * 3 + 2]);

        if (this->color==RED) {
        	double score = Star0_F(next_state, -DBL_MAX, DBL_MAX, 8); // Depth = 50
	        if (score > best_score) {
	            best_score = score;
	            best_move[0] = legal_moves[i * 3];
	            best_move[1] = legal_moves[i * 3 + 1];
	            best_move[2] = legal_moves[i * 3 + 2];
	        }
		}
		else {
			double score = Star0_G(next_state, -DBL_MAX, DBL_MAX, 8); // Depth = 50
	        if (score < best_score) {
	            best_score = score;
	            best_move[0] = legal_moves[i * 3];
	            best_move[1] = legal_moves[i * 3 + 1];
	            best_move[2] = legal_moves[i * 3 + 2];
	        }
		}
    }
    
    sprintf(move, "%c%c%c%c", 'A' + best_move[1] % BOARD_SIZE,
            '1' + best_move[1] / BOARD_SIZE,
            'A' + best_move[2] % BOARD_SIZE,
            '1' + best_move[2] / BOARD_SIZE);
    Make_move(best_move[0], best_move[1], best_move[2]);
}

// get all legal moves
int MyAI::get_legal_move(int* result)
{
	int src, dst[3];
	int movable_piece;
	int move_count = 0;
	int result_count = 0;

	if(this->color == BLUE)
	{
		// the corresponding piece is alive
		if(this->blue_exist[this->dice - 1])
		{
			movable_piece = this->dice;
			move_count = this->referee(movable_piece, &src, dst);
			for(int i = result_count; i < result_count + move_count; i++) 
			{
				result[i * 3] = movable_piece;
				result[i * 3 + 1] = src;
				result[i * 3 + 2] = dst[i];
//				fprintf(stderr, "Evaluating move: Piece %d from %c%c to %c%c with score %.2f\n",
//			        legal_moves[i * 3],
//			        'A' + legal_moves[i * 3 + 1] % BOARD_SIZE, '1' + legal_moves[i * 3 + 1] / BOARD_SIZE,
//			        'A' + legal_moves[i * 3 + 2] % BOARD_SIZE, '1' + legal_moves[i * 3 + 2] / BOARD_SIZE,
//			        score);

			}
			result_count += move_count;
		}
		// the corresponding piece does not exist
		else
		{
			// seeking for the next-higher piece
			for(int i = this->dice; i <= PIECE_NUM; ++i)
			{
				if(this->blue_exist[i - 1])
				{
					movable_piece = i;
					move_count = this->referee(movable_piece, &src, dst);
					int index = 0;
					for(int j = result_count; j < result_count + move_count; j++, index++) 
					{
						result[j * 3] = movable_piece;
						result[j * 3 + 1] = src;
						result[j * 3 + 2] = dst[index];
					}
					result_count += move_count;
					break;
				}
			}
			// seeking for the next-lower piece
			for(int i = this->dice; i >= 1; --i)
			{
				if(this->blue_exist[i - 1])
				{
					movable_piece = i;
					move_count = this->referee(movable_piece, &src, dst);
					int index = 0;
					for(int j = result_count; j < result_count + move_count; j++, index++) 
					{
						result[j * 3] = movable_piece;
						result[j * 3 + 1] = src;
						result[j * 3 + 2] = dst[index];
					}
					result_count += move_count;
					break;
				}
			}
		}
	}

	else if(this->color == RED)
	{
		// the corresponding piece is alive
		if(this->red_exist[this->dice - 1])
		{
			movable_piece = this->dice + PIECE_NUM;
			move_count = this->referee(movable_piece, &src, dst);
			for(int i = result_count; i < result_count + move_count; i++) 
			{
				result[i * 3] = movable_piece;
				result[i * 3 + 1] = src;
				result[i * 3 + 2] = dst[i];
			}
			result_count += move_count;
		}
		// the corresponding piece does not exist
		else
		{
			// seeking for the next-higher piece
			for(int i = this->dice; i <= PIECE_NUM; ++i)
			{
				if(this->red_exist[i - 1])
				{
					movable_piece = i + PIECE_NUM; 
					move_count = this->referee(movable_piece, &src, dst);
					int index = 0;
					for(int j = result_count; j < result_count + move_count; j++, index++) 
					{
						result[j * 3] = movable_piece;
						result[j * 3 + 1] = src;
						result[j * 3 + 2] = dst[index];
					}
					result_count += move_count;
					break;
				}
			}
			// seeking for the next-lower piece
			for(int i = this->dice; i >= 1; --i)
			{
				if(this->red_exist[i - 1])
				{
					movable_piece = i + PIECE_NUM; 
					move_count = this->referee(movable_piece, &src, dst);
					int index = 0;
					for(int j = result_count; j < result_count + move_count; j++, index++) 
					{
						result[j * 3] = movable_piece;
						result[j * 3 + 1] = src;
						result[j * 3 + 2] = dst[index];
					}
					result_count += move_count;
					break;
				}
			}			
		}		
	}
	

	return result_count;
}

// get possible moves of the piece
int MyAI::referee(int piece, int* src, int* dst)
{
	for(int i = 0; i < BOARD_SIZE; i++)
	{
		for(int j = 0; j < BOARD_SIZE; j++)
		{
			if(this->board[i][j] == piece)
			{
				*src = i * BOARD_SIZE + j;
			}
		}
	}
	// blue piece
	if(piece <= PIECE_NUM) 
	{
		// the piece is on the leftmost column
		if(*src % BOARD_SIZE == 0)
		{
			dst[0] = *src - BOARD_SIZE; // up
			return 1;
		}
		// the piece is on the uppermost row
		else if(*src < BOARD_SIZE)
		{
			dst[0] = *src - 1; // left
			return 1;
		}
		else
		{
			dst[0] = *src - 1; // left
			dst[1] = *src - BOARD_SIZE; // up
			dst[2] = *src - BOARD_SIZE - 1; // upper left
			return 3;
		}
	}

	// red piece
	else
	{
		// the piece is on the rightmost column
		if(*src % BOARD_SIZE == 4)
		{
			dst[0] = *src + BOARD_SIZE; // down
			return 1;
		}
		// the piece is on the downmost row
		else if(*src >= BOARD_SIZE * (BOARD_SIZE - 1))
		{
			dst[0] = *src + 1; // right
			return 1;
		}
		else
		{
			dst[0] = *src + 1; // right
			dst[1] = *src + BOARD_SIZE; // down
			dst[2] = *src + BOARD_SIZE + 1; // bottom right
			return 3;
		}
	}	
}

void MyAI::Make_move(const int piece, const int start_point, const int end_point)
{
	int start_row = start_point / BOARD_SIZE;
	int start_col = start_point % BOARD_SIZE;
	int end_row = end_point / BOARD_SIZE;
	int end_col = end_point % BOARD_SIZE;

	this->board[start_row][start_col] = 0;

	// there has another piece on the target sqaure
	if(this->board[end_row][end_col] > 0)
	{
		if(this->board[end_row][end_col] <= PIECE_NUM)
		{
			this->blue_exist[this->board[end_row][end_col] - 1] = 0;
			this->blue_piece_num--;
		}
		else
		{
			this->red_exist[this->board[end_row][end_col] - 7] = 0;
			this->red_piece_num--;			
		}
	}
	this->board[end_row][end_col] = piece;
}

bool MyAI::is_terminal_state(MyAI game_state) {
//    fprintf(stderr, "Checking terminal state. Red Pieces: %d, Blue Pieces: %d\n", 
//            game_state.red_piece_num, game_state.blue_piece_num);
//
//	game_state.Print_chessboard();
    // Check if a player has lost all their pieces
    if (game_state.red_piece_num == 0) {
        fprintf(stderr, "Blue wins: Red has no pieces left.\n");
        return true; // Blue wins
    }

    if (game_state.blue_piece_num == 0) {
        fprintf(stderr, "Red wins: Blue has no pieces left.\n");
        return true; // Red wins
    }

    // Check if Blue occupies the top-left corner (A1)
    if (game_state.board[0][0] > 0 && game_state.board[0][0] <= PIECE_NUM) {
        fprintf(stderr, "Blue wins: A1 occupied by Blue.\n");
        return true;
    }

    // Check if Red occupies the bottom-right corner (E5)
    if (game_state.board[BOARD_SIZE - 1][BOARD_SIZE - 1] >= PIECE_NUM + 1) {
        fprintf(stderr, "Red wins: E5 occupied by Red.\n");
        return true;
    }

    return false; // Not a terminal state
}


double MyAI::evaluate_state(MyAI game_state) {
    double score = 0.0;

    // Weight factors
    const double DISTANCE_WEIGHT = 1.5;
    const double PROBABILITY_WEIGHT = 2.0;
    const double ATTACK_WEIGHT = 2.5;
    const double DEFENSE_WEIGHT = 2.0;

    // Iterate over the board
    for (int row = 0; row < BOARD_SIZE; row++) {
        for (int col = 0; col < BOARD_SIZE; col++) {
            int piece = game_state.board[row][col];
            if (piece > 0) {
                double piece_score = 0.0;

                // Distance factor
                if (piece > PIECE_NUM) { // Red piece (maximizer)
                    double distance_to_goal = std::max(BOARD_SIZE - 1 - row, BOARD_SIZE - 1 - col);
                    piece_score -= DISTANCE_WEIGHT * distance_to_goal;
                } else { // Blue piece (minimizer)
                    double distance_to_goal = std::max(row, col);
                    piece_score += DISTANCE_WEIGHT * distance_to_goal;
                }

                // Probability factor
                int adjacent_opponents = 0;
                for (int d_row = -1; d_row <= 1; d_row++) {
                    for (int d_col = -1; d_col <= 1; d_col++) {
                        if (d_row == 0 && d_col == 0) continue; // Skip the piece itself
                        int adj_row = row + d_row;
                        int adj_col = col + d_col;
                        if (adj_row >= 0 && adj_row < BOARD_SIZE &&
                            adj_col >= 0 && adj_col < BOARD_SIZE) {
                            int adj_piece = game_state.board[adj_row][adj_col];
                            if ((piece <= PIECE_NUM && adj_piece > PIECE_NUM) || // Blue vs Red
                                (piece > PIECE_NUM && adj_piece <= PIECE_NUM && adj_piece > 0)) { // Red vs Blue
                                adjacent_opponents++;
                            }
                        }
                    }
                }
                piece_score -= PROBABILITY_WEIGHT * adjacent_opponents;

                // Add to the total score based on piece color
                if (piece > PIECE_NUM) {
                    score += ATTACK_WEIGHT * piece_score; // Red's score contribution
                } else {
                    score -= DEFENSE_WEIGHT * piece_score; // Blue's score contribution
                }
            }
        }
    }

    // Adjust the score based on the number of pieces remaining
    score += ATTACK_WEIGHT * (PIECE_NUM - game_state.blue_piece_num) * 20; // Red's advantage
    score -= DEFENSE_WEIGHT * (PIECE_NUM - game_state.red_piece_num) * 20; // Blue's advantage

    // Return the score based on the current player
    return (this->color == RED) ? score : -score; // Maximizer for Red, Minimizer for Blue
}


double MyAI::F_MAX(int piece, MyAI game_state, double alpha, double beta, int depth){
//	fprintf(stderr, "F_MAX: piece=%d, depth=%d, alpha=%.2f, beta=%.2f\n", piece, depth, alpha, beta);
//	game_state.Print_chessboard();

	if (depth ==0 || is_terminal_state(game_state)) {
        return evaluate_state(game_state); // Terminal or depth limit
    }
	
    int legal_moves[100];
    int cnt=0;
    for (int row = 0; row < BOARD_SIZE; row++) {
        for (int col = 0; col < BOARD_SIZE; col++) {
            int p = game_state.board[row][col];
            if (p==piece){ //max -> red piece
            	
				if (row<BOARD_SIZE-1){
					legal_moves[cnt*3]=p;
        			legal_moves[cnt*3+1]=row*BOARD_SIZE+col;
        			legal_moves[cnt*3+2]=(row+1)*BOARD_SIZE+col;
        			cnt++;
				}
				if (col<BOARD_SIZE-1){
					legal_moves[cnt*3]=p;
        			legal_moves[cnt*3+1]=row*BOARD_SIZE+col;
        			legal_moves[cnt*3+2]=row*BOARD_SIZE+col+1;
        			cnt++;
				}
				if (row<BOARD_SIZE-1 && col<BOARD_SIZE-1){
					legal_moves[cnt*3]=p;
        			legal_moves[cnt*3+1]=row*BOARD_SIZE+col;
        			legal_moves[cnt*3+2]=(row+1)*BOARD_SIZE+col+1;
        			cnt++;
				}
			}
        }
    }

//    if (move_count == 0) { // No legal moves
//        return evaluate_state(game_state); 
//    }
	double max_value=-DBL_MAX;
	
	for (int i = 0; i < cnt; i++) {
        MyAI next_state = game_state;
        next_state.Make_move(legal_moves[i * 3], legal_moves[i * 3 + 1], legal_moves[i * 3 + 2]);

        // Recursive call to MIN for the opponent
        double t = Star0_F(next_state, std::max(alpha, max_value), beta, depth - 1);

        if (t > max_value) {
            max_value = t;
        }
        alpha = std::max(alpha, max_value);
        if (max_value >= beta) {
            break; // Beta cut-off
        }
    }

    return max_value;
	
}

double MyAI::G_MIN(int piece, MyAI game_state, double alpha, double beta, int depth){
//	fprintf(stderr, "G_MIN: piece=%d, depth=%d, alpha=%.2f, beta=%.2f\n", piece, depth, alpha, beta);
//		game_state.Print_chessboard();

	if (depth ==0 || is_terminal_state(game_state)) {
        return evaluate_state(game_state); // Terminal or depth limit
    }

    int legal_moves[100];
    int cnt=0;
    for (int row = 0; row < BOARD_SIZE; row++) {
        for (int col = 0; col < BOARD_SIZE; col++) {
            int p = game_state.board[row][col];
            if (p==piece){ // min -> blue piece
        		if (row>0) {
        			legal_moves[cnt*3]=p;
        			legal_moves[cnt*3+1]=row*BOARD_SIZE+col;
        			legal_moves[cnt*3+2]=(row-1)*BOARD_SIZE+col;
        			cnt++;
				}
				if (col>0){
					legal_moves[cnt*3]=p;
        			legal_moves[cnt*3+1]=row*BOARD_SIZE+col;
        			legal_moves[cnt*3+2]=row*BOARD_SIZE+col-1;
        			cnt++;
				}
				if (row>0 && col>0){
					legal_moves[cnt*3]=p;
        			legal_moves[cnt*3+1]=row*BOARD_SIZE+col;
        			legal_moves[cnt*3+2]=(row-1)*BOARD_SIZE+col-1;
        			cnt++;
				}
			}
        }
    }

//    if (move_count == 0) { // No legal moves
//        return evaluate_state(game_state); 
//    }

	double min_value=DBL_MAX;
	
	for (int i = 0; i < cnt; i++) {
        MyAI next_state = game_state;
        next_state.Make_move(legal_moves[i * 3], legal_moves[i * 3 + 1], legal_moves[i * 3 + 2]);
		
		
        // Recursive call to MIN for the opponent
        double t = Star0_G(next_state, alpha, std::min(beta, min_value), depth - 1);

        if (t <min_value) {
            min_value = t;
        }
        beta = std::min(beta, min_value);
        if (min_value <= alpha) {
            break; // Alpha cut-off
        }
        
    }

    return min_value;
}

double MyAI::Star0_F(MyAI game_state, double alpha, double beta, int depth) {
//	fprintf(stderr, "Star0_F: depth=%d, alpha=%.2f, beta=%.2f\n", depth, alpha, beta);
//	game_state.Print_chessboard();

	if (depth ==0 || is_terminal_state(game_state)) {
        return evaluate_state(game_state); // Terminal or depth limit
    }
    
	double t[PIECE_NUM];
	double v_exp=0.0;
	
	for (int i=0; i<PIECE_NUM; i++){
		t[i]=-DBL_MAX;
		if (game_state.blue_exist[i]) {
			t[i] = G_MIN(i+1, game_state, alpha, beta, depth-1);
			v_exp += 1.0/6.0 * t[i];
		}
	}
	for (int i=0; i<PIECE_NUM; i++){
		if (!(game_state.blue_exist[i])){ //find nearest smaller and/or larger
			for (int j=i-1; j>=0; j--) if (game_state.blue_exist[j]){
				t[i]=std::max(t[i], t[j]);
				break;
			}
			for (int j=i+1; j<PIECE_NUM; j++) if (game_state.blue_exist[j]){
				t[i]=std::max(t[i], t[j]);
				break;
			}
			v_exp += 1.0/6.0 * t[i];
		}
	}
	return v_exp;
}

double MyAI::Star0_G(MyAI game_state, double alpha, double beta, int depth) {
//    fprintf(stderr, "Star0_G: depth=%d, alpha=%.2f, beta=%.2f\n", depth, alpha, beta);
//	game_state.Print_chessboard();
	if (depth ==0 || is_terminal_state(game_state)) {
        return evaluate_state(game_state); // Terminal or depth limit
    }
    
	double t[PIECE_NUM];
	double v_exp=0.0;
	
	for (int i=0; i<PIECE_NUM; i++){
		t[i]=DBL_MAX;
		if (game_state.red_exist[i]) {
			t[i] = F_MAX(i+1+PIECE_NUM, game_state, alpha, beta, depth-1);
			v_exp += 1.0/6.0 * t[i];
		}
	}
	for (int i=0; i<PIECE_NUM; i++){
		if (!(game_state.red_exist[i])){ //find nearest smaller and/or larger
			for (int j=i-1; j>=0; j--) if (game_state.red_exist[j]){
				t[i]=std::min(t[i], t[j]);
				break;
			}
			for (int j=i+1; j<PIECE_NUM; j++) if (game_state.red_exist[j]){
				t[i]=std::min(t[i], t[j]);
				break;
			}
			v_exp += 1.0/6.0 * t[i];
		}
	}
	return v_exp;
}


