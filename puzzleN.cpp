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
        int edge = 1;

        //Graph stuff
        vector<vector<int>> adj; //adj list
        vector<bool> visited; 
        priority_queue<pair<int, int>, vector<pair<int, int>>, Priority> pq;
        int vCounter;
        vector<Node> vertices;

    public:
        PuzzleN(int size) : adj(size), visited(size) {
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
            curr = 0; 
            vertices.emplace_back(0, numbers, heuristicF(numbers), 0);
            pq.push(make_pair(0, vertices.back().h));
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

        void shufflePuzzle() {
            random_device rd;
            mt19937 g(rd());

            shuffle(numbers.begin(), numbers.end(), g);
        }

        void expand() {
            vector<string> directions = {"up", "down", "left", "right"};
            vector<size_t> newVertices;
            int parent = vCounter-1;
            for (const auto& dir : directions) {
                //cout << "Trying direction: " << dir << endl;
                if (move(dir)){
                    vector<int> temp = numbers;
                    newSpace = (newRow * side) + newCol;
                    //cout << "newSpace: " << newSpace << ", newRow: " << newRow << ", newCol: " << newCol << endl;
                    swap(temp[emptySpace], temp[newSpace]);
                    vertices.emplace_back(vCounter, temp, heuristicF(temp), vertices[curr].g + 1);
                    if (adj.size() <= vCounter) {
                        adj.resize(vCounter + 1);
                    }
                    newVertices.push_back(vCounter);
                    if (!newVertices.empty()) {
                        //cout << "Adding edge..."; 
                        addEdge(parent, newVertices.back());  
                        pq.push(make_pair(vCounter, vertices.back().h + vertices.back().g));
                    }
                    //printNode(vertices.back());
                    vCounter++;

                } else {
                    //cout << "Move failed" << endl; 
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

            int child = pq.top().first;
            visited[child] = true;

            pq.pop();

            path.push_back(make_pair(child, curr));

            curr = child;

            numbers = vertices[child].data;

            //Create board

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
        }

        void addEdge(int v1, int v2) {
            if (v1 >= vertices.size() || v2 >= vertices.size()) {
                cout << "Error: Vertex index out of bounds" << endl;
                return;
            }
            adj[v1].push_back(v2);
            adj[v2].push_back(v1);
            //cout << "Edge added successfully" << endl;
        }

        void execute() {
            int k = 250;
            //cout << "heuristics: " << vertices[curr].h << endl;
            //cout << "queue empty? " << !pq.empty() << endl;
            while(!pq.empty() && vertices[curr].h != 0 && k > 0) {
                //printBoard();
                expand();
                //printAdjacencyList();
                choice();
                //printBoard();
                k--;
                //cout << "steps: " << k << endl; 
                }
            printBoard();
            cout << "End of program." << endl;
        }

        void emptyQueue() {
            while (!pq.empty()) {
                cout << "(" << pq.top().first << ", " << pq.top().second << ") ";
                pq.pop();
            }
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
    PuzzleN puzzle(9);


    puzzle.execute();
    puzzle.printPath();

    return 0;
}