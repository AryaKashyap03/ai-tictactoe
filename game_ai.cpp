#include <iostream>
#include <climits>
#include <vector>
#include <cmath>
#include <unordered_map>
#include <random>
using namespace std;


// Transposition Table Entry
struct TTEntry {
    int depth;
    int score;
    int flag; // 0 = Exact, 1 = Lowerbound (Alpha), 2 = Upperbound (Beta)
};

// Globals for Zobrist
uint64_t zobristTable[25][2]; // Max 25 squares (5x5), 2 players (0 for 'X', 1 for 'O')
unordered_map<uint64_t, TTEntry> transpositionTable;

// Initialize the table with random 64-bit numbers
void initZobrist() {
    mt19937_64 rng(12345); // Fixed seed for reproducibility
    for (int i = 0; i < 25; i++) {
        zobristTable[i][0] = rng(); // Random 64-bit num for 'X'
        zobristTable[i][1] = rng(); // Random 64-bit num for 'O'
    }
}

// Function to compute the initial hash of a board (only run once at the start of minimax)
uint64_t computeHash(const vector<char>& board) {
    uint64_t hash = 0;
    for (int i = 0; i < board.size(); i++) {
        if (board[i] == 'X') hash ^= zobristTable[i][0];
        else if (board[i] == 'O') hash ^= zobristTable[i][1];
    }
    return hash;
}

void printBoard(vector<char>& board, int n) {

    int m = sqrt(n);
    for (int i = 0; i < n; i++) {

        cout << " " << board[i] << " ";
        
        // Print vertical separator
        if ((i + 1) % m != 0) {
            cout << "|";
        }
        // End of row
        else {
            cout << endl;
            // Print horizontal separator except after last row
            if (i != n - 1) {

                for (int j = 0; j < m; j++) {
                    cout << "---";

                    if (j != m - 1) {
                        cout << "+";
                    }
                }
                cout << endl;
            }
        }
    }
}

// 1. Pass 'm' to make it dynamic for 3x3, 4x4, and 5x5
int evaluateLine(int xCount, int oCount, int m) {
    if (xCount > 0 && oCount > 0) return 0; // Blocked line
    
    // AI ('O') is maximizing
    if (oCount == m) return 1000000;         // Win threshold bumped to 1 Million
    if (oCount == m - 1) return 100;         // Strong threat
    if (oCount == m - 2 && m > 3) return 10; // Building
    
    // Human ('X') is minimizing
    if (xCount == m) return -1000000; 
    if (xCount == m - 1) return -100;
    if (xCount == m - 2 && m > 3) return -10;
    
    return 0;
}

// Scans the entire board
int evaluateBoard(const vector<char>& board, int m) {
    int totalScore = 0;
    
    // Rows
    for (int i = 0; i < m * m; i += m) {
        int xCount = 0, oCount = 0;
        for (int j = 0; j < m; j++) {
            if (board[i + j] == 'X') xCount++;
            else if (board[i + j] == 'O') oCount++;
        }
        int score = evaluateLine(xCount, oCount, m);
        if (score >= 1000000 || score <= -1000000) return score; // Short-circuit!
        totalScore += score;
    }
    
    // Columns
    for (int i = 0; i < m; i++) {
        int xCount = 0, oCount = 0;
        for (int j = i; j < m * m; j += m) {
            if (board[j] == 'X') xCount++;
            else if (board[j] == 'O') oCount++;
        }
        int score = evaluateLine(xCount, oCount, m);
        if (score >= 1000000 || score <= -1000000) return score; // Short-circuit!
        totalScore += score;
    }
    
    // Left Diagonal
    int xCount = 0, oCount = 0;
    for (int i = 0; i < m * m; i += (m + 1)) {
        if (board[i] == 'X') xCount++;
        else if (board[i] == 'O') oCount++;
    }
    int scoreLeft = evaluateLine(xCount, oCount, m);
    if (scoreLeft >= 1000000 || scoreLeft <= -1000000) return scoreLeft;
    totalScore += scoreLeft;
    
    // Right Diagonal
    xCount = 0; oCount = 0;
    for (int i = m - 1; i <= (m - 1) * m; i += (m - 1)) {
        if (board[i] == 'X') xCount++;
        else if (board[i] == 'O') oCount++;
    }
    int scoreRight = evaluateLine(xCount, oCount, m);
    if (scoreRight >= 1000000 || scoreRight <= -1000000) return scoreRight;
    totalScore += scoreRight;
    
    return totalScore;
}

// A much faster draw check for the recursion loop
bool isBoardFull(const vector<char>& board) {
    for (char c : board) {
        if (c == ' ') return false;
    }
    return true;
}

int minimax(vector<char> &board, int depth, bool isMaximizing, int alpha, int beta, int m, uint64_t currentHash) {
    
    // 1. TRANSPOSITION TABLE LOOKUP
    if (transpositionTable.find(currentHash) != transpositionTable.end()) {
        TTEntry entry = transpositionTable[currentHash];
        // Only use the cached score if we searched to at least the same depth
        if (entry.depth >= depth) {
            if (entry.flag == 0) return entry.score; // Exact
            if (entry.flag == 1 && entry.score > alpha) alpha = entry.score; // Lowerbound
            if (entry.flag == 2 && entry.score < beta) beta = entry.score;   // Upperbound
            if (alpha >= beta) return entry.score; // Prune immediately!
        }
    }

    int score = evaluateBoard(board, m);
    
    // Terminal states
    if (score >= 1000000) return score + depth; 
    if (score <= -1000000) return score - depth;
    if (isBoardFull(board) || depth == 0) return score;
    
    int originalAlpha = alpha;
    int bestScore;

    if (isMaximizing) {
        bestScore = INT_MIN;
        for (int i = 0; i < board.size(); i++) {
            if (board[i] == ' ') {
                board[i] = 'O';
                // XOR to add 'O' to the hash
                uint64_t newHash = currentHash ^ zobristTable[i][1]; 
                
                bestScore = max(bestScore, minimax(board, depth - 1, false, alpha, beta, m, newHash));
                
                board[i] = ' '; // Backtrack
                alpha = max(alpha, bestScore);
                if (beta <= alpha) break; // Prune
            }
        }
    } else {
        bestScore = INT_MAX;
        for (int i = 0; i < board.size(); i++) {
            if (board[i] == ' ') {
                board[i] = 'X';
                // XOR to add 'X' to the hash
                uint64_t newHash = currentHash ^ zobristTable[i][0];
                
                bestScore = min(bestScore, minimax(board, depth - 1, true, alpha, beta, m, newHash));
                
                board[i] = ' '; // Backtrack
                beta = min(beta, bestScore);
                if (beta <= alpha) break; // Prune
            }
        }
    }

    // 2. TRANSPOSITION TABLE STORE
    TTEntry newEntry;
    newEntry.score = bestScore;
    newEntry.depth = depth;
    if (bestScore <= originalAlpha) newEntry.flag = 2; // Upperbound
    else if (bestScore >= beta) newEntry.flag = 1;     // Lowerbound
    else newEntry.flag = 0;                            // Exact Match
    
    transpositionTable[currentHash] = newEntry;

    return bestScore;
}

int getBestMove(vector<char> &board, int m) {
    int bestScore = INT_MIN; 
    int bestIndex = -1;
    int maxDepth = 5;
    if(m==3)
        maxDepth=9;
    else if(m==4)
        maxDepth=5;
    else
        maxDepth=4;
    
    uint64_t currentHash = computeHash(board); // Get the starting hash
    transpositionTable.clear(); // Clear table between turns to save memory
    
    for (int i = 0; i < board.size(); i++) {
        if (board[i] == ' ') {
            board[i] = 'O';
            uint64_t newHash = currentHash ^ zobristTable[i][1];
            
            int score = minimax(board, maxDepth, false, INT_MIN, INT_MAX, m, newHash);
            board[i] = ' '; 
            
            if (score > bestScore) {  
                bestScore = score;
                bestIndex = i;
            }
        }
    }
    return bestIndex;
}

int main(int argc, char* argv[]) {

    if (argc < 3) {
        cout << "Invalid Arguments";
        return 1;
    }

    initZobrist();

    string values = argv[1];
    int n = stoi(argv[2]);

    vector<char> board;

    for (int i = 0; i < n * n; i++) {
        board.push_back(values[i]);
    }

    int AIindex = getBestMove(board, n);

    cout << AIindex;

    return 0;
}
