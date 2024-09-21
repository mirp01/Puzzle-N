#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <queue>
#include <random>
#include <cmath>
#include <iomanip>
#include <unordered_set>

using namespace std;

struct Node {
    vector<int> state;
    int g;
    int h;
    int f;
    int emptyPos;
    shared_ptr<Node> parent;

    Node(vector<int> s, int cost, int heuristic, int empty, shared_ptr<Node> p = nullptr)
        : state(s), g(cost), h(heuristic), f(g + h), emptyPos(empty), parent(p) {}
};

struct CompareNode {
    bool operator()(const shared_ptr<Node>& a, const shared_ptr<Node>& b) {
        return a->f > b->f;
    }
};

class PuzzleN {
private:
    int size;
    int side;
    vector<int> goal;
    vector<int> initial;
    priority_queue<shared_ptr<Node>, vector<shared_ptr<Node>>, CompareNode> openSet;
    unordered_set<string> closedSet;

public:
    PuzzleN(int n) : size(n), side(sqrt(n)) {
        for (int i = 0; i < size; ++i) {
            goal.push_back(i);
            initial.push_back(i);
        }
        random_device rd;
        mt19937 g(rd());
        shuffle(initial.begin(), initial.end(), g);
    }

    int manhattanDistance(const vector<int>& state) {
        int distance = 0;
        for (int i = 0; i < size; ++i) {
            if (state[i] != 0) {
                int targetX = state[i] % side;
                int targetY = state[i] / side;
                int currentX = i % side;
                int currentY = i / side;
                distance += abs(targetX - currentX) + abs(targetY - currentY);
            }
        }
        return distance;
    }

    string stateToString(const vector<int>& state) {
        string s;
        for (int i : state) {
            s += to_string(i);
        }
        return s;
    }

    vector<shared_ptr<Node>> getNeighbors(shared_ptr<Node> node) {
        vector<shared_ptr<Node>> neighbors;
        int emptyPos = node->emptyPos;
        vector<int> moves = {-side, side, -1, 1}; // up, down, left, right

        for (int move : moves) {
            int newPos = emptyPos + move;
            if (newPos >= 0 && newPos < size &&
                (move == -side || move == side || (emptyPos % side == newPos % side))) {
                vector<int> newState = node->state;
                swap(newState[emptyPos], newState[newPos]);
                int h = manhattanDistance(newState);
                neighbors.push_back(make_shared<Node>(newState, node->g + 1, h, newPos, node));
            }
        }
        return neighbors;
    }

    void solve() {
        int emptyPos = find(initial.begin(), initial.end(), 0) - initial.begin();
        shared_ptr<Node> start = make_shared<Node>(initial, 0, manhattanDistance(initial), emptyPos);
        openSet.push(start);

        int iterations = 0;
        while (!openSet.empty() && iterations < 100000) {
            shared_ptr<Node> current = openSet.top();
            openSet.pop();

            if (current->state == goal) {
                cout << "Solution found in " << iterations << " iterations!" << endl;
                printSolution(current);
                return;
            }

            string stateStr = stateToString(current->state);
            if (closedSet.find(stateStr) != closedSet.end()) {
                continue;
            }
            closedSet.insert(stateStr);

            for (auto neighbor : getNeighbors(current)) {
                string neighborStr = stateToString(neighbor->state);
                if (closedSet.find(neighborStr) == closedSet.end()) {
                    openSet.push(neighbor);
                }
            }

            iterations++;
            if (iterations % 1000 == 0) {
                cout << "Iterations: " << iterations << ", Queue size: " << openSet.size() << endl;
            }
        }

        cout << "Solution not found within 100000 iterations." << endl;
    }

    void printSolution(shared_ptr<Node> node) {
        vector<shared_ptr<Node>> path;
        while (node != nullptr) {
            path.push_back(node);
            node = node->parent;
        }
        reverse(path.begin(), path.end());

        cout << "Solution path (" << path.size() - 1 << " moves):" << endl;
        for (const auto& n : path) {
            printBoard(n->state);
            cout << endl;
        }
    }

    void printBoard(const vector<int>& state) {
        for (int i = 0; i < size; ++i) {
            if (i % side == 0 && i != 0) cout << endl;
            cout << setw(2) << state[i] << " ";
        }
        cout << endl;
    }
};

int main() {
    PuzzleN puzzle(9);
    puzzle.solve();
    return 0;
}