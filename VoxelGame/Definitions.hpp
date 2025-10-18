#pragma once

#include "SimView/SimView.hpp"
#include <glm/glm.hpp>

using namespace SimView;
using namespace glm;

typedef SimView::u16 blockType;

constexpr int CHUNK_SPAN = 16;
constexpr int CHUNK_AREA = CHUNK_SPAN * CHUNK_SPAN;
constexpr int CHUNK_VOLUME = CHUNK_AREA * CHUNK_SPAN;

class World;