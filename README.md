# The-Optimal-Cleaner-Robot
Drawing inspiration from The-Robotic-Cleaner-Project and the automatic cleaning robots commonly found in households, I developed a C program that calculates the shortest path for a robot to clean all the dirt on a given 2D grid map.

This C program is designed to find the shortest path for a robot to clean all dirt tiles in a 2D grid world. The world is represented by a 2D grid, where each cell can be one of the following:

- X: Represents the robot's starting position.
- D: Represents a tile with dirt that needs to be cleaned.
- E: Represents an empty tile the robot can move to.
- W: Represents a wall tile that blocks movement.

The program uses the Breadth-First Search (BFS) algorithm to systematically explore all possible paths, ensuring that the shortest path to clean all dirt is found. It employs a queue data structure to manage states during the BFS process. Each state includes the current configuration of the world, the sequence of moves taken so far, and the count of moves.

Key features include:

- Checking for legal moves based on the grid boundaries and wall positions.
- Updating the robot's position and cleaning dirt as it moves.
- Tracking visited states to avoid redundant processing.
- The program also includes test cases to validate the solution for various grid configurations, including simple paths, paths with obstacles, and cases where some dirt tiles are unreachable. The output provides the optimal sequence of moves (u, d, l, r) required to clean all accessible dirt tiles.

## FOR MORE INFORMATION ABOUT THE ORIGINAL PROJECT
If you want to see the detailed description for the tasks related to this topic, then please refers to [The-Robotic-Cleaner-Project](https://github.com/minhtrietcancode/The-Robotic-Cleaner-Project)
