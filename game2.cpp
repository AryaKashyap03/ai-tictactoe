#include <iostream>
#include <climits>
using namespace std;

void printBoard(char board[9]){
    for(int i = 0; i < 9;i++){
        if(i%3 == 0 && i>0){
            cout<<endl;
            cout<<"-----------";
            cout<<endl;
        }
        if((i+1)%3==0)
        cout<<" "<<board[i]<<" ";
        else
        cout<<" "<<board[i]<<" "<<"|";
        
    }
    cout<<endl;
}

bool hasWon(char board[9], char player){
    
    if((board[0] == board[1]  && board[1] == board[2]  && board[2] == player) || (board[3] == board[4]  && board[4] == board[5]  && board[5] == player) ||
        (board[6] == board[7]  && board[7] == board[8]  && board[8] == player) ||
        (board[0] == board[3]  && board[3] == board[6]  && board[6] == player)||(board[1] == board[4]  && board[4] == board[7]  && board[7] == player)||(board[2] == board[5]  && board[5] == board[8]  && board[8] == player)||(board[0] == board[4]  && board[4] == board[8]  && board[8] == player)||(board[2] == board[4]  && board[4] == board[6]  && board[6] == player))
        {
            return true;
        }
        return false;
}

bool isDraw(char board[9]){
    
    if(hasWon(board,'X') || hasWon(board,'O'))
    return false;
    
    else{
        for(int i = 0; i < 9; i++){
            if(board[i] == ' '){
                return false;
            }
        }
    }
    return true;
}

int minimax(char board[9], bool isMaximizing, int alpha, int beta) {
    if(hasWon(board,'X')) return 1;
    if(hasWon(board,'O')) return -1;
    if (isDraw(board)) return 0;
    
    if(isMaximizing){
        int bestScore = INT_MIN;
        for(int i = 0; i < 9; i++){
            if(board[i] == ' '){
                board[i] = 'X';
                // Pass alpha and beta down the tree
                int score = minimax(board, false, alpha, beta);
                board[i] = ' ';
                
                bestScore = max(bestScore, score);
                alpha = max(alpha, bestScore); // Update alpha
                
                if(beta <= alpha) {
                    break; // Prune the branch!
                }
            }
        }
        return bestScore;
    }
    else {  // Minimizing player
        int bestScore = INT_MAX;
        for(int i = 0; i < 9; i++){
            if(board[i] == ' '){
                board[i] = 'O';
                // Pass alpha and beta down the tree
                int score = minimax(board, true, alpha, beta);
                board[i] = ' ';
                
                bestScore = min(bestScore, score);
                beta = min(beta, bestScore); // Update beta
                
                if(beta <= alpha) {
                    break; // Prune the branch!
                }
            }
        }
        return bestScore;
    }
}
int getBestMove(char board[9]){
    int bestScore = INT_MAX;
    int bestIndex = -1;
    
    for(int i = 0; i < 9; i++){
        if(board[i] == ' '){
            board[i] = 'O';
            // Start the recursion with negative and positive infinity
            int score = minimax(board, true, INT_MIN, INT_MAX);
            board[i] = ' '; 
            
            if(score < bestScore){  
                bestScore = score;
                bestIndex = i;
            }
        }
    }
    return bestIndex;
}

int main(int argc, char* argv[]){

    // 1. Safety check: ensure the user actually passed a string
    if (argc < 2) {
        cout << "Error: No board state provided." << endl;
        return 1; // Exit with an error code
    }

    string values = argv[1];
    char board[9]={' ',' ',' ',' ',' ',' ',' ',' ',' '};
    for(int i = 0; i < 9; i++){
         board[i] = values[i];
    }
    int index = getBestMove(board);
    cout<<index;
    return 0;
}