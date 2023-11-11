#include "ShapeDatabase.h"
#include <fstream>
#include <stdexcept>
#include <iostream>
#include <algorithm>
using namespace std;

ShapeDatabase::ShapeDatabase()
{
    static const float FMAX = 100000.0f;

    globalConstraint.min = vec3(-FMAX, -FMAX, -FMAX);
    globalConstraint.max = vec3(FMAX, FMAX, FMAX);
    gridCellSize = vec3(5.0f, 5.0f, 5.0f);
}

ShapeDatabase::~ShapeDatabase()
{
    // delete all mesh objects in the symbolMeshMap
    for (map<string, Mesh*>::iterator i = meshMap.begin(); i != meshMap.end(); ++i)
    {
        delete i->second;
    }
}

void ShapeDatabase::loadSymbolMeshMap(const char* filename)
{
    // load the symbol mesh map
    std::ifstream ifs(filename);
    if (!ifs.is_open())
    {
        throw std::logic_error("could not open mapping file");
    }

    while (!ifs.eof())
    {
        std::string symbol, mesh;
        ifs >> symbol >> mesh;
        if (symbol.empty() || mesh.empty()) break;

        // if the mesh has aready been loaded
        map<string, Mesh*>::const_iterator i = meshMap.find(mesh);
        if (i != meshMap.end())
        {
            // assign (already loaded) mesh to symbol map
            symbolMeshMap[symbol] = meshMap[mesh];
        }
        else
        {
            //cout << "adding symbol to mesh map: " << symbol << endl;
            cout << "loading mesh: " << symbol << endl;
            Mesh* m = new Mesh(mesh);
            symbolMeshMap[symbol] = m;
            // add loaded mesh to meshMap
            meshMap[mesh] = m;
        }
    }
}

ShapeDatabase::GridRange ShapeDatabase::getGridRange(const BoundingBox& aabb) const
{
    GridRange g;
    g.startX = (int)floor(aabb.min.x / gridCellSize.x);
    g.startY = (int)floor(aabb.min.y / gridCellSize.y);
    g.startZ = (int)floor(aabb.min.z / gridCellSize.z);
    g.endX = (int)ceil(aabb.max.x / gridCellSize.x);
    g.endY = (int)ceil(aabb.max.y / gridCellSize.y);
    g.endZ = (int)ceil(aabb.max.z / gridCellSize.z);
    return g;
}


bool ShapeDatabase::occlusionTest(const BoundingBox& aabb, const Shape* ignore) const
{
    /*
    // first find the range of grid cells that the shape intersects
    // with
    GridRange g = getGridRange(aabb);

    // loop through all virtual grid cells
    Vec3i v;
    //cout << "occlusion test grid cells:" << endl;
    //cout << "\t(" << g.startX << " " << g.startY << " " << g.startZ << ")" << endl;
    //cout << "\t(" << g.endX << " " << g.endY << " " << g.endZ << ")" << endl;
    for(v.x = g.startX; v.x < g.endX; ++v.x) {
        for(v.y = g.startY; v.y < g.endY; ++v.y) {
            for(v.z = g.startZ; v.z < g.endZ; ++v.z) {
                // check if there is an entry for this grid cell
                // in the grid map
                map<Vec3i, vector<const Shape*> >::const_iterator i = grid.find(v);
                if(i != grid.end()) {
                    // ifso then go through the list of shapes that intersect with this cell
                    // and check to see if they intersect with 'shape'
                    const vector<const Shape*>& list = i->second;
                    for(int j = 0; j < list.size(); ++j) {
                        // ignore occlusion test if set to
                        if(list[j] != ignore) {
                            if(list[j]->aabb.intersects(aabb)) {
                                return true;
                            }
                        }
                    }
                }
            }
        }
    }
    return false;
*/
    if (!aabb.inside(globalConstraint))
    {
        return true;
    }

    for (list<Shape>::const_iterator i = shapes.begin(); i != shapes.end(); ++i)
    {
        if (&(*i) != ignore && intersects(aabb, i->aabb))
        {
            return true;
        }
    }
    return false;
}

bool ShapeDatabase::intersects(const BoundingBox& a, const BoundingBox& b)
{
    static const vec3 EPSILON(0.001f, 0.001f, 0.001f);
    // shrink the 'b' bounding box by EPSILON
    BoundingBox tmp;
    tmp.min = b.min + EPSILON;
    tmp.max = b.max - EPSILON;
    // standard test
    return a.intersects(tmp);
}

ShapeDatabase::iterator ShapeDatabase::insert(iterator pos, const Shape& shape)
{
    //cout << "inserting shape " << shape.symbol << endl;
    // insert the shape into the list, storing the new pos in rval
    iterator rval = shapes.insert(pos, shape);
    /*
    // need to insert into the grid map aswell
    // find grid range
    GridRange g = getGridRange(shape.aabb);
    // iterate through all intersecting cells
    Vec3i v;
    //cout << "inserting shape to grid cells:" << endl;
    //cout << "\t(" << g.startX << " " << g.startY << " " << g.startZ << ")" << endl;
   // cout << "\t(" << g.endX << " " << g.endY << " " << g.endZ << ")" << endl;
    for(v.x = g.startX; v.x < g.endX; ++v.x) {
        for(v.y = g.startY; v.y < g.endY; ++v.y) {
            for(v.z = g.startZ; v.z < g.endZ; ++v.z) {
                // find the entry in the grid map (if one exists)
                map<Vec3i, vector<const Shape*> >::iterator i = grid.find(v);
                // if an entry exists
                if(i != grid.end()) {
                    // add to the list of intersecting shapes for this cell
                    i->second.push_back(&(*rval));
                } else {
                    // create new list
                    vector<const Shape*> s;
                    s.push_back(&(*rval));
                    // add to the map
                    grid[v] = s;
                }
            }
        }
    }
    */
    return rval;

    cout << "end inserting shape" << endl;
}

ShapeDatabase::iterator ShapeDatabase::remove(iterator pos)
{
    /*
   // cout << "removing shape" << endl;
    // need to remove this from the grid map aswell
    // but can't simply iterate through the map and remove all
    // references as this would take too long, first find the range of grid cells
    // the shape intersects with
    GridRange g = getGridRange((*pos).aabb);
    //cout << "removing shape from grid cells:" << endl;
    //cout << "\t(" << g.startX << " " << g.startY << " " << g.startZ << ")" << endl;
    //cout << "\t(" << g.endX << " " << g.endY << " " << g.endZ << ")" << endl;
    // iterate through all cells
    Vec3i v;
    for(v.x = g.startX; v.x < g.endX; ++v.x) {
        for(v.y = g.startY; v.y < g.endY; ++v.y) {
            for(v.z = g.startZ; v.z < g.endZ; ++v.z) {
                //cout << "at("<< v.x << " " << v.y << " " << v.z << ")" << endl;
                // now find the entry in the list
                map<Vec3i, vector<const Shape*> >::iterator i = grid.find(v);
                // if an entry exists
                if(i != grid.end()) {
                    // remove the referece to Shape*
                    vector<const Shape*>::iterator ii = std::find(i->second.begin(), i->second.end(), &(*pos));
                    if(ii != i->second.end()) {
                        i->second.erase(ii);
                    }
                }
            }
        }
    }
    // remove shape from shapes array
   // cout << "end remove shape" << endl;
   */
    return shapes.erase(pos);
}


void ShapeDatabase::setGlobalConstraint(const BoundingBox& bb)
{
    globalConstraint = bb;
}

void ShapeDatabase::clear()
{
    shapes.clear();
    grid.clear();
}
