#pragma once

#include <glm/glm.hpp>
#include <algorithm>

using namespace glm;

inline constexpr size_t GetIndex(ivec3 pos, ivec3 size)
{
	return pos.x + size.x * pos.y + size.x * size.y * pos.z;
}

template <typename T, int X, int Y, int Z>
class Grid
{
public:
	T* data = new T[X * Y * Z]{};

	Grid()
	{

	}
	Grid(const Grid<T, X, Y, Z>& other)
	{
		memcpy(data, other.data, X * Y * Z * sizeof(T));
	}
	T& At(ivec3 pos)
	{
		return data[pos.x + X * pos.y + X * Y * pos.z];
	}
	void Fill(T value)
	{
		std::fill(data, data + X * Y * Z, value);
	}
	~Grid()
	{
		if(data)
			delete[] data;
	}

	constexpr int SizeX() { return X; }
	constexpr int SizeY() { return Y; }
	constexpr int SizeZ() { return Z; }
};

