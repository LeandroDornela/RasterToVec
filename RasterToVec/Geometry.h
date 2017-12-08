#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "Vertex.h"
#include <vector>


class Geometry
{
    public:
        std::vector<GLushort> vertexesIDs;

        Geometry();
        virtual ~Geometry();

    private:
};

#endif // GEOMETRY_H
