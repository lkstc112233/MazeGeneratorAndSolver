All : BFS DFS AAsterisk MazeGen

BFS : BFS.cpp
	g++ -std=c++14 -o BFS BFS.cpp

DFS : DFS.cpp
	g++ -std=c++14 -o DFS DFS.cpp

AAsterisk : AAsterisk.cpp
	g++ -std=c++14 -o AAsterisk AAsterisk.cpp

MazeGen : Maze.cpp
	g++ -std=c++14 -o MazeGen Maze.cpp

clean :
	rm -f *.o BFS DFS AAsterisk MazeGen

clear :
	rm -f *-result *.bmp