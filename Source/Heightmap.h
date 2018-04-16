#pragma once
#include "math3d.h"
#include "Array2D.h"

class Heightmap : public Array2D<float> {
protected:
    // the difference between a heightmap and a simple 2d array of floats
    // is that the heightmap has scale factors to determine it's size in the world
    // (used for calculating normals etc.)
	float	scaleX;
	float	scaleY;
	float	scaleZ;

    // returns the normal vector of the terrain at the given x, y position
    // note: vector returned depends on scale factors (x, y, z) as these
    // affect the surface orientations
    vec3 GetNormal(int x, int z) const;

    // determines whether the angle of a point's normal
    // is within a specific threshold (used for determining
    // if a point is walkable or not)
    bool isWalkable(int x, int z, float angleThreshold) const;

public:
    Heightmap(int width, int height);
    Heightmap(int width, int height, float scaleX, float scaleY, float scaleZ);
	~Heightmap();

	float GetScaleY() const { return scaleY; }
	float GetScaleX() const { return scaleX; }
	float GetScaleZ() const { return scaleZ; }


	// calculates the mean value of the heightmaps corresponding slopemap
	float CalculateMeanSlope() const;
	// calculates the standard deviation of the slopemap
	float CalculateSlopeStandardDeviation(float meanSlope) const;
    // calculates the erosion score for this heightmap
	float CalculateErosionScore() const;


    // calculates the total walkable area given an angle threshold
    // the angleThreshold is the angle (between 0-90) of which players can walk upon
    float CalculateTotalWalkableArea(float angleThreshold) const;

    // calculates the number of regions in the map which the player can walk on
    int CalculateNumDisconnectedRegions(float angleThreshold) const;

	float GetScaledHeight(int x, int y) const;
	// returns an interpolated height value given the real world-x and z coords
	float GetWorldHeight(float x, float y) const;

    float GetMinValue() const;
    float GetMaxValue() const;
};
