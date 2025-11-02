#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <array>
#include <cmath>
#include <algorithm>

using namespace std;

enum class State
{
    kEmpty,
    kObstacle,
    kClosed,
    kPath
};

vector<State> ParseLine(string line)
{
    stringstream myLine(line);

    int n;
    char c;
    vector<State> row;

    while (myLine >> n >> c && c == ',')
    {
        if (n != 0)
        {
            row.push_back(State::kObstacle);
        }
        else
        {
            row.push_back(State::kEmpty);
        }
    }

    return row;
}

vector<vector<State>> ReadBoard(string path)
{
    ifstream myFile;

    myFile.open(path);

    vector<vector<State>> board;

    if (myFile)
    {
        string line;

        while (getline(myFile, line))
        {
            vector<State> row = ParseLine(line);

            board.push_back(row);
        }
    }

    return board;
}

bool Compare(vector<int> node1, vector<int> node2)
{
    int f1 = node1[2] + node1[3];
    int f2 = node2[2] + node2[3];

    return f1 > f2;
}

void CellSort(vector<vector<int>> *v)
{
    sort(v->begin(), v->end(), Compare);
}

string CellString(State cell)
{
    switch (cell)
    {
    case State::kObstacle:
        return "⛰️   ";
    case State::kClosed:
        return "⚪   ";
    case State::kPath:
        return "🚗   ";
    default:
        return "0   ";
    }
}

void PrintBoard(vector<vector<State>> board)
{
    for (vector<State> row : board)
    {
        for (State cell : row)
        {
            cout << CellString(cell);
        }

        cout << "\n";
    }
}

int Heuristic(int x_1, int y_1, int x_2, int y_2)
{
    return abs(x_2 - x_1) + abs(y_2 - y_1);
}

void AddToOpen(int x, int y, int g, int h, vector<vector<int>> &openNodes, vector<vector<State>> &grid)
{
    vector<int> node = {x,
                        y,
                        g,
                        h};

    openNodes.push_back(node);

    grid[x][y] = State::kClosed;
}

bool CheckValidCell(int x, int y, vector<vector<State>> &grid)
{
    bool on_grid_x = x >= 0 && x < grid.size();
    bool on_grid_y = y >= 0 && y < grid[0].size();

    return on_grid_x && on_grid_y && grid[x][y] == State::kEmpty;
}

void ExpandNeighbors(vector<int> &currentNode, vector<vector<int>> &openNodes, vector<vector<State>> &board, array<int, 2> &goal)
{
    const int x = currentNode[0];
    const int y = currentNode[1];
    const int g = currentNode[2];

    const int delta[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

    for (auto dir : delta)
    {
        const int x2 = x + dir[0];
        const int y2 = y + dir[1];

        if (CheckValidCell(x2, y2, board))
        {
            const int h2 = Heuristic(x2, y2, goal[0], goal[1]);
            const int g2 = g + 1;

            AddToOpen(x2, y2, g2, h2, openNodes, board);
        }
    }
}

vector<vector<State>> Search(vector<vector<State>> &board, array<int, 2> start, array<int, 2> goal)
{
    vector<vector<int>> open{};

    int x = start[0];
    int y = start[1];
    int g = 0;
    int h = Heuristic(x, y, goal[0], goal[1]);

    AddToOpen(x, y, g, h, open, board);

    while (!open.empty())
    {
        CellSort(&open);

        vector<int> current = open.back();

        open.pop_back();

        x = current[0];
        y = current[1];

        board[x][y] = State::kPath;

        if (x == goal[0] && y == goal[1])
        {
            cout << "Goal found!\n";

            return board;
        }

        ExpandNeighbors(current, open, board, goal);
    }

    cout << "No path found!" << "\n";
    return std::vector<vector<State>>{};
}

int main()
{
    int init[2]{0, 0};
    int goal[2]{4, 5};
    vector<vector<State>> board = ReadBoard("1.board");
    vector<vector<State>> solution = Search(board, {0, 0}, {4, 5});
    PrintBoard(solution);
}
