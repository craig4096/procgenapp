
#include "Heightmap.h"
#include <iostream>
#include <cstdlib>
#include <deque>
using namespace std;

Heightmap::Heightmap(int width, int height)
    : Array2D<float>(width, height)
    , scaleX(0.7f)
    , scaleY(100.0f)
    , scaleZ(0.7f)
{
    ClearAll(0.0f);
}

Heightmap::Heightmap(int width, int height, float scaleX, float scaleY, float scaleZ)
    : Array2D<float>(width, height)
{
    this->scaleX = scaleX;
    this->scaleY = scaleY;
    this->scaleZ = scaleZ;

    ClearAll(0.0f);
}

Heightmap::~Heightmap() {
}

float Heightmap::GetScaledHeight(int x, int y) const
{
    return GetValue(x, y) * scaleY;
}

float Heightmap::CalculateMeanSlope() const
{
    float mean = 0.0f;
    for (int x = 0; x < GetWidth(); ++x)
    {
        for (int y = 0; y < GetHeight(); ++y)
        {
            float h = GetValue(x, y);
            int nindices[4][2] = {
                { x - 1, y },
                { x + 1, y },
                { x, y - 1 },
                { x, y + 1 }
            };

            float maxSlope = -1000000.0f;
            for (int i = 0; i < 4; ++i)
            {
                if (IndexValid(nindices[i][0], nindices[i][1]))
                {
                    float slope = h - GetValue(nindices[i][0], nindices[i][1]);
                    if (slope > maxSlope)
                    {
                        maxSlope = slope;
                    }
                }
            }
            mean += maxSlope;
        }
    }
    return mean / (float)(GetWidth() * GetHeight());
}

float sqrd(float a) { return a * a; }

float Heightmap::CalculateSlopeStandardDeviation(float average) const
{
    float sd = 0.0f;
    for (int x = 0; x < GetWidth(); ++x)
    {
        for (int y = 0; y < GetHeight(); ++y)
        {
            float h = GetValue(x, y);
            int nindices[4][2] = {
                { x - 1, y },
                { x + 1, y },
                { x, y - 1 },
                { x, y + 1 }
            };

            float maxSlope = -1000000.0f;
            for (int i = 0; i < 4; ++i)
            {
                if (IndexValid(nindices[i][0], nindices[i][1]))
                {
                    float slope = h - GetValue(nindices[i][0], nindices[i][1]);
                    if (slope > maxSlope)
                    {
                        maxSlope = slope;
                    }
                }
            }
            sd += sqrd(maxSlope - average);
        }
    }
    return sqrt(sd / (float)(GetWidth() * GetHeight()));
}


float Heightmap::CalculateErosionScore() const
{
    float mean = CalculateMeanSlope();
    return CalculateSlopeStandardDeviation(mean) / mean;
}


float Heightmap::GetWorldHeight(float xr, float zr) const
{
    // convert coordinates into normal space (i.e without scale factors)
    float xx = (xr / scaleX) + (GetWidth() * 0.5f);
    float zz = (zr / scaleZ) + (GetHeight() * 0.5f);

    // get the location of these coordinates in the heightmap
    int x = (int)floor(xx);
    int z = (int)floor(zz);

    // check for out of bounds
    if (x < 0 || z < 0 || x > GetWidth() - 2 || z > GetHeight() - 2) {
        return 0.0f;
    }

    // get the four heights
    float a = GetValue(x, z);
    float b = GetValue(x + 1, z);
    float c = GetValue(x + 1, z + 1);
    float d = GetValue(x, z + 1);

    // use bilinear filtering to calculate the height inbetween these values
    float xratio = (xx - (float)x);
    float ab = a + ((b - a) * xratio);
    float dc = d + ((c - d) * xratio);
    float zratio = (zz - (float)z);

    return (ab + ((dc - ab) * zratio)) * scaleY;
}


vec3 Heightmap::GetNormal(int x, int z) const
{
    // sample 8 world heights from 8 neighbours of x and y
    int neighbours[8][2] = {
        x - 1, z - 1,
        x,   z - 1,
        x + 1, z - 1,
        x - 1, z,
        x + 1, z,
        x - 1, z + 1,
        x,   z + 1,
        x + 1, z + 1
    };
    vec3 accum(0, 0, 0);
    vec3 centrePos(x * scaleX, GetValue(x, z) * scaleY, z * scaleZ);
    // for all valid neighbours:
    for (int i = 0; i < 8; ++i)
    {
        int nx = neighbours[i][0];
        int ny = neighbours[i][1];
        if (IndexValid(nx, ny))
        {
            // construct a line seg between the center pos and the neighbour pos
            vec3 neighbourPos(nx * scaleX,
                GetValue(nx, ny) * scaleY,
                ny * scaleZ);

            vec3 ab(neighbourPos - centrePos);
            // cross this vector with the up axis
            vec3 tmp(ab.crossProduct(vec3(0, 1, 0)));
            // cross again to find normal
            vec3 unorm(tmp.crossProduct(ab));
            accum += unorm;
        }
    }
    return accum.normalized();
}

float Heightmap::GetMinValue() const
{
    float min = GetValue(0);
    for (int i = 1; i < GetSize(); ++i)
    {
        if (GetValue(i) < min)
        {
            min = GetValue(i);
        }
    }
    return min;
}

float Heightmap::GetMaxValue() const
{
    float max = GetValue(0);
    for (int i = 1; i < GetSize(); ++i)
    {
        if (GetValue(i) > max)
        {
            max = GetValue(i);
        }
    }
    return max;
}


bool Heightmap::isWalkable(int x, int z, float angleThreshold) const
{
    // get the surface normal
    vec3 normal = GetNormal(x, z);
    // check if the angle this normal makes with the up axis
    // is greater than the threshold
    float angle = (normal.calcAngle(vec3(0, 1, 0)) * RAD_TO_DEG);
    //cout << "angle: "<< normal <<" "<< angle << endl;
    return angle <= angleThreshold;
}

float Heightmap::CalculateTotalWalkableArea(float angleThreshold) const
{
    // total number of patches of walkable space
    int total = 0;
    // loop through all points
    for (int x = 0; x < GetWidth(); ++x)
    {
        for (int z = 0; z < GetHeight(); ++z)
        {
            // check if the point is walkable
            if (isWalkable(x, z, angleThreshold))
            {
                total++;
            }
        }
    }

    // return value in meters squared (hence sqrt)
    return sqrt(total * (scaleX * scaleZ));
}

struct NodeD {
    NodeD(int x, int z) : x(x), z(z) {}
    int x, z;
};


int Heightmap::CalculateNumDisconnectedRegions(float angleThreshold) const
{
    // create a tmp walkable array - which areas the player can walk or not
    Array2D<bool> walkable(GetWidth(), GetHeight());
    walkable.ClearAll(false);
    // fill the array
    for (int x = 0; x < GetWidth(); ++x)
    {
        for (int z = 0; z < GetHeight(); ++z)
        {
            walkable.SetValue(x, z, isWalkable(x, z, angleThreshold));
        }
    }
    int walkableAreas = 0;
    for (int i = 0; i < walkable.GetSize(); ++i)
    {
        if (walkable.GetValue(i)) walkableAreas++;
    }

    // create a checked array - stores which areas have been checked
    Array2D<bool> checked(GetWidth(), GetHeight());
    checked.ClearAll(false);

    // find an area the player can walk and perform a flood fill
    int numDisconnectedRegions = 0;
    int areasChecked = 0;
    while (true)
    {
        // find an unchecked (and walkable) point
        int xx, zz;
        bool found = false;
        for (int x = 0; x < GetWidth() && !found; ++x)
        {
            for (int z = 0; z < GetHeight() && !found; ++z)
            {
                if (!checked.GetValue(x, z) && walkable.GetValue(x, z))
                {
                    found = true;
                    xx = x;
                    zz = z;
                }
            }
        }
        // if a point was found
        if (found)
        {
            // perform flood fill on this point
            /*
            Flood-fill (node, target-color, replacement-color):
             1. Set Q to the empty queue.
             2. Add node to the end of Q.
             4. While Q is not empty:
             5.     Set n equal to the last element of Q.
             7.     Remove last element from Q.
             8.     If the color of n is equal to target-color:
             9.         Set the color of n to replacement-color.
             10.        Add west node to end of Q.
             11.        Add east node to end of Q.
             12.        Add north node to end of Q.
             13.        Add south node to end of Q.
             14. Return
            */
            // previous recursive version was crashing due to stack overflow!
            // used queue instead
            std::deque<NodeD> q;
            q.push_back(NodeD(xx, zz));

            while (!q.empty())
            {
                NodeD n = q.back();
                q.pop_back();
                if (checked.IndexValid(n.x, n.z) && !checked(n.x, n.z) && walkable(n.x, n.z))
                {
                    checked(n.x, n.z) = true;
                    areasChecked++;
                    q.push_back(NodeD(n.x, n.z + 1));
                    q.push_back(NodeD(n.x, n.z - 1));
                    q.push_back(NodeD(n.x + 1, n.z));
                    q.push_back(NodeD(n.x - 1, n.z));
                }
            }
            // function will terminate when all accessible areas have been
            // traversed and checked, therefore increment numDisconnectRegions
            numDisconnectedRegions++;
        }
        else
        {
            // no valid points found - algorithm terminates
            break;
        }
    }

    return numDisconnectedRegions;
}

