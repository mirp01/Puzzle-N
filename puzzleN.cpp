#include <iostream>
#include <vector>
#include <variant>
#include <algorithm>
#include <queue>
#include <random>

using namespace std;


struct Node {
    const int id;
    vector<int> data;
    const int weight;
    Node* next;

    Node(int n, vector<int>& numbers, int w) : id(n), data(numbers), weight(w), next(nullptr) {}
};

class PuzzleN {
    private: 
        vector<vector<int>> board; //dynamic board
        int emptyRow, emptyCol, emptySpace, newSpace, side, size, newRow, newCol, boardSize; //important variables
        vector<int> numbers; // Vector of numbers in the actual board
        vector<int> tState; // Vector of numbers Terminal State

        //Graph stuff
        vector<vector<int>> adj; //adj list
        vector<bool> visited; 
        queue<int> q;
        int vCounter;
        vector<Node> vertices;

    public:
        PuzzleN(int size) {
            boardSize = size;
            terminalState(size);
            shufflePuzzle();
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
            //cout << emptyRow << emptyCol << emptySpace << endl;

            //Creation of n0
            vCounter = 1;
            cout << numbers[1];
            vertices.emplace_back(0, numbers, heuristicF(numbers));
            printNode(vertices[0]);
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

        void shufflePuzzle() {
            random_device rd;
            mt19937 g(rd());

            shuffle(numbers.begin(), numbers.end(), g);
        }

        void expand() {
            vector<string> directions = {"up", "down", "left", "right"};
            vector<size_t> newVertices;
            for (const auto& dir : directions) {
                cout << "Trying direction: " << dir << endl;
                if (move(dir)){
                    vector<int> temp = numbers;
                    newSpace = (newRow * side) + newCol;
                    //cout << "newSpace: " << newSpace << ", newRow: " << newRow << ", newCol: " << newCol << endl;
                    swap(temp[emptySpace], temp[newSpace]);
                    vertices.emplace_back(vCounter, temp, heuristicF(temp));
                    if (!newVertices.empty()) {
                        addEdge(vCounter, newVertices.back());
                    }
                    newVertices.push_back(vertices.size() - 1);
                    printNode(vertices.back());
                    vCounter++;
                } else {
                    cout << "Move failed" << endl; 
                }
            }
        }

        bool move(string dir) { //checks if move is valid
            newRow = emptyRow;
            newCol = emptyCol;

            if (dir == "up") {
                newRow--;
            } else if (dir == "down") {
                newRow++;
            } else if (dir == "left") {
                newCol--;
            } else if (dir == "right") {
                newCol++;
            }

            if (newRow < 0 || newRow >= side || newCol < 0 || newCol >= side) {
                return false;
            } 
            return true;
        }

        int heuristicF(const vector<int>& v) {
            int h = v.size();
            for (size_t i = 0; i < min(v.size(), tState.size()); ++i) {
                if (tState[i] == v[i]) {
                    h--;
                }
            }
            return h;
        }

        void bfs(Node* node){

            queue<int> q;

            visited[node -> id] = true;
            q.push(node -> id);

            while (!q.empty()) {

                int curr = q.front();
                q.pop();
                cout << curr << " ";

                for (int i : adj[curr]){
                    if (!visited[i]){
                        visited[i] = true;
                        q.push(i);
                    }
                }
            }
        }

        void addEdge(int v1, int v2) {
            cout << "Adding edge between " << v1 << " and " << v2 << endl;
            if (v1 >= vertices.size() || v2 >= vertices.size()) {
                cout << "Error: Vertex index out of bounds" << endl;
                return;
            }
            adj[v1].push_back(v2);
            adj[v2].push_back(v1);
            cout << "Edge added successfully" << endl;
        }

        void printNode(const Node& node) {
            cout << endl << "Node num: " << node.id << endl;
            cout << "Node weight: " << node.weight << endl;
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
};


int main(){
    PuzzleN puzzle(9);

    puzzle.printBoard();
    //puzzle.printNumbers();
    puzzle.expand();
    cout << "bu";
    return 0;
}