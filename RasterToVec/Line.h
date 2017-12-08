#ifndef LINE_H
#define LINE_H

#include "Vertex.h"
#include <vector>

class Line
{
    public:
        std::vector<GLushort> vertexesIDs;
        glm::vec4 color;

        Line();
        virtual ~Line();

    private:

};

#endif // LINE_H
