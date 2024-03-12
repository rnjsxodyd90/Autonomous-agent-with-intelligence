#include <iostream>
#include <fstream>
#include <vector>
#include <stack>
#include <chrono>
#include <thread>

class MazeRunner {
public:
    MazeRunner(const std::vector<std::vector<int>>& maze)
        : maze_(maze), position_({0, 0}), direction_(Direction::Right), steps_(0),
          perceptiveField_(1), stepWidth_(1) {}

    // Function to move the agent
    void move() {
        int nextX = position_[0], nextY = position_[1];

        switch (direction_) {
            case Direction::Up:
                nextX--;
                break;
            case Direction::Down:
                nextX++;
                break;
            case Direction::Left:
                nextY--;
                break;
            case Direction::Right:
                nextY++;
                break;
        }

        if (isValidMove(nextX, nextY)) {
            position_ = {nextX, nextY};
            steps_++;
            updateGameplay();
        }
    }

    // Function to turn the agent left
    void turnLeft() {
        switch (direction_) {
            case Direction::Up:
                direction_ = Direction::Left;
                break;
            case Direction::Down:
                direction_ = Direction::Right;
                break;
            case Direction::Left:
                direction_ = Direction::Down;
                break;
            case Direction::Right:
                direction_ = Direction::Up;
                break;
        }
    }

    // Function to turn the agent right
    void turnRight() {
        switch (direction_) {
            case Direction::Up:
                direction_ = Direction::Right;
                break;
            case Direction::Down:
                direction_ = Direction::Left;
                break;
            case Direction::Left:
                direction_ = Direction::Up;
                break;
            case Direction::Right:
                direction_ = Direction::Down;
                break;
        }
    }

    // Function to display the maze
    void display() const {
        for (int i = 0; i < maze_.size(); ++i) {
            for (int j = 0; j < maze_[i].size(); ++j) {
                if (position_[0] == i && position_[1] == j) {
                    std::cout << "A "; // Display the agent
                } else {
                    std::cout << maze_[i][j] << " ";
                }
            }
            std::cout << '\n';
        }
    }

    // Function to check if the goal is reached
    bool isGoalReached() const {
        return maze_[position_[0]][position_[1]] == 3;
    }

    // Function to perform depth-first search to move the agent through the maze
    void performDFS() {
        std::stack<std::pair<int, int>> stack;
        std::vector<std::vector<bool>> visited(maze_.size(), std::vector<bool>(maze_[0].size(), false));

        stack.push({position_[0], position_[1]});

        while (!stack.empty()) {
            auto current = stack.top();
            stack.pop();

            position_ = {current.first, current.second};
            steps_++;
            updateGameplay();

            if (isGoalReached()) {
                std::cout << "Goal reached! Steps taken: " << steps_ << std::endl;
                break;
            }

            visited[current.first][current.second] = true;

            for (const auto& neighbor : getNeighbors(current.first, current.second)) {
                int x = neighbor.first, y = neighbor.second;
                if (isValidMove(x, y) && !visited[x][y]) {
                    stack.push({x, y});
                }
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Adjust the delay as needed

            display();
        }
    }

private:
    enum class Direction { Up, Down, Left, Right };

    std::vector<std::vector<int>> maze_;
    std::vector<int> position_;
    Direction direction_;
    int steps_;
    int perceptiveField_;
    int stepWidth_;

    // Function to check if the move is valid
    bool isValidMove(int x, int y) const {
        return x >= 0 && x < maze_.size() && y >= 0 && y < maze_[x].size() && maze_[x][y] == 0;
    }

    // Function to get neighboring cells
    std::vector<std::pair<int, int>> getNeighbors(int x, int y) const {
        std::vector<std::pair<int, int>> neighbors;
        if (x > 0) neighbors.emplace_back(x - 1, y);
        if (x < maze_.size() - 1) neighbors.emplace_back(x + 1, y);
        if (y > 0) neighbors.emplace_back(x, y - 1);
        if (y < maze_[x].size() - 1) neighbors.emplace_back(x, y + 1);
        return neighbors;
    }

    // Function to update gameplay based on the cell value
    void updateGameplay() {
        int cellValue = maze_[position_[0]][position_[1]];

        switch (cellValue) {
            case 4: // Googles
                perceptiveField_++;
                break;
            case 5: // Speed potion
                stepWidth_ = std::min(stepWidth_ + 1, 3);
                break;
            case 6: // Fog
                perceptiveField_ = std::max(perceptiveField_ - 1, 1);
                break;
            case 7: // Slowpoke potion
                stepWidth_ = std::max(stepWidth_ - 1, 1);
                break;
            case 3: // Goal
                std::cout << "Goal reached! Steps taken: " << steps_ << std::endl;
                break;
        }
    }
};

// Function to read maze from a file
std::vector<std::vector<int>> readMazeFromFile(const std::string& filename) {
    std::ifstream file(filename);
    std::vector<std::vector<int>> maze;

    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        // Handle the error, possibly exit the program
    }

    int value;
    while (file >> value) {
        maze.emplace_back();
        do {
            maze.back().push_back(value);
        } while (file.peek() == ',' && file >> std::ws >> value);
    }

    return maze;
}

// Function to display the maze
void displayMaze(const std::vector<std::vector<int>>& maze) {
    for (const auto& row : maze) {
        for (int value : row) {
            std::cout << value << " ";
        }
        std::cout << '\n';
    }
}

int main() {
    // Test the readMazeFromFile function
    std::vector<std::vector<int>> maze = readMazeFromFile("your_maze_file.txt");

    // Test the displayMaze function
    displayMaze(maze);

    // Test the movement, turning, and gameplay of the agent
    MazeRunner mazeRunner(maze);

    // Perform DFS to move the agent through the maze
    mazeRunner.performDFS();

    return 0;
}
