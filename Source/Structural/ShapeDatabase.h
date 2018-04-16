#ifndef SHAPEDATABASE_H
#define SHAPEDATABASE_H
#include "Shape.h"
#include <vector>
#include <map>
#include <list>
#include "Mesh.h"

/*
ShapeDatabase represents a collection of Shape objects, it is used for
managing the insertion and removale of shape objects along with perfoming
an optimized occlusion test to see if shapes overlap
*/
class ShapeDatabase {
    // the shapes array
    std::list<Shape> shapes;

    struct Vec3i {
        int x, y, z;

        bool operator <(const Vec3i& other) const {
            return x < other.x && y < other.y && z < other.z;
        }
    };
    // maps grid cell coordinates to lists of intersecting shapes
    // used for optimizing the occlusion/intersection test
    std::map<Vec3i, std::vector<const Shape*> > grid;


    // the size of a grid cell
    vec3 gridCellSize;

    // describes a range of grid cells in the virtual grid
    // (used for optimizing the occlusion test)
    struct GridRange {
        int startX, endX, startY, endY, startZ, endZ;
    };

    // occlusion test will also incorporate a global
    // anything outside this bounding box will be discarded as a candidate rule
    BoundingBox globalConstraint;

    // returns a grid range based off of gridCellSize given a real
    // valued bounding box
    GridRange getGridRange(const BoundingBox& aabb) const;

    // maps shape symbols to mesh objects
    SymbolMeshMap symbolMeshMap;
    // maps filenames to mesh objects
    // for caching mesh objects (so they don;t get loaded more than once)
    std::map<std::string, Mesh*> meshMap;

    // intersection test of two bounding boxes - used for occlusion test
    static bool intersects(const BoundingBox& a, const BoundingBox& b);

public:
    ShapeDatabase();
    ~ShapeDatabase();

    typedef std::list<Shape>::iterator iterator;

    iterator begin() { return shapes.begin(); }
    iterator end() { return shapes.end(); }


    // loads the file mapping shape symbols to mesh objects
    void loadSymbolMeshMap(const char* filename);

    // inserts a shapes into the database at the specific iterator position
    // returns the position of the newely inserted shape
    iterator insert(iterator, const Shape& shape);

    // removes a specific shape at the specified iterator pos
    iterator remove(iterator);

    // a shape database is associated with a mapping between shape symbols and
    // mesh objects, it needs this mapping to calculate shape bounding boxes and
    // perform occlusion tests
    const SymbolMeshMap& getSymbolMeshMap() const { return symbolMeshMap; }

    // determines whether a current shape is intersecting with the specified
    // bounding box, ignore - address of shape to be ignore from occlusion test
    // (usually lhs shape of shape rule)
    bool occlusionTest(const BoundingBox& aabb, const Shape* ignore) const;

    // returns the number of shapes in the database
   // int getShapeCount() const { return shapes.size(); }

    // returns the shape at the specific index
    //const Shape& getShape(int index) const { return shapes[index]; }

    // clears all shapes and any entries in shapeBounds map
    void clear();

    void setGlobalConstraint(const BoundingBox&);
};

#endif // SHAPEDATABASE_H
