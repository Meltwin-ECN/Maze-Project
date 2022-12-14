#include <a_star.h>
#include <maze.h>

using namespace std;
using namespace ecn;

// a node is an x-y position, we move from 1 each time
class Position : public Point
{
    typedef std::unique_ptr<Position> PositionPtr;

public:
    // constructor from coordinates
    Position(int _x, int _y) : Point(_x, _y) {}

    // constructor from base ecn::Point
    Position(ecn::Point p) : Point(p.x, p.y) {}

    int distToParent()
    {
        // in cell-based motion, the distance to the parent is always 1
        return 1;
    }

    std::vector<PositionPtr> children()
    {
        // this method should return  all positions reachable from this one
        std::vector<PositionPtr> generated;
        for (int i = -1; i<=1; i++) {
            for (int j = -1; j <= 1; j++) {
                if (maze.isFree(x+i, y+j))
                    generated.push_back(std::make_unique<Position>(x+i, y+j));
            }
        }


        return generated;
    }
};



int main( int argc, char **argv )
{
    // load file
    std::string filename = "../mazes/maze.png";
    if(argc == 2)
        filename = std::string(argv[1]);

    // let Point know about this maze
    Position::maze.load(filename);

    // initial and goal positions as Position's
    Position start = Position::maze.start(),
             goal = Position::maze.end();

    // call A* algorithm
    auto path = ecn::Astar(start, goal);
    std::cout << "Astar passed" << std::endl;

    for (auto &cell : path) {
        Position::maze.passThrough(cell.x, cell.y);
    }

    std::cout << Position::maze.path.size() << std::endl;

    // save final image
    Position::maze.saveSolution("cell");
    //cv::waitKey(0);

}
