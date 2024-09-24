#include <iostream>
#include <string>
#include <vector>
#include <variant>
#include <algorithm>
#include <queue>
#include <random>

#include "hash_table.hpp"

using namespace std;

struct HashFunction {
    int operator()(const string& s) const {  
        size_t hash = 0;

        for (char c : s) {
            int digit = c - '0';
            hash = hash * 31 + digit;
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
        vector<vector<int>> board; //dynamic board
        int emptyRow, emptyCol, emptySpace, newSpace, side, size, newRow, newCol, boardSize, curr; //important variables
        vector<int> numbers; // Vector of numbers in the actual board
        vector<int> tState; // Vector of numbers Terminal State
        vector<pair<int, int>> path; //child, curr
        bool solvable = false;
        int edge = 1;

        //Graph stuff
        vector<vector<int>> adj; //adj list
        vector<bool> visited; 
        priority_queue<pair<int, int>, vector<pair<int, int>>, Priority> pq;
        int vCounter;
        vector<Node> vertices;

        //Create hash table 
        HashTable<string, bool, HashFunction> ht; // key, exists, function

    public:
        PuzzleN(int size) : adj(size), visited(size) {
            boardSize = side*side;
            terminalState(size);
            solvable = true; // Change if you want a solvable puzzle

            if (solvable) {
                shufflePuzzle();
                findEmptySpace();
                while (!isSolvable(numbers)) {
                    cout << "shuffling puzzle" << endl;
                    shufflePuzzle();
                    findEmptySpace();
                    cout << "puzzle shuffled" << endl;
                }
            } else {
                shufflePuzzle();
            }

            cout << "solvable? " << isSolvable(numbers) << endl;

            int index = 0;
            for (int i = 0; i < side; i++) {
                vector<int> temp;
                for (int j = 0; j < side; j++) {
                    temp.push_back(numbers[index]);
                    // if (numbers[index] == 0) {
                    //     emptyRow = i;
                    //     emptyCol = j;
                    // }
                    index++;
                }
                board[i] = temp;
            }
            //emptySpace = emptyCol + side*emptyRow;
            cout << emptyRow << emptyCol << emptySpace << endl;

            //Creation of n0
            vCounter = 1;
            curr = 0; 
            vertices.emplace_back(0, numbers, heuristicF(numbers), 0);
            pq.push(make_pair(0, vertices.back().h));
            ht.insert(vectorToString(numbers), true);
            printNode(vertices[0]);
            printBoard();
        }    

        void terminalState(int size) {
            side = sqrt(size);
            board.resize(side, vector<int>(side));
            //numbers.resize(size);
            int num = 0;
            for (int i = 0; i < side; i++){
                vector<int> temp;
                for (int j = 0; j < side; j++){
                    temp.push_back(num);
                    numbers.push_back(num);
                    num++;
                }
                board[i] = temp;
            }
            tState.resize(size);
            tState = numbers;
        }

        bool isSolvable(const vector<int>& v) {
            int invCount = 0;
            for (int i = 0; i < side*side; i++) {
                for (int j = i + 1; j < side*side; j++) {
                    if (v[i] != 0 && v[j] != 0 && v[i] > v[j]) {
                        invCount++;
                    }
                }
            }

            if (side*side % 2 == 1 || side*side % 2 == 0) {
                return (invCount % 2 == 0); //Odd
            } else {
                return (invCount % 2 == 0); //Even
            }

            return false;
        }

        void shufflePuzzle() {
            random_device rd;
            mt19937 g(rd());

            shuffle(numbers.begin(), numbers.end(), g);
        }

        void findEmptySpace() {
            int index = 0;
            for (int i = 0; i < side; i++) {
                for (int j = 0; j < side; j++) {
                    if (numbers[index] == 0) {
                        emptyRow = i;
                        emptyCol = j;
                    }
                    index++;
                }
            }
            emptySpace = emptyCol + side*emptyRow;
        }

        void expand() {
            vector<string> directions = {"up", "down", "left", "right"};
            vector<size_t> newVertices;
            int parent = vCounter-1;
            for (const auto& dir : directions) {
                vector<int> tempNumbers = numbers;
                int tempEmptyRow = emptyRow;
                int tempEmptyCol = emptyCol;
                int tempEmptySpace = emptySpace;

                bool nums = ht.get(vectorToString(numbers));
                //cout << "node already exists: " << nums << endl;

                if (move(dir)) {
                    vertices.emplace_back(vCounter, numbers, heuristicF(numbers), vertices[curr].g + 1);
                    if (adj.size() <= vCounter) {
                        adj.resize(vCounter + 1);
                    }
                    newVertices.push_back(vCounter);
                    if (!newVertices.empty()) {
                        addEdge(parent, newVertices.back());  
                        pq.push(make_pair(vCounter, vertices.back().h + vertices.back().g));
                    }

                    //printNode(vertices[vCounter]);

                    vCounter++;

                    // Revert the move 
                    numbers = tempNumbers;
                    emptyRow = tempEmptyRow;
                    emptyCol = tempEmptyCol;
                    emptySpace = tempEmptySpace;
                    board[emptyRow][emptyCol] = 0;
                }
            }
        }

        bool move(string dir) {
            int newRow = emptyRow;
            int newCol = emptyCol;

            if (dir == "up") {
                newRow--;
            } else if (dir == "down") {
                newRow++;
            } else if (dir == "left") {
                newCol--;
            } else if (dir == "right") {
                newCol++;
            } else {
                cout << "Invalid direction: " << dir << endl;
                return false;
            }

            if (newRow < 0 || newRow >= side || newCol < 0 || newCol >= side) {
                return false;
            }

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
            int hn = v.size();
            for (size_t i = 0; i < min(v.size(), tState.size()); ++i) {
                if (tState[i] == v[i]) {
                    hn--;
                }
            }
            return hn;
        }

        void choice(){
            //cout << "(" << pq.top().first << ", " << pq.top().second << ") " << endl;

             while (!pq.empty()) {
                int child = pq.top().first;
                pq.pop(); 

                if (visited[child]) {
                    continue;
                }

                string nodeD = vectorToString(vertices[child].data);
                if (ht.get(nodeD)) {
                    //cout << "Found data in node: " << child << endl;
                    visited[child] = true;
                    continue; 
                }

                // Suitable node
                path.push_back(make_pair(child, curr));
                curr = child;
                numbers = vertices[child].data;
                ht.insert(nodeD, true);

                // Create board 
                int index = 0;
                for (int i = 0; i < side; i++) {
                    vector<int> temp;
                    for (int j = 0; j < side; j++) {
                        temp.push_back(numbers[index]);
                        if (numbers[index] == 0) {
                            emptyRow = i;
                            emptyCol = j;
                        }
                        index++;
                    }
                    board[i] = temp;
                }
                emptySpace = emptyCol + side*emptyRow;

                return;
            }
        }

        void addEdge(int v1, int v2) {
            if (v1 >= vertices.size() || v2 >= vertices.size()) {
                return;
            }
            adj[v1].push_back(v2);
            adj[v2].push_back(v1);
            //cout << "Edge added successfully" << endl;
        }

        void execute() {
            int k = 10000; //362880
            
            cout << "heuristics: " << vertices[curr].h << endl;
            while(!pq.empty() && vertices[curr].h != 0 && k > 0) {
                //cout << vertices[curr].id << endl;
                //printNode(vertices[curr]);
                //printBoard();
                expand();
                choice();
                //printBoard();
                k--;
                }
            
            printBoard();
            cout << "heuristics: " << vertices[curr].h << endl;
            printNumbers();
            cout << endl << "queue empty? " << pq.empty() << endl;
            //cout << "Nodes created: " << vertices.size() << endl;
            cout << "steps left: " << k << endl; 
            cout << "End of program." << endl;
        }

        void emptyQueue() {
            while (!pq.empty()) {
                cout << "(" << pq.top().first << ", " << pq.top().second << ") ";
                pq.pop();
            }
        }

        string vectorToString(const vector<int>& v) {
            string result;
            for (int num : v) {
                result += to_string(num);
            }
            return result;
        }

        void printNode(const Node& node) {
            cout << "Node num: " << node.id << endl;
            cout << "Node weight: " << node.h << endl;
            cout << "Node cost: " << node.g << endl;
            cout << "Node data: " << endl;
            for (const int value : node.data) {
                cout << value << " ";
            }
            cout << endl;
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

        void printNumbers() {
            for (int num : numbers) {
                cout << num << " ";
            }
        }

        void printAdjacencyList() {
            for (int i = 0; i < adj.size(); ++i) {
                cout << "Vertex " << i << ": ";
                for (int j = 0; j < adj[i].size(); ++j) {
                    cout << adj[i][j] << " ";
                }
                cout << endl;
            }
        }

        void printPath() {
            for (const auto& pair : path) {
                cout << pair.second << " -> ";
            }

            cout << endl;
        }

};


int main(){
    //cout << "hello" << endl;
    PuzzleN puzzle(9);
    //cout << "hi" << endl;

    puzzle.execute();
    //puzzle.printAdjacencyList();
    //puzzle.printPath();

    return 0;
}