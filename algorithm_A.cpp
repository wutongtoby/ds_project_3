#include <iostream>
#include <cstdlib>
#include <limits>
#include <queue>
#include <ctime>
#include "pair.h"
#include "../include/algorithm.h"
#include "../include/board.h"
#include "../include/player.h"

#define DEPTH 3

using namespace std;

constexpr int MAX = std::numeric_limits<int>::max();
constexpr int MIN = std::numeric_limits<int>::min();

const int critical_mass[5][6] = {{2, 3, 3, 3, 3, 2},
                                 {3, 4, 4, 4, 4, 3},
                                 {3, 4, 4, 4, 4, 3},
                                 {3, 4, 4, 4, 4, 3},
                                 {2, 3, 3, 3, 3, 2}};
const int neighbor[30][4] = {{1, 6, -1, -1}, {0, 2, 7, -1}, {1, 3, 8, -1},
                             {2, 4, 9, -1}, {3, 5, 10, -1}, {4, 11, -1, -1},
                             {0, 7, 12, -1}, {1, 6, 8, 13}, {2, 7, 9, 14},
                             {3, 8, 10, 15}, {4, 9, 11, 16}, {5, 10, 17, -1},
                             {6, 13, 18, -1}, {7, 12, 14, 19}, {8, 13, 15, 20},
                             {9, 14, 16, 21}, {10, 15, 17, 22}, {11, 16, 23, -1},
                             {12, 19, 24, -1}, {13, 18, 20, 25}, {14, 19, 21, 26},
                             {15, 20, 22, 27}, {16, 21, 23, 28}, {17, 22, 29, -1},
                             {18, 25, -1, -1}, {19, 24, 26, -1}, {20, 25, 27, -1},
                             {21, 26, 28, -1}, {22, 27, 29, -1}, {23, 28, -1, -1}};
/* 00 01 02 03 04 05 
 * 06 07 08 09 10 11
 * 12 13 14 15 16 17
 * 18 19 20 21 22 23 
 * 24 25 26 27 28 29 
 */
bool first_round = true; 
int evaluation(Board &board, const char &mycolor, const char &opcolor)
{
    int final_score = 0;
    int i, j, k; // loop index
    bool critical_enemy;

/* 1. For every orb, for every enemy critical cell surrounding the orb, subtract (5 subtract
 *    the critical mass of that cell) from the value 
 * 2. In case that the orb has no critical enemy cells in its adjacent cells at all, 
 *    add 2 to the value if it is an edge cell or 3 if its is a corner cell
 * 3. In case that the orb has no critical enemy cells in its adjacent cells at all,
 *    add 2 to the value if the cell is critical
 * 4. For every orb of the player's color, add 1 to the value.
 * 5. For every continous blocks of critical cells of the player's color, add the twice the 
 *    number of cells in the block to the score.
 */
    for (i = 0; i < 5; i++) {
        for (j = 0; j < 6; j++) {
            // rule 4
            if (board.get_cell_color(i, j) == mycolor)
                final_score += board.get_orbs_num(i, j);
            else if (board.get_cell_color(i, j) == opcolor)
                final_score -= board.get_orbs_num(i, j);
            // rule 1
            for (k = 0, critical_enemy = false; k < 4; k++) {
                int neighbor_pos = neighbor[i * 6 + j][k];
                if (neighbor_pos != -1) {
                    // if surrounding cell is critical enemy
                    if (board.get_orbs_num(neighbor_pos / 6, neighbor_pos % 6) == 
                        critical_mass[neighbor_pos / 6][neighbor_pos % 6] - 1 && 
                        board.get_cell_color(neighbor_pos / 6, neighbor_pos % 6) == opcolor) {
                        final_score -= (5 - critical_mass[i][j]) * board.get_orbs_num(i, j);
                        critical_enemy = true;
                    }
                }
            }
            if (!critical_enemy && board.get_cell_color(i, j) != opcolor) {
                // rule 2
                final_score +=  (critical_mass[i][j] == 4 ? 0 : critical_mass[i][j]) * board.get_orbs_num(i, j);
                // rule 3
                if (board.get_orbs_num(i, j) + 1 == critical_mass[i][j])
                    final_score += 2 * critical_mass[i][j] - 1;
            }
            /*
            for (k = 0; k < 4; k++) {
                int neighbor_pos = neighbor[i * 6 + j][k];
                if (neighbor_pos != -1) {
                    if (board.get_orbs_num(neighbor_pos / 6, neighbor_pos % 6) == 
                        critical_mass[neighbor_pos / 6][neighbor_pos % 6] - 1 && 
                        board.get_cell_color(neighbor_pos / 6, neighbor_pos % 6) == mycolor &&
                        board.get_orbs_num(i, j) == critical_mass[i][j])
                        final_score += (critical_mass[i][j] - 1) * 2;
                }
            }
            */
        }
   }
   return final_score;
}
int minimax(Board &board, int depth, int alpha, int beta, bool maximizingPlayer, const char &mycolor, const char &opcolor) 
{
    if (depth == 0) 
        return evaluation(board, mycolor, opcolor);
    else if (board.win_the_game(Player(mycolor))) {
        if (depth == DEPTH)
            return 5001;
        else
            return 5000;
    }
    else if (board.win_the_game(Player(opcolor)))
        if (depth == DEPTH - 1)
            return -5001;
        else
            return -5000;

    if (maximizingPlayer) {
        int maxEval = MIN;
        for (int i = 0; i < 30; i++) {
            Board myboard(board);
            if (myboard.get_cell_color(i / 6, i % 6) == opcolor)
                continue;
            Player p(mycolor);
            myboard.place_orb(i / 6, i % 6, &p);
            int eval = minimax(myboard, depth - 1, alpha, beta, false, mycolor, opcolor);
            maxEval = max(maxEval, eval);
            alpha = max(alpha, eval);
            if (beta <= alpha)
                break;
        }
        return maxEval;
    }
    else {
        int minEval = MAX;
        for (int i = 0; i < 30; i++) {
            Board myboard(board);
            if (myboard.get_cell_color(i / 6, i % 6) == mycolor)
                continue;
            Player p(opcolor);
            myboard.place_orb(i / 6, i % 6, &p);
            int eval = minimax(myboard, depth - 1, alpha, beta, true, mycolor, opcolor);
            minEval = min(minEval, eval);
            beta = min(beta, eval);
            if (beta <= alpha)
                break;
        }
        return minEval;
    }
}
void algorithm_A(Board board, Player player, int index[])
{
    char mycolor = player.get_color();
    char opcolor;
    int best_score = MIN;
    int current_score;
    int evalutaion_result[5][6];
    static int round = -1;
    int current_depth;
    round += 2;

    if (mycolor == 'r')
        opcolor = 'b';
    else
        opcolor = 'r';


    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 6; j++) {
            evalutaion_result[i][j] = -8787; 
            Board myboard(board);
            // so we will absolutely not place orb on the enemy's place!!
            if (board.get_cell_color(i, j) != opcolor) {
                myboard.place_orb(i, j, &player);
                current_score = minimax(myboard, DEPTH, MIN, MAX, false, mycolor, opcolor);
                evalutaion_result[i][j] = current_score;
                if (current_score >= best_score) {
                    best_score = current_score;
                    //index[0] = i, index[1] = j;
                }          
            }  
        }
    }
    first_round = false;
    queue<PAIR> q;
    srand(time(NULL)*time(NULL));
    
    for (int i = 0; i < 5; i++) 
        for (int j = 0; j < 6; j++)
            if (evalutaion_result[i][j] == best_score)
                q.push(PAIR(i, j));
    int temp = rand() % q.size();
    for (int i = 0; i < temp && q.size() != 1; i++)
        q.pop();
    index[0] = q.front().row;
    index[1] = q.front().col; 
    /*
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 6; j++)
            cout << evalutaion_result[i][j] << ' ';
        cout << endl;
    }
    cout << "my color is " << (mycolor == 'r'? 'O' : 'X') << endl;
    */
    //cout << "depth is " << current_depth << endl;
}