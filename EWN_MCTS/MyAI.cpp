#include "float.h"
#include "MyAI.h"

MyAI::MyAI(void){}

MyAI::~MyAI(void){}

void MyAI::Name(const char* data[], char* response)
{
	strcpy(response, "MyAI");
}

void MyAI::Version(const char* data[], char* response)
{
	strcpy(response, "1.0.0");
}

void MyAI::Time_setting(const char* data[], char* response)
{
	this->red_time = stoi(data[1]);
	this->blue_time = stoi(data[1]);
	strcpy(response, "1");
}

void MyAI::Board_setting(const char* data[], char* response)
{
	this->board_size = stoi(data[1]);
	this->red_piece_num = stoi(data[2]);
	this->blue_piece_num = stoi(data[2]);
	strcpy(response, "1");
}

void MyAI::Ini(const char* data[], char* response)
{
	// set color
	if(!strcmp(data[1], "R"))
	{
		this->color = RED;
	}
	else if(!strcmp(data[1], "B"))
	{
		this->color = BLUE;
	}

	char position[15];
	this->Init_board_state(position);

	sprintf(response, "%c%c %c%c %c%c %c%c %c%c %c%c", position[0], position[1], position[2], position[3],
													   position[4], position[5], position[6], position[7],
													   position[8], position[9], position[10], position[11]);

}

void MyAI::Get(const char* data[], char* response)
{
	// set color
	if(!strcmp(data[1], "R"))
	{
		this->color = RED;
	}
	else if(!strcmp(data[1], "B"))
	{
		this->color = BLUE;
	}

	// set dice & board
	this->dice = stoi(data[2]);
	char position[25];
	sprintf(position, "%s%s%s%s%s%s%s%s%s%s%s%s", data[3], data[4], data[5], data[6], data[7], data[8], 
												  data[9], data[10], data[11], data[12], data[13], data[14]);
	this->Set_board(position);

	// generate move
	char move[4];
	this->Generate_move(move);
	sprintf(response, "%c%c %c%c", move[0], move[1], move[2], move[3]);
}

void MyAI::Exit(const char* data[], char* response)
{
	fprintf(stderr, "Bye~\n");
}

// *********************** AI FUNCTION *********************** //

void MyAI::Init_board_state(char* position)
{
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

void MyAI::Set_board(char* position)
{
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

void MyAI::Print_chessboard()
{
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

void MyAI::Generate_move(char* move)
{
	Node* root = new Node(nullptr, -1, -1, -1, this->color);
    int result[100];
    int moveCount = this->get_legal_move(result);
    Expand(root, result, moveCount, this->color);

    for (int i = 0; i < 5000; i++) { // Perform 1000 simulations
        Node* selectedNode = Select(root);
        if (selectedNode->children.size() == 1) {
            // If only one child after expansion, skip simulation
            selectedNode = selectedNode->children[0];
        } else if (selectedNode->visits == 0) {
            int simResult = Simulate(selectedNode, *this);
            Backpropagate(selectedNode, simResult);
        } else {
            Expand(selectedNode, result, moveCount, 1 - selectedNode->player);
        }
    }

    Node* bestChild = nullptr;
    double bestScore = -DBL_MAX;
    for (Node* child : root->children) {
        double score = child->wins / (child->visits + 1e-6);
        if (score > bestScore) {
            bestScore = score;
            bestChild = child;
        }
    }

    sprintf(move, "%c%c%c%c", 'A' + bestChild->move[1] % BOARD_SIZE, 
            '1' + bestChild->move[1] / BOARD_SIZE, 
            'A' + bestChild->move[2] % BOARD_SIZE, 
            '1' + bestChild->move[2] / BOARD_SIZE);

    delete root;
    this->Make_move(bestChild->move[0], bestChild->move[1], bestChild->move[2]);
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
				std::cerr << "[DEBUG] Legal Move: Piece=" << result[i * 3]
                          << ", Start=" << result[i * 3 + 1]
                          << ", End=" << result[i * 3 + 2] << "\n";
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

int MyAI::get_legal_move(int* result, int currentPlayer)
{
    int src, dst[3];
    int movable_piece;
    int move_count = 0;
    int result_count = 0;

    if (currentPlayer == BLUE) // Generate moves for BLUE
    {
        for (int i = 1; i <= PIECE_NUM; ++i)
        {
            if (this->blue_exist[i - 1]) // If the piece is alive
            {
                movable_piece = i;
                move_count = this->referee(movable_piece, &src, dst);
                for (int j = 0; j < move_count; j++)
                {
                    result[result_count * 3] = movable_piece;
                    result[result_count * 3 + 1] = src;
                    result[result_count * 3 + 2] = dst[j];
                    result_count++;
                }
            }
        }
    }
    else if (currentPlayer == RED) // Generate moves for RED
    {
        for (int i = 1; i <= PIECE_NUM; ++i)
        {
            if (this->red_exist[i - 1]) // If the piece is alive
            {
                movable_piece = i + PIECE_NUM;
                move_count = this->referee(movable_piece, &src, dst);
                for (int j = 0; j < move_count; j++)
                {
                    result[result_count * 3] = movable_piece;
                    result[result_count * 3 + 1] = src;
                    result[result_count * 3 + 2] = dst[j];
                    result_count++;
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

Node* MyAI::Select(Node* root) {
//    std::cerr << "[DEBUG] Selecting best node...\n";
    Node* current = root;
    while (!current->children.empty()) {
        double bestUCB = -DBL_MAX;
        int min_visits = INT_MAX;
        Node* bestChild = nullptr;

        for (Node* child : current->children) {
            double ucb;
            if (child->visits == 0) {
                ucb = DBL_MAX; // Unvisited node has maximum priority
            } else {
                double exploitation = (current->player == this->color) 
                    ? child->wins / (child->visits + 1e-6) // Maximizer
                    : 1 - (child->wins / (child->visits + 1e-6)); // Minimizer
                double exploration = sqrt(2 * log(current->visits + 1) / (child->visits + 1e-6));
                ucb = exploitation + exploration;
            }

//            std::cerr << "[DEBUG] Node: Wins=" << child->wins << ", Visits=" << child->visits
//                      << ", UCB=" << ucb << "\n";

            if (ucb > bestUCB || (abs(ucb - bestUCB) <= 1e-6 && child->visits < min_visits)) {
                bestUCB = ucb;
                bestChild = child;
                min_visits = child->visits;
            }
        }

        current = bestChild;
        if (!current) {
//            std::cerr << "[ERROR] No valid child found during selection!\n";
            break;
        }
    }

//    std::cerr << "[DEBUG] Best node selected with visits=" << current->visits 
//              << ", wins=" << current->wins << "\n";
    return current;
}

void MyAI::Expand(Node* node, int* legalMoves, int moveCount, int currentPlayer) {
//    std::cerr << "[DEBUG] Expanding node with " << moveCount << " moves...\n";
    for (int i = 0; i < moveCount; i++) {
        Node* child = new Node(
            node,                        // Parent node
            legalMoves[i * 3],           // Piece
            legalMoves[i * 3 + 1],       // Start position
            legalMoves[i * 3 + 2],       // End position
            1 - currentPlayer            // Opponent's turn
        );
        node->children.push_back(child);

//        std::cerr << "[DEBUG] Added child node: Piece=" << legalMoves[i * 3]
//                  << ", Start=" << legalMoves[i * 3 + 1]
//                  << ", End=" << legalMoves[i * 3 + 2] << "\n";
    }
}

int MyAI::Simulate(Node* node, MyAI &ai) {
//    std::cerr << "[DEBUG] Simulating from node...\n";

    // Backup the current state
    int tempBoard[BOARD_SIZE][BOARD_SIZE];
    memcpy(tempBoard, ai.board, sizeof(ai.board));
    bool tempRedExist[PIECE_NUM], tempBlueExist[PIECE_NUM];
    memcpy(tempRedExist, ai.red_exist, sizeof(ai.red_exist));
    memcpy(tempBlueExist, ai.blue_exist, sizeof(ai.blue_exist));
    int tempRedPieces = ai.red_piece_num;
    int tempBluePieces = ai.blue_piece_num;
    
//        	ai.Print_chessboard();


//    // Reconstruct the board state from the root to the current node
//    std::vector<Node*> path;
//    Node* currentNode = node;
//    while (currentNode->parent != nullptr) {
//        path.push_back(currentNode);
//        currentNode = currentNode->parent;
//    }
//
//    std::reverse(path.begin(), path.end());
//    for (Node* step : path) {
//        ai.Make_move(step->move[0], step->move[1], step->move[2]);
//    }
    
    

    // Start the simulation
    int currentPlayer = node->player;
    while (true) {
//    	ai.Print_chessboard();
        int result[100];
        int moveCount = ai.get_legal_move(result, currentPlayer);

        // If no moves are possible, opponent wins
        if (moveCount == 0) {
//            std::cerr << "[DEBUG] No moves left for player " << currentPlayer << "\n";
            return (currentPlayer == RED) ? 1 : -1; // 1: Blue wins, -1: Red wins
        }

        // Choose a random move
//        std::cerr << "[DEBUG] have  " << moveCount << "legit moves" << "\n";

        int randMove = rand() % moveCount;
//        std::cerr << "[DEBUG] choose " <<randMove << "\n";

        int piece = result[randMove * 3];
        int start = result[randMove * 3 + 1];
        int end = result[randMove * 3 + 2];

//        std::cerr << "[DEBUG] Simulating move: Piece=" << piece
//                  << ", Start=" << start
//                  << ", End=" << end << "\n";

        ai.Make_move(piece, start, end);

        // Check win condition for Blue
        if (currentPlayer == BLUE && end == 0) {
//            std::cerr << "[DEBUG] Blue wins by occupying position 0.\n";
            // Restore the original state after simulation
		    memcpy(ai.board, tempBoard, sizeof(tempBoard));
		    memcpy(ai.red_exist, tempRedExist, sizeof(tempRedExist));
		    memcpy(ai.blue_exist, tempBlueExist, sizeof(tempBlueExist));
		    ai.red_piece_num = tempRedPieces;
		    ai.blue_piece_num = tempBluePieces;
            return 1; // Blue wins
        }

        // Check win condition for Red
        if (currentPlayer == RED && end == (BOARD_SIZE * BOARD_SIZE) - 1) {
//            std::cerr << "[DEBUG] Red wins by occupying final position.\n";
            // Restore the original state after simulation
		    memcpy(ai.board, tempBoard, sizeof(tempBoard));
		    memcpy(ai.red_exist, tempRedExist, sizeof(tempRedExist));
		    memcpy(ai.blue_exist, tempBlueExist, sizeof(tempBlueExist));
		    ai.red_piece_num = tempRedPieces;
		    ai.blue_piece_num = tempBluePieces;
			return -1; // Red wins
        }

        // Check if one side has lost all pieces
        if (ai.red_piece_num == 0) {
//            std::cerr << "[DEBUG] Red loses all pieces. Blue wins.\n";
            // Restore the original state after simulation
		    memcpy(ai.board, tempBoard, sizeof(tempBoard));
		    memcpy(ai.red_exist, tempRedExist, sizeof(tempRedExist));
		    memcpy(ai.blue_exist, tempBlueExist, sizeof(tempBlueExist));
		    ai.red_piece_num = tempRedPieces;
		    ai.blue_piece_num = tempBluePieces;
			return 1; // Red loses
        }
        if (ai.blue_piece_num == 0) {
//            std::cerr << "[DEBUG] Blue loses all pieces. Red wins.\n";
            // Restore the original state after simulation
		    memcpy(ai.board, tempBoard, sizeof(tempBoard));
		    memcpy(ai.red_exist, tempRedExist, sizeof(tempRedExist));
		    memcpy(ai.blue_exist, tempBlueExist, sizeof(tempBlueExist));
		    ai.red_piece_num = tempRedPieces;
		    ai.blue_piece_num = tempBluePieces;
			return -1; // Blue loses
        }

        // Switch to the next player
        currentPlayer = 1 - currentPlayer;
    }

    // Restore the original state after simulation
    memcpy(ai.board, tempBoard, sizeof(tempBoard));
    memcpy(ai.red_exist, tempRedExist, sizeof(tempRedExist));
    memcpy(ai.blue_exist, tempBlueExist, sizeof(tempBlueExist));
    ai.red_piece_num = tempRedPieces;
    ai.blue_piece_num = tempBluePieces;

    return 0; // Shouldn't reach here
}

void MyAI::Backpropagate(Node* node, int result) {
//    std::cerr << "[DEBUG] Backpropagating result: " << result << "\n";
    while (node != nullptr) {
        node->visits++;
        if (this->color == RED && result == -1) {
            node->wins++;
        } else if (this->color == BLUE && result == 1) {
            node->wins++;
        }
//        std::cerr << "[DEBUG] Node updated: Visits=" << node->visits
//                  << ", Wins=" << node->wins << "\n";
        node = node->parent;
    }
}

