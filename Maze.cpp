#include <iostream>
#include <fstream>
#include <utility>
#include <vector>
#include <algorithm>
#include <string>
#include <vector>
#include <random>
#include <cmath>

enum NodeType
{
    Path,
    Begin,
    End,
};

class PathNode
{
public:
    PathNode *group;
    NodeType type = Path;
    PathNode()
    : group(this)
    {}
    PathNode* Find()
    {
        if (this == group)
            return this;
        auto newGroup = group->Find();
        group = newGroup;
        return newGroup;
    }
    bool Union(PathNode* node)
    {
        if (node->Find() == Find())
            return false;
        Find()->group = node->Find();
        return true;
    }
};

class Wall
{
public:
    PathNode* node1 = nullptr;
    PathNode* node2 = nullptr;
    bool block = true;
    bool breakWall()
    {
        if (node1->Union(node2))
            block = false;
        return !block;
    }
};

class Maze
{
public:
    int width;
    int height;
    int pathWidth;
    int wallWidth;
    PathNode* maze;
    Wall* horizontalWalls;
    Wall* verticalWalls;
    Maze(int wid, int hei, int patw, int walw)
    : width(wid)
    , height(hei)
    , pathWidth(patw)
    , wallWidth(walw)
    {
        maze = new PathNode[width * height];
        horizontalWalls = new Wall[width * height];
        verticalWalls = new Wall[width * height];
        std::vector<Wall*> allWalls;
        for (int i = 0; i < width; ++i)
            for (int j = 0; j < height; ++j)
            {
                if (i < width - 1)
                {
                    auto wall = verticalWalls + j * width + i;
                    wall->node1 = maze + j * width + i;
                    wall->node2 = maze + j * width + i + 1;
                    allWalls.push_back(wall);
                }
                if (j < height - 1)
                {
                    auto wall = horizontalWalls + j * width + i;
                    wall->node1 = maze + j * width + i;
                    wall->node2 = maze + (j + 1) * width + i;
                    allWalls.push_back(wall);
                }
            }
        std::random_device rd;
        std::mt19937 rand(rd());
        std::shuffle(allWalls.begin(), allWalls.end(), rand);
        int count = width * height;
        for (auto wall: allWalls)
        {
            if (wall->breakWall())
                count -= 1;
            if (count == 1)
                break;
        }
        int b = abs((int)rand());
        b %= width * height;
        int e = abs((int)rand());
        e %= width * height;
        while (b == e)
        {
            e = abs((int)rand());
            e %= width * height;
        }
        maze[b].type = Begin;
        maze[e].type = End;
    }
    
    ~Maze()
    {
        delete[] maze;
        delete[] horizontalWalls;
        delete[] verticalWalls;
    }
    
    void write(std::string& fileName)
    {
        std::ofstream ofs(fileName, std::ios::binary);
        int fourb;
        short twob;
        // BMP header
        ofs.put('B');
        ofs.put('M');
        fourb = getFileSize();
        ofs.write(reinterpret_cast<char*>(&fourb), 4);
        fourb = 0;
        ofs.write(reinterpret_cast<char*>(&fourb), 4);
        fourb = 54;
        ofs.write(reinterpret_cast<char*>(&fourb), 4);
        // DIB header
        fourb = 40;
        ofs.write(reinterpret_cast<char*>(&fourb), 4);
        fourb = getBitmapWidth();
        ofs.write(reinterpret_cast<char*>(&fourb), 4);
        fourb = getBitmapHeight();
        ofs.write(reinterpret_cast<char*>(&fourb), 4);
        twob = 1;
        ofs.write(reinterpret_cast<char*>(&twob), 2);
        twob = 24;
        ofs.write(reinterpret_cast<char*>(&twob), 2);
        fourb = 0;
        ofs.write(reinterpret_cast<char*>(&fourb), 4);
        fourb = getBitmapSize();
        ofs.write(reinterpret_cast<char*>(&fourb), 4);
        fourb = 0xb12;
        ofs.write(reinterpret_cast<char*>(&fourb), 4);
        ofs.write(reinterpret_cast<char*>(&fourb), 4);
        fourb = 0;
        ofs.write(reinterpret_cast<char*>(&fourb), 4);
        ofs.write(reinterpret_cast<char*>(&fourb), 4);
        // Bitmap Array
        int rowWidth = getBitmapRowWidth();
        int padding = rowWidth - getBitmapWidth() * 3;
        for (int i = 0; i < wallWidth; ++i)
        {
            int rw = getBitmapWidth();
            for (int j = 0; j < rw; ++j)
                ofs.write(BLACK, 3);
            switch (padding) {
                case 3:
                    ofs.put(0);
                case 2:
                    ofs.put(0);
                case 1:
                    ofs.put(0);
                default:
                    break;
            }
        }
        for (int i = 0; i < height; ++i)
        {
            for (int k = 0; k < pathWidth; ++k)
            {
                for (int j = 0; j < wallWidth; ++j)
                    ofs.write(BLACK, 3);
                for (int j = 0; j < width; ++j)
                {
                    auto tp = maze[j + i * width].type;
                    const char* color = WHITE;
                    switch (tp) {
                        case Begin:
                            color = RED;
                            break;
                        case End:
                            color = GREEN;
                            break;
                        default:
                            break;
                    }
                    for (int l = 0; l < pathWidth; ++l)
                        ofs.write(color, 3);
                    if (verticalWalls[j + i * width].block)
                        color = BLACK;
                    else
                        color = WHITE;
                    for (int l = 0; l < wallWidth; ++l)
                        ofs.write(color, 3);
                }
                switch (padding) {
                    case 3:
                        ofs.put(0);
                    case 2:
                        ofs.put(0);
                    case 1:
                        ofs.put(0);
                    default:
                        break;
                }
            }
            for (int k = 0; k < wallWidth; ++k)
            {
                for (int j = 0; j < wallWidth; ++j)
                    ofs.write(BLACK, 3);
                for (int j = 0; j < width; ++j)
                {
                    const char* color = WHITE;
                    if (horizontalWalls[j + i * width].block)
                        color = BLACK;
                    for (int l = 0; l < pathWidth; ++l)
                        ofs.write(color, 3);
                    color = BLACK;
                    for (int l = 0; l < wallWidth; ++l)
                        ofs.write(color, 3);
                }
                switch (padding) {
                    case 3:
                        ofs.put(0);
                    case 2:
                        ofs.put(0);
                    case 1:
                        ofs.put(0);
                    default:
                        break;
                }
            }
        }
    }
    const char RED[3]={0,0,-1};
    const char GREEN[3]={0,-1,0};
    const char BLACK[3]={0,0,0};
    const char WHITE[3]={-1,-1,-1};
    
    void fill(char* array, int x, int y)
    {
        int xbase = x * (pathWidth + wallWidth) + wallWidth;
        int ybase = y * (pathWidth + wallWidth) + wallWidth;
        for (int i = 0; i < pathWidth; ++i)
        {
            auto tp = maze[x + y * width].type;
            char color = -1;
            for (int j = 0; j < pathWidth; ++j)
            {
                int index = (i + ybase) * getBitmapRowWidth() + (j + xbase) * 3;
                array[index] = (tp == End || tp == Begin)? 0: color;
                array[index + 1] = tp == End? 0: color;
                array[index + 2] = tp == Begin? 0: color;
            }
            if (verticalWalls[x + y * width].block)
                color = 0;
            for (int j = 0; j < wallWidth; ++j)
            {
                int index = (i + ybase) * getBitmapRowWidth() + (j + xbase + pathWidth) * 3;
                array[index] = color;
                array[index + 1] = color;
                array[index + 2] = color;
            }
        }
        for (int i = 0; i < wallWidth; ++i)
        {
            char color = -1;
            if (horizontalWalls[x + y * width].block)
                color = 0;
            for (int j = 0; j < pathWidth; ++j)
            {
                int index = (i + ybase + pathWidth) * getBitmapRowWidth() + (j + xbase) * 3;
                array[index] = color;
                array[index + 1] = color;
                array[index + 2] = color;
            }
            color = 0;
            for (int j = 0; j < wallWidth; ++j)
            {
                int index = (i + ybase + pathWidth) * getBitmapRowWidth() + (j + xbase + pathWidth) * 3;
                array[index] = color;
                array[index + 1] = color;
                array[index + 2] = color;
            }
        }
        
    }
    
    int getBitmapWidth()
    {
        return width * (pathWidth + wallWidth) + wallWidth;
    }
    
    int getBitmapHeight()
    {
        return height * (pathWidth + wallWidth) + wallWidth;
    }
    
    int getBitmapRowWidth()
    {
        return (24 * getBitmapWidth() + 31) / 32 * 4;
    }
    
    int getFileSize()
    {
        return getBitmapSize() + 54;
    }
    int getBitmapSize()
    {
        return getBitmapHeight() * getBitmapRowWidth();
    }
    char* getBitmapArray()
    {
        return new char[getBitmapSize()]();
    }
};

int main(int argc, const char * argv[]) {
    if (argc < 2)
    {
        std::cerr << argv[0] << " FileName [maze_width maze_height [path_width [wall_width]]]" << std::endl;
        return 0;
    }
    int width = 40;
    int height = 40;
    int pwidth = 2;
    int wwidth = 2;
    std::string name = argv[1];
    if (argc > 2)
    {
        if (argc < 4)
        {
            std::cerr << argv[0] << " FileName [maze_width maze_height [path_width [wall_width]]]" << std::endl;
            return 0;
        }
        width = atoi(argv[2]);
        height = atoi(argv[3]);
        if (width == 0)
            width = 40;
        if (height == 0)
            height = 40;
        if (argc > 4)
        {
            pwidth = atoi(argv[4]);
            if (pwidth == 0)
                pwidth = 2;
            if (argc > 5)
            {
                wwidth = atoi(argv[5]);
                if (wwidth == 0)
                    wwidth = 2;
            }
        }
    }
    Maze maze(width, height, pwidth, wwidth);
    std::cout << "Maze generated." << std::endl;
    maze.write(name);
    std::cout << "Image generated." << std::endl;
    return 0;
}
