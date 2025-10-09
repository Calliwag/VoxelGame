#pragma once

#include "Plane.hpp"
#include "Face.hpp"
#include <cmath>

class Frustum
{
public:
	vec3 pos;
	Plane planes[6];

	Frustum() {};
	Frustum(vec3 cameraPos, vec3 cameraDir, float nearDist, float farDist, float fovY, float aspect /* width/height */)
	{
		pos = cameraPos;
		cameraDir = normalize(cameraDir);

		planes[0] = Plane(cameraDir, nearDist);
		planes[1] = Plane(-cameraDir, -farDist);

		vec3 rightDir = normalize(vec3(cameraDir.y, -cameraDir.x, 0));
		vec3 upDir = cross(rightDir, cameraDir);
		
		vec3 uVec = normalize(cameraDir * cos(fovY / 2) + upDir * sin(fovY / 2));
		vec3 uNorm = cross(uVec, rightDir);
		planes[2] = Plane(uNorm, 0);

		vec3 dVec = normalize(cameraDir * cos(fovY / 2) - upDir * sin(fovY / 2));
		vec3 dNorm = cross(rightDir, dVec);
		planes[3] = Plane(dNorm, 0);

		float fovX = 2 * atanf(tan(fovY / 2) * aspect);

		vec3 rVec = normalize(cameraDir * cos(fovX / 2) + rightDir * sin(fovX / 2));
		vec3 rNorm = cross(upDir, rVec);
		planes[4] = Plane(rNorm, 0);

		vec3 lVec = normalize(cameraDir * cos(fovX / 2) - rightDir * sin(fovX / 2));
		vec3 lNorm = cross(lVec, upDir);
		planes[5] = Plane(lNorm, 0);
	}
	bool CullPoint(vec3 point)
	{
		for (int i = 0; i < 6; i++)
		{
			if (planes[i].GetSignedDist(point - pos) < 0)
				return false;
		}
		return true;
	}
	bool CullFace(Face& face, vec3 chunkOffset)
	{
		for (int p = 0; p < 6; p++)
		{
			int in = 0;
			for (int i = 0; i < 4; i++) {
				if (planes[p].GetSignedDist(((vec3)face.verts[i] + chunkOffset) - pos) > 0)
					in++;
			}
			if (in == 0)
				return false;
		}
		return true;
	}
	bool CullAABB(vec3 nPoint, vec3 pPoint)
	{
		// Use our min max to define eight corners
		vec3 corners[8] = {
			{nPoint.x, nPoint.y, nPoint.z}, // x y z
			{pPoint.x, nPoint.y, nPoint.z}, // X y z
			{nPoint.x, pPoint.y, nPoint.z}, // x Y z
			{pPoint.x, pPoint.y, nPoint.z}, // X Y z

			{nPoint.x, nPoint.y, pPoint.z}, // x y Z
			{pPoint.x, nPoint.y, pPoint.z}, // X y Z
			{nPoint.x, pPoint.y, pPoint.z}, // x Y Z
			{pPoint.x, pPoint.y, pPoint.z}, // X Y Z
		};

		for (int p = 0; p < 6; p++)
		{
			int in = 0;
			for (int i = 0; i < 8; i++) {
				if (planes[p].GetSignedDist(corners[i] - pos) > 0)
					in++;
			}
			if (in == 0)
				return false;
		}
		return true;
	}
};

