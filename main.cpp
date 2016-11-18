#include <iostream>
#include <map>
#include <queue>
#include <cmath>
#define PUZZLESIZE 8
using namespace std;

class PuzzleState {
public:
    int puzzleSize = 8;
    int mode = 0; // 0: bfs, 1: Misplaced, 2: Manhattan
    int g;
    vector<int> state;
    vector<vector<int>> path;

    PuzzleState() {}
    PuzzleState(int p, int m, int g, vector<int> state):puzzleSize(p), mode(m), g(g), state(state) {
        (this -> path).push_back(state);
    }

    bool empty() {return state.empty();}

    // Compare function, used for priority_queue
    bool operator < (const PuzzleState &p) const{
        if (!mode) {
            return (this->g) >= p.g;
        } else if (mode == 1) {
            return (MTH(*this) + this->g) > (MTH(p) + p.g);
        } else {
            return (this->g + MDH(*this)) > p.g + MDH(p);
        }
    }

    // Misplaced Tile heuristic
    const int MTH(PuzzleState puzzleState) const{
        vector<int> state = puzzleState.state;
        int misplaced = 0;
        for (int i = 0; i < puzzleSize + 1; ++i ) {
            if (!state[i]) continue;
            if (state[i] != i + 1) misplaced++;
        }
        return misplaced;
    }

    const int findMD(int current, int goal, int rowSize) const{
        int x1, y1, x2, y2;
        x1 = current / rowSize;
        y1 = current % rowSize;
        x2 = goal / rowSize;
        y2 = goal % rowSize;

        return abs(x1 - x2) + abs(y1 - y2);
    }

    // Manhattan Distance heuristic
    const int MDH(PuzzleState puzzleState) const{
        vector<int> state = puzzleState.state;
        int rowSize = (int)sqrt(puzzleSize + 1);
        int distance = 0;
        for (int i = 0; i < puzzleSize + 1; ++i) {
            if (!state[i]) continue;
            distance += findMD(state[i], i + 1, rowSize);
        }
        return distance;
    }
};

class EightPuzzle {
private:
    map<vector<int>, bool> flag;
public:
    int expandNode = 0;
    int maxNodeInQ = 0;
    int puzzleSize = 8;
    int rowSize = 3;
    bool goal(vector<int> state) {
        for (int i = 0; i < puzzleSize; i++) {
            if (i+1 != state[i])
                return false;
        }
        return true;
    }

    int findBlank(vector<int> state) {
        for (int i = 0; i < state.size(); i++) {
            if (!state[i])
                return i;
        }
        cout << "No blank (0) in the puzzle!!!\n";
        return -1;
    }

    // Generic method to move blank to other place
    PuzzleState move(PuzzleState state, int blankPosition, bool borderJudge, int next) {
        vector<int> currentState = state.state;
        // If blank is on the border, don't move
        if (borderJudge) {
            return PuzzleState();
        }
        int nextBlankPosition = blankPosition + next;
        swap(currentState[blankPosition], currentState[nextBlankPosition]);
        state.state = currentState;
        state.path.push_back(state.state);
        return state;
    }

    PuzzleState goUp(int blankPosition, PuzzleState state) {
        return move(state, blankPosition, (blankPosition < this->rowSize), -(this->rowSize));
    }

    PuzzleState goDown(int blankPosition, PuzzleState state) {
        bool borderJudge = blankPosition > puzzleSize - this->rowSize;
        return move(state, blankPosition, borderJudge, this->rowSize);
    }

    PuzzleState goLeft(int blankPosition, PuzzleState state) {
        bool borderJudge = (blankPosition % this->rowSize == 0);
        return move(state, blankPosition, borderJudge, -1);
    }

    PuzzleState goRight(int blankPosition, PuzzleState state) {
        bool borderJudge = ((blankPosition + 1) % this->rowSize == 0);
        return move(state, blankPosition, borderJudge, 1);
    }

    void showSinglePath(vector<int> p) {
        for (int i = 0; i < puzzleSize + 1; ++i) {
            if (i % this->rowSize == 0)
                cout << endl;
            cout << p[i] << " ";
        }
        cout << endl;
    }

    void showPath(PuzzleState p) {
        auto path = p.path;
        for (auto state : path) {
            showSinglePath(state);
        }
    }

    PuzzleState generalSearch(PuzzleState state) {

        priority_queue<PuzzleState, vector<PuzzleState>> nodes;
        nodes.push(state);

        while (!nodes.empty()) {
            // Update the maximum nodes in queue
            this->maxNodeInQ = max(this->maxNodeInQ, (int)nodes.size());
            PuzzleState currentState = nodes.top();
//            if (currentState.mode == 2) showSinglePath(currentState.state);
            nodes.pop();

            // If current state had been visited
            if (flag.count(currentState.state))
                continue;

            flag[currentState.state] = true;
            currentState.g++;

            // Goal state test
            if (goal(currentState.state)) {
                return currentState;
            }

            // Generic anonymous method to push node into queue
            auto pushState = [this, &nodes](PuzzleState p) {
                if (!p.empty()) {
                    this->expandNode++;
                    nodes.push(p);
                }
            };

            // Move blank to Left, Right, Up, Down
            int blankPosition = findBlank(currentState.state);
            pushState(goLeft(blankPosition, currentState));
            pushState(goRight(blankPosition, currentState));
            pushState(goUp(blankPosition, currentState));
            pushState(goDown(blankPosition, currentState));
        }

        return PuzzleState();
    }
};



int main() {
    int puzzleSize = PUZZLESIZE, mode = 0;
    cout << "Please input puzzle size: ";
    cin >> puzzleSize;
    cout << "Please chose algorithm (0: UCS, 1: A*_MTH, 2: A*_MDH): ";
    cin >> mode;
    cout << "Please input initial state " << puzzleSize + 1 << " separate number (0 as blank):\n";
    vector<int> init((unsigned long)puzzleSize + 1);
    for (int i = 0; i < puzzleSize + 1; ++i) {
        cin >> init[i];
    }

    clock_t beginTime, endTime;
    beginTime = clock();
    EightPuzzle eightPuzzle;
    eightPuzzle.puzzleSize = puzzleSize;
    eightPuzzle.rowSize = (int) sqrt((puzzleSize + 1));
    PuzzleState puzzleState(puzzleSize, mode, 0, init);
    puzzleState = eightPuzzle.generalSearch(puzzleState);
    endTime = clock();
    cout << "To solve this problem the search algorithm expanded a total of " << eightPuzzle.expandNode << " nodes.\n";
    cout << "The maximum number of nodes in the queue at any one time was " << eightPuzzle.maxNodeInQ << endl;
    cout << "The depth of the goal node was " << puzzleState.path.size() - 1 << endl;
    cout << "Totally processor time consumed: " << endTime - beginTime << endl;
//    if (!puzzleState.empty())
//        eightPuzzle.showPath(puzzleState);
}

/***
 * Test Case
1 2 3 4 8 0 7 6 5
1 2 3 4 5 6 7 0 8
8 0 7 3 5 6 2 1 4

2 0 3
7 4 6
5 1 8

1 2 3
4 0 6
7 5 8

8 7 1
6 0 2
5 4 3

1 2 3 4
5 6 7 8
9 10 11 12
13 14 0 15

0 2 3 8
1 6 4 7
5 10 11 12
9 13 14 15

1 5 3 7
9 14 4 8
2 6 13 12
10 0 15 11

2 3 4 5 10
1 7 0 8 9
6 12 13 14 15
11 17 18 19 20
16 21 22 23 24

2 0 3 4 5
1 7 8 9 10
6 12 13 14 15
11 17 18 19 20
16 21 22 23 24

7 1 5 4 11
16 17 14 8 15
3 13 23 2 10
21 12 9 6 18
22 0 19 20 24
 ***/