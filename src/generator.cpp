//Code by Jacek Wieczorek

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <vector>

#include <maze.h>

class Node
{
public:
    int x, y; //Node position - little waste of memory, but it allows faster generation
    Node *parent; //Pointer to parent node
    char c = '#'; //Character to be displayed
    char dirs; //Directions that still haven't been explored
} ;

std::vector<Node> nodes; //Nodes array
int width, height, imperfection; //Maze dimensions


int init( )
{
    //Allocate memory for maze
    nodes = std::vector<Node>(0);
    for (int x = 0; x < width*height; x++)
        nodes.push_back(Node());
    if (nodes.empty()) return 1;

    //Setup crucial nodes
    for (int i = 0; i < width; i++ )
    {
        for (int j = 0; j < height; j++ )
        {
            Node &n = nodes.at(i+j*width);
            n.x = i;
            n.y = j;
            if ( (i * j) % 2 == 1)
            {
                n.dirs = 15; //Assume that all directions can be explored (4 youngest bits set)
                n.c = ' ';
            }
            else n.c = '#'; //Add walls between nodes
        }
    }
    return 0;
}

Node *link( Node *n )
{
    //Connects node to random neighbor (if possible) and returns
    //address of next node that should be visited

    int x, y;
    char dir;

    //Nothing can be done if null pointer is given - return
    if ( n == nullptr ) return  nullptr;

    //While there are directions still unexplored
    while ( n->dirs )
    {
        //Randomly pick one direction
        dir = ( 1 << ( rand( ) % 4 ) );

        //If it has already been explored - try again
        if ( ~n->dirs & dir ) continue;

        //Mark direction as explored
        n->dirs &= ~dir;

        //Depending on chosen direction
        switch ( dir )
        {
            //Check if it's possible to go right
            case 1:
                if ( n->x + 2 < width )
                {
                    x = n->x + 2;
                    y = n->y;
                }
                else continue;
                break;

                //Check if it's possible to go down
            case 2:
                if ( n->y + 2 < height )
                {
                    x = n->x;
                    y = n->y + 2;
                }
                else continue;
                break;

                //Check if it's possible to go left
            case 4:
                if ( n->x - 2 >= 0 )
                {
                    x = n->x - 2;
                    y = n->y;
                }
                else continue;
                break;

                //Check if it's possible to go up
            case 8:
                if ( n->y - 2 >= 0 )
                {
                    x = n->x;
                    y = n->y - 2;
                }
                else continue;
                break;
        }

        //Get destination node into pointer (makes things a tiny bit faster)
        Node &dest = nodes[x + y * width];

        //Make sure that destination node is not a wall
        if ( dest.c == ' ' )
        {
            //If destination is a linked node already - abort
            if ( dest.parent != nullptr ) continue;

            //Otherwise, adopt node
            dest.parent = n;

            //Remove wall between nodes
            Node &between = nodes[n->x + ( x - n->x ) / 2 + ( n->y + ( y - n->y ) / 2 ) * width];
            between.c = ' ';

            //Return address of the child node
            return &dest;
        }
    }

    //If nothing more can be done here - return parent's address
    return n->parent;
}

void eraseWalls() {
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            Node &n = nodes[i+j*width];
            if (n.c == '#') {
                int destroy = (rand()%100)+1;
                if (destroy < imperfection) n.c = ' ';
            }
        }
    }
}

void draw( )
{
    std::cout << "Drawing" << std::endl;
    ecn::Maze m = ecn::Maze(height, width);
    for (int i = 1; i<width-1; i++) {
        for (int j = 1; j < height-1; j++) {
            Node n = nodes[i+j*width];
            if (n.x == 0 && n.y == 0) std::cout << "Case 0 0 : " << n.c << std::endl;
            if (n.c == ' ') {
                m.dig(n.x, n.y);
            }
        }
    }
    m.save();
}

int main( int argc, char **argv )
{
    Node *start, *last;

    //Check argument count
    if ( argc < 4 )
    {
        fprintf( stderr, "%s: please specify maze dimensions and imperfection!\n", argv[0] );
        exit( 1 );
    }

    //Read maze dimensions from command line arguments
    if ( sscanf( argv[1], "%d", &width ) + sscanf( argv[2], "%d", &height ) < 2 )
    {
        fprintf( stderr, "%s: invalid maze size value!\n", argv[0] );
        exit( 1 );
    }

    //Allow only odd dimensions
    if ( !( width % 2 ) || !( height % 2 ) )
    {
        fprintf( stderr, "%s: dimensions must be odd!\n", argv[0] );
        exit( 1 );
    }

    //Do not allow negative dimensions
    if ( width <= 0 || height <= 0 )
    {
        fprintf( stderr, "%s: dimensions must be greater than 0!\n", argv[0] );
        exit( 1 );
    }

    //Read maze dimensions from command line arguments
    if ( sscanf( argv[3], "%d", &imperfection )<1)
    {
        fprintf( stderr, "%s: invalid imperfection value!\n", argv[0] );
        exit( 1 );
    }

    //Do not allow values outside [0,100]
    if ( imperfection < 0 || imperfection > 100 )
    {
        fprintf( stderr, "%s: imperfection should be between 0 (no wall erased) and 100 (every wall erased)!\n", argv[0] );
        exit( 1 );
    }

    //Seed random generator
    srand( time( NULL ) );

    //Initialize maze
    if ( init( ) )
    {
        fprintf( stderr, "%s: out of memory!\n", argv[0] );
        exit( 1 );
    }

    //Setup start node
    start = &nodes[1 + width];
    start->parent = start;
    last = start;

    int iter = 0;
    //Connect nodes until start node is reached and can't be left
    while ( ( last = link( last ) ) != start ) {
        std::cout << "Iter" << ++iter << std::endl;
    }
    eraseWalls();
    draw( );
}
