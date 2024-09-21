#include <iostream>
#include <string>
#include <vector>
#include <variant>
#include <algorithm>
#include <queue>
#include <random>
#include <cmath>

#include "hash_table.hpp"

using namespace std;

struct HashFunction {
    int operator()(const string& s) const {  
        size_t hash = 0;
        for (char c : s) {
            int digit = c - '0';
            hash = hash * 10 + digit;
        }
        return hash;
    }
};

struct Node {
    const int id;
    vector<int> data;
    const int h; // h(n)
    int g; // g(n)
    Node* next;

    Node(int n, vector<int>& numbers, int weight, int cost) : id(n), data(numbers), h(weight), g(cost), next(nullptr) {}
};

struct Priority {
    bool operator()(const pair<int, int>& p1, const pair<int, int>& p2) {
        return p1.second > p2.second;
    }
};

class PuzzleN {
private: 
    vector<vector<int>> board;
    int emptyRow, emptyCol, emptySpace, side, boardSize, curr;
    vector<int> numbers;
    vector<int> tState;
    vector<pair<int, int>> path;
    vector<vector<int>> adj;
    vector<bool> visited;
    priority_queue<pair<int, int>, vector<pair<int, int>>, Priority> pq;
    int vCounter;
    vector<Node> vertices;
    HashTable<string, bool, HashFunction> ht;

public:
    PuzzleN(int size) : adj(size), visited(size) {
        boardSize = size;
        side = sqrt(size);
        terminalState(size);
        shufflePuzzle();
        initializeBoard();
        initializeFirstNode();
    }    

    void terminalState(int size) {
        board.resize(side, vector<int>(side));
        for (int i = 0; i < size; i++) {
            numbers.push_back(i);
        }
        tState = numbers;
    }

    void shufflePuzzle() {
        random_device rd;
        mt19937 g(rd());
        shuffle(numbers.begin(), numbers.end(), g);
    }

    void initializeBoard() {
        int index = 0;
        for (int i = 0; i < side; i++) {
            for (int j = 0; j < side; j++) {
                board[i][j] = numbers[index];
                if (numbers[index] == 0) {
                    emptyRow = i;
                    emptyCol = j;
                }
                index++;
            }
        }
        emptySpace = emptyCol + side * emptyRow;
    }

    void initializeFirstNode() {
        vCounter = 1;
        curr = 0;
        vertices.emplace_back(0, numbers, heuristicF(numbers), 0);
        pq.push(make_pair(0, vertices.back().h));
        ht.insert(vectorToString(numbers), true);
    }

    void expand() {
        vector<string> directions = {"up", "down", "left", "right"};
        int parent = curr;
        for (const auto& dir : directions) {
            if (isLegalMove(dir)) {
                vector<int> tempNumbers = numbers;
                int tempEmptyRow = emptyRow, tempEmptyCol = emptyCol, tempEmptySpace = emptySpace;
                
                if (move(dir)) {
                    string newState = vectorToString(numbers);
                    if (!ht.get(newState)) {
                        vertices.emplace_back(vCounter, numbers, heuristicF(numbers), vertices[curr].g + 1);
                        if (adj.size() <= vCounter) {
                            adj.resize(vCounter + 1);
                        }
                        addEdge(parent, vCounter);
                        pq.push(make_pair(vCounter, vertices.back().h + vertices.back().g));
                        vCounter++;
                        ht.insert(newState, true);
                    }
                    
                    // Revert the move
                    numbers = tempNumbers;
                    emptyRow = tempEmptyRow;
                    emptyCol = tempEmptyCol;
                    emptySpace = tempEmptySpace;
                    board[emptyRow][emptyCol] = 0;
                }
            }
        }
    }

    bool isLegalMove(const string& dir) const {
        if (dir == "up" && emptyRow > 0) return true;
        if (dir == "down" && emptyRow < side - 1) return true;
        if (dir == "left" && emptyCol > 0) return true;
        if (dir == "right" && emptyCol < side - 1) return true;
        return false;
    }

    bool move(string dir) {
        int newRow = emptyRow, newCol = emptyCol;
        
        if (dir == "up") newRow--;
        else if (dir == "down") newRow++;
        else if (dir == "left") newCol--;
        else if (dir == "right") newCol++;
        else return false;

        int newSpace = newRow * side + newCol;
        swap(numbers[emptySpace], numbers[newSpace]);
        
        board[emptyRow][emptyCol] = numbers[emptySpace];
        board[newRow][newCol] = numbers[newSpace];

        emptyRow = newRow;
        emptyCol = newCol;
        emptySpace = newSpace;

        return true;
    }

    int heuristicF(const vector<int>& v) {
        int hn = 0;
        for (size_t i = 0; i < v.size(); ++i) {
            if (v[i] != 0 && v[i] != tState[i]) {
                hn++;
            }
        }
        return hn;
    }

    void choice() {
        while (!pq.empty()) {
            int child = pq.top().first;
            pq.pop();
            
            if (!visited[child]) {
                visited[child] = true;
                path.push_back(make_pair(child, curr));
                curr = child;
                numbers = vertices[child].data;
                updateBoardFromNumbers();
                break;
            }
        }
    }

    void updateBoardFromNumbers() {
        int index = 0;
        for (int i = 0; i < side; i++) {
            for (int j = 0; j < side; j++) {
                board[i][j] = numbers[index];
                if (numbers[index] == 0) {
                    emptyRow = i;
                    emptyCol = j;
                }
                index++;
            }
        }
        emptySpace = emptyCol + side * emptyRow;
    }

    void addEdge(int v1, int v2) {
        if (v1 >= adj.size() || v2 >= adj.size()) {
            cout << "Error: Vertex index out of bounds" << endl;
            return;
        }
        adj[v1].push_back(v2);
        adj[v2].push_back(v1);
    }

    void execute() {
        int maxIterations = 500;
        int iterations = 0;
        while (!pq.empty() && vertices[curr].h != 0 && iterations < maxIterations) {
            cout << "Iteration: " << iterations << ", Node: " << vertices[curr].id << endl;
            printBoard();
            expand();
            choice();
            iterations++;
        }
        
        if (vertices[curr].h == 0) {
            cout << "Solution found!" << endl;
        } else {
            cout << "Solution not found within " << maxIterations << " iterations." << endl;
        }
        printBoard();
        printPath();
    }

    string vectorToString(const vector<int>& v) {
        string result;
        for (int num : v) {
            result += to_string(num);
        }
        return result;
    }

    void printBoard() {
        cout << "-";
        if (side < 4){
            for (int i = 0; i < side; i++){
                cout << "----";
            }
            cout << endl;
            for (const auto& row : board) {
                cout << "| ";
                for (int tile : row){
                    if (tile == 0){
                        cout << "  | ";
                    } else {
                        cout << tile << " | ";
                    }
                }
                cout << "\n";
            }
            for (int i = 0; i < side; i++){
                cout << "----";
            }
        } else {
            for (int i = 0; i < side; i++){
                cout << "-----";
            }
            cout << endl;
            for (const auto& row : board) {
                cout << "| ";
                for (int tile : row){
                    if (tile < 10 && tile != 0){
                        cout << tile << "  | ";
                    } else if (tile == 0){
                        cout << "   | ";
                    } else {
                        cout << tile << " | ";
                    }
                }
                cout << "\n";
            }
            for (int i = 0; i < side; i++){
                cout << "-----";
            }   
        }
            
        cout << "-" << endl;
    }


    void printPath() {
        cout << "Solution path: ";
        for (const auto& pair : path) {
            cout << pair.first << " -> ";
        }
        cout << "Goal" << endl;
    }
};

int main() {
    PuzzleN puzzle(9);
    puzzle.execute();
    return 0;
}