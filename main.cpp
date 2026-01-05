#include <algorithm>
#include <chrono>
#include <fstream>
#include <strings.h>
#include <random>
#include <iostream>
#include <set>
#include <stack>
#include <unordered_map>

int SIZE = 10;
int MIN_NUM_TILES = (SIZE*SIZE)/2;
int** arr = nullptr;

enum DIRECTIONS {
    UP = 0,
    DOWN,
    RIGHT,
    LEFT
};

enum TILES {
    THRU_UP_DOWN = 0,
    THRU_RIGHT_LEFT,    // 1
    ELBOW_UP_RIGHT,     // 2
    ELBOW_UP_LEFT,      // 3
    ELBOW_DOWN_RIGHT,   // 4
    ELBOW_DOWN_LEFT,    // 5
    T_NO_UP,            // 6
    T_NO_DOWN,          // 7
    T_NO_LEFT,          // 8
    T_NO_RIGHT,         // 9
    DEAD_END_UP,        // 10
    DEAD_END_DOWN,      // 11
    DEAD_END_RIGHT,     // 12
    DEAD_END_LEFT,      // 13
    FOUR_WAY,           // 14
    TILES_SIZE          // 15
};

// just for printing out
std::unordered_map<int, std::string> charset {
    {T_NO_LEFT, "⊢"},
    {T_NO_RIGHT, "⊣"},
    {T_NO_UP, "⊤"},
    {T_NO_DOWN, "⊥"},
    {THRU_UP_DOWN, "|"},
    {THRU_RIGHT_LEFT, "-"},
    {FOUR_WAY, "+"},
    {ELBOW_DOWN_RIGHT, "⎾"},
    {ELBOW_UP_RIGHT, "⎿"},
    {ELBOW_DOWN_LEFT, "⏋"},
    {ELBOW_UP_LEFT, "⏌"},
    {DEAD_END_DOWN, "▲"},
    {DEAD_END_UP, "▼"},
    {DEAD_END_RIGHT, "◀"},
    {DEAD_END_LEFT, "▶"},
    {-1, "?"}
};

int adjacencyList[TILES_SIZE][4];

void buildTileAdjacency () {
    // adjacency for tile
    adjacencyList[THRU_UP_DOWN][UP] = 1;
    adjacencyList[THRU_UP_DOWN][DOWN] = 1;
    adjacencyList[THRU_UP_DOWN][RIGHT]  = 0;
    adjacencyList[THRU_UP_DOWN][LEFT]  = 0;

    // adjacency for tile
    adjacencyList[THRU_RIGHT_LEFT][UP]      = 0;
    adjacencyList[THRU_RIGHT_LEFT][DOWN]    = 0;
    adjacencyList[THRU_RIGHT_LEFT][RIGHT]   = 1;
    adjacencyList[THRU_RIGHT_LEFT][LEFT]    = 1;

    // adjacency for tile
    adjacencyList[ELBOW_UP_RIGHT][UP]       = 1;
    adjacencyList[ELBOW_UP_RIGHT][DOWN]     = 0;
    adjacencyList[ELBOW_UP_RIGHT][RIGHT]    = 1;
    adjacencyList[ELBOW_UP_RIGHT][LEFT]     = 0;

    // adjacency for tile
    adjacencyList[ELBOW_UP_LEFT][UP]        = 1;
    adjacencyList[ELBOW_UP_LEFT][DOWN]      = 0;
    adjacencyList[ELBOW_UP_LEFT][RIGHT]     = 0;
    adjacencyList[ELBOW_UP_LEFT][LEFT]      = 1;

    // adjacency for tile
    adjacencyList[ELBOW_DOWN_RIGHT][UP]     = 0;
    adjacencyList[ELBOW_DOWN_RIGHT][DOWN]   = 1;
    adjacencyList[ELBOW_DOWN_RIGHT][RIGHT]  = 1;
    adjacencyList[ELBOW_DOWN_RIGHT][LEFT]   = 0;

    // adjacency for tile
    adjacencyList[ELBOW_DOWN_LEFT][UP]      = 0;
    adjacencyList[ELBOW_DOWN_LEFT][DOWN]    = 1;
    adjacencyList[ELBOW_DOWN_LEFT][RIGHT]   = 0;
    adjacencyList[ELBOW_DOWN_LEFT][LEFT]    = 1;

    // adjacency for tile
    adjacencyList[T_NO_UP][UP] = 0;
    adjacencyList[T_NO_UP][DOWN] = 1;
    adjacencyList[T_NO_UP][RIGHT]  = 1;
    adjacencyList[T_NO_UP][LEFT]  = 1;

    // adjacency for tile
    adjacencyList[T_NO_DOWN][UP] = 1;
    adjacencyList[T_NO_DOWN][DOWN] = 0;
    adjacencyList[T_NO_DOWN][RIGHT]  = 1;
    adjacencyList[T_NO_DOWN][LEFT]  = 1;

    // adjacency for tile
    adjacencyList[T_NO_RIGHT][UP] = 1;
    adjacencyList[T_NO_RIGHT][DOWN] = 1;
    adjacencyList[T_NO_RIGHT][RIGHT]  = 0;
    adjacencyList[T_NO_RIGHT][LEFT]  = 1;

    // adjacency for tile
    adjacencyList[T_NO_LEFT][UP] = 1;
    adjacencyList[T_NO_LEFT][DOWN] = 1;
    adjacencyList[T_NO_LEFT][RIGHT]  = 1;
    adjacencyList[T_NO_LEFT][LEFT]  = 0;

    // adjacency for tile
    adjacencyList[DEAD_END_UP][UP] = 1;
    adjacencyList[DEAD_END_UP][DOWN] = 0;
    adjacencyList[DEAD_END_UP][RIGHT]  = 0;
    adjacencyList[DEAD_END_UP][LEFT]  = 0;

    // adjacency for tile
    adjacencyList[DEAD_END_DOWN][UP] = 0;
    adjacencyList[DEAD_END_DOWN][DOWN] = 1;
    adjacencyList[DEAD_END_DOWN][RIGHT]  = 0;
    adjacencyList[DEAD_END_DOWN][LEFT]  = 0;

    // adjacency for tile
    adjacencyList[DEAD_END_RIGHT][UP] = 0;
    adjacencyList[DEAD_END_RIGHT][DOWN] = 0;
    adjacencyList[DEAD_END_RIGHT][RIGHT]  = 1;
    adjacencyList[DEAD_END_RIGHT][LEFT]  = 0;

    // adjacency for tile
    adjacencyList[DEAD_END_LEFT][UP] = 0;
    adjacencyList[DEAD_END_LEFT][DOWN] = 0;
    adjacencyList[DEAD_END_LEFT][RIGHT]  = 0;
    adjacencyList[DEAD_END_LEFT][LEFT]  = 1;

    // adjacency for tile
    adjacencyList[FOUR_WAY][UP] = 1;
    adjacencyList[FOUR_WAY][DOWN] = 1;
    adjacencyList[FOUR_WAY][RIGHT]  = 1;
    adjacencyList[FOUR_WAY][LEFT]  = 1;
}


// checks if a tile is valid to be placed at the x/y coordinate
bool valid (const int x, const int y, const TILES value) {
    // check if edge connection outside of grid
    if (adjacencyList[value][UP] == 1 && x+1 >= SIZE) {
        return false;
    }
    if (adjacencyList[value][DOWN] == 1 && x-1 < 0) {
        return false;
    }
    if (adjacencyList[value][RIGHT] == 1 && y+1 >= SIZE) {
        return false;
    }
    if (adjacencyList[value][LEFT] == 1 && y-1 < 0) {
        return false;
    }
    // check if shares edge connection with adjacent tiles
    if (0 <= x-1 && x-1 < SIZE) {
        // check if the cell we are checking has been resolved
        if (arr[x-1][y] != -1) {
            if ( adjacencyList[value][DOWN] != adjacencyList[arr[x-1][y]][UP] ) {
                return false;
            }
        }
    }
    if (0 <= x+1 && x+1 < SIZE) {
        // check if the cell we are checking has been resolved
        if (arr[x+1][y] != -1) {
            if ( adjacencyList[value][UP] != adjacencyList[arr[x+1][y]][DOWN] ) {
                return false;
            }
        }
    }
    if (0 <= y+1 && y+1 < SIZE) {
        // check if the cell we are checking has been resolved
        if (arr[x][y+1] != -1) {
            if ( adjacencyList[value][RIGHT] != adjacencyList[arr[x][y+1]][LEFT] ) {
                return false;
            }
        }
    }
    if (0 <= y-1 && y-1 < SIZE) {
        // check if the cell we are checking has been resolved
        if (arr[x][y-1] != -1) {
            if ( adjacencyList[value][LEFT] != adjacencyList[arr[x][y-1]][RIGHT] ) {
                return false;
            }
        }
    }
    return true;
}



std::set<int> visited{};
std::stack<int> s1{}, s2{};
void insertNeighbors (const int i) {
    int ind;
    // left
    if (0 <= i-1 && i-1 < SIZE*SIZE) {
        ind = i-1;
        if (!visited.contains(ind)) {
            visited.insert(ind);
            s2.push(ind);
        }
    }
    // right
    if (0 <= i+1 && i+1 < SIZE*SIZE) {
        ind = i+1;
        if (!visited.contains(ind)) {
            visited.insert(ind);
            s2.push(ind);
        }
    }
    // up
    if (0 <= i-SIZE && i-SIZE < SIZE*SIZE) {
        ind = i-SIZE;
        if (!visited.contains(ind)) {
            visited.insert(ind);
            s2.push(ind);
        }
    }
    // down
    if (0 <= i+SIZE && i+SIZE < SIZE*SIZE) {
        ind = i+SIZE;
        if (!visited.contains(ind)) {
            visited.insert(ind);
            s2.push(ind);
        }
    }
}

int traverse (const int x, const int y) {
    // setup breadth first search
    const int start = y*SIZE+x;
    int amt = 1;
    visited = std::set<int>();
    s1 = std::stack<int>();
    s2 = std::stack<int>();
    visited.insert(start);
    s1.push(start);

    while (!s1.empty()) {
        // precalc for neighborhood check
        const int ind = s1.top();
        s1.pop();
        const int dx = ind % SIZE; // unflattened y coordinate
        const int dy = ind / SIZE; // unflattened x coordinate
        int di;
        auto t = static_cast<TILES>(arr[dx][dy]);

        // check if the left neighbor is available
        if (adjacencyList[t][LEFT] == 1 ) {
            if (0 <= dy-1 && dy-1 < SIZE) {
                di = (dy-1)*SIZE+dx;
                // if we've visited this neighbor don't bother visiting it again
                if (!visited.contains(di)) {
                    visited.insert(di);
                    s2.push(di);
                    amt++;
                }
            }
        }

        // check if the right neighbor is available
        if (adjacencyList[t][RIGHT] == 1 ) {
            if (0 <= dy+1 && dy+1 < SIZE) {
                di = (dy+1)*SIZE+dx;
                // if we've visited this neighbor don't bother visiting it again
                if (!visited.contains(di)) {
                    visited.insert(di);
                    s2.push(di);
                    amt++;
                }
            }
        }

        // check if the upward neighbor is available
        if (adjacencyList[t][UP] == 1 ) {
            if (0 <= dx+1 && dx+1 < SIZE) {
                di = dy*SIZE+(dx+1);
                // if we've visited this neighbor don't bother visiting it again
                if (!visited.contains(di)) {
                    visited.insert(di);
                    s2.push(di);
                    amt++;
                }
            }
        }

        // check if the downward neighbor is available
        if (adjacencyList[t][DOWN] == 1 ) {
            if (0 <= dx-1 && dx-1 < SIZE) {
                di = dy*SIZE+(dx-1);
                // if we've visited this neighbor don't bother visiting it again
                if (!visited.contains(di)) {
                    visited.insert(di);
                    s2.push(di);
                    amt++;
                }
            }
        }

        if (s1.empty()) {
            // go to the next layer of the breadth first search
            s2.swap(s1);
        }
    }

    return amt;
}

/**
 * Traverse through the dungeon and remove room information for tiles that are not connected to the starting tile.
 * @param x starting x position
 * @param y starting y position
 * @return the return value of traverse(x, y), which returns the amount of rooms available to the starting tile.
 */
int traverseAndMask (const int x, const int y) {
    const int val = traverse(x, y);
    for (int a = 0; a < SIZE; a++) {
        for (int b = 0; b < SIZE; b++) {
            if (!visited.contains(b*SIZE+a)) {
                arr[a][b] = -1;
            }
        }
    }
    return val;
}

/**
 * Creates a dungeon at a starting point. the function assumes your starting tile has inward connections.
 * @param sx starting x coordinate
 * @param sy starting y coordinate
 * @param tile starting tile type
 */
void create (const int sx, const int sy, const TILES tile) {
    arr[sx][sy] = tile;

    std::vector<TILES> t1(TILES_SIZE);
    for (int a = 0; a < TILES_SIZE; a++) {
        t1[a] = static_cast<TILES>(a);
    }

    std::random_device rd;
    std::mt19937 g(rd());

    const int start = 0;
    visited = std::set<int>();
    s1 = std::stack<int>();
    s2 = std::stack<int>();
    visited.insert(start);
    s1.push(start);
    while (!s1.empty()) {
        // put neighbors in
        const int ind = s1.top();
        const int x = ind % SIZE;
        const int y = ind / SIZE;
        s1.pop();
        insertNeighbors(ind);

        // figure the tile
        if (ind != start) {
            // shuffle the order we test tiles to try to get a more interesting output
            std::ranges::shuffle(t1, g);
            for (int a = 0; a < TILES_SIZE; a++) {
                auto t = t1[a];

                // if the tile is valid to be placed, mark it and go to the next space.
                if (valid(x, y, t)) {
                    arr[x][y] = t;
                    break;
                }
            }
        }

        //
        if (s1.empty()) {
            s2.swap(s1);
        }

    }
}

/**
 * Timer function
 * @param start the starting point
 * @return the amount of time that has elapsed since the last time calculateTime was called.
 */
long double calculateTime (std::chrono::time_point<std::chrono::high_resolution_clock>* start) {
    auto end = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - *start);
    *start = std::chrono::high_resolution_clock::now();
    return static_cast<long double>(end.count())/1000000.0;
}

void reset () {
    if (arr != nullptr) {
        for (int a = 0; a < SIZE; a++) {
            memset(arr[a], -1, sizeof(int)*SIZE);
        }
    }
}

/**
 * Generates a dungeon and returns statistics. dungeon exists in global "arr" TILES array.
 * @return [time to generate, number of accessible rooms from start]
 */
std::vector<double> trial () {
    // setup
    reset();


    auto start = std::chrono::high_resolution_clock::now();
    int i;

    // create initial dungeon
    create(0, 0, FOUR_WAY);
    while (true) {
        i = traverseAndMask(0, 0);
        // if the amount of accessible tiles 'i' is not within the minimum, regenerate the dungeon and count again.
        if (i < MIN_NUM_TILES) {
            reset();
            create(0, 0, FOUR_WAY);
        }
        else {
            break;
        }
    }

    // optional printout
    // for (int a = SIZE-1; a >= 0; a--) {
    //     for (int b = 0; b < SIZE; b++) {
    //         std::cout << std::setw(2) << arr[a][b] << "  ";
    //     }
    //     std::cout << std::endl;
    // }
    return {static_cast<double>(calculateTime(&start)), static_cast<double>(i)};
}

/**
 * Writes current dungeon to output file. this output will use the charset defined on top of the file.
 * @param fname the file name
 */
void writeToFile (const std::string& fname) {
    std::ofstream file(fname);
    for (int a = SIZE-1; a >= 0; a--) {
        for (int b = 0; b < SIZE; b++) {
            if (charset[arr[a][b]] == charset[-1]) {
                file << "   ";
            }
            else {
                file << charset[arr[a][b]] << "  ";
            }
        }
        file << std::endl;
    }
    file.close();
}

int main (int argc, char** argv) {
    //setup
    buildTileAdjacency();
    double averageRuntime = 0;
    double maxRun = INT_MIN;
    double minRun = INT_MAX;
    double averageRoomCount = 0;

    int amt = 1;
    if (argc > 1) {
        amt = std::stoi(argv[1]);
    }

    // if the user gave on the command line an arg for size, reallocate.
    if (argc > 2) {
        SIZE = std::stoi(argv[2]);
        MIN_NUM_TILES = (SIZE*SIZE)/2;

        arr = static_cast<int **>(malloc(sizeof(int *) * SIZE));
        for (int a = 0; a < SIZE; a++) {
            arr[a] = static_cast<int *>(malloc(sizeof(int) * SIZE));
        }
    }

    if (arr == nullptr) {
        std::cout << "Error occured in generation. dungeon is null" << std::endl;
        return -1;
    }


    // tester, generates 'amt' amount of dungeons and gathers statistics
    for (int a = 0; a < amt; a++) {
        auto t = trial();
        averageRuntime += t[0];
        maxRun = std::max(maxRun, t[0]);
        minRun = std::min(minRun, t[0]);
        averageRoomCount += t[1];
    }
    averageRuntime /= amt;
    averageRoomCount /= amt;

    //
    std::cout << "Amount of trials: " << amt << std::endl;
    std::cout << "Average generation time: " << averageRuntime << std::endl;
    std::cout << "MAX: " << maxRun << "\tMIN: " << minRun << std::endl;
    std::cout << "Average room count " << averageRoomCount << "/" << MIN_NUM_TILES << std::endl;
    writeToFile("output.txt");

    // free arr
    for (int a = 0; a < SIZE; a++) {
        free(arr[a]);
    }
    free(arr);

    return 0;
}