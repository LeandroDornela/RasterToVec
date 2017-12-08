#ifndef VERTEX_H
#define VERTEX_H

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GL/freeglut.h>


class Vertex
{
    public:
        glm::vec3 position;
        glm::vec4 color;
        int index;
		bool visited = false;
		bool border = false;
		bool stack = false;
		long long int neighbors[8] = { -1,-1,-1,-1,-1,-1,-1,-1 };

        Vertex();
        void Render();
        virtual ~Vertex();

    private:

};

#endif // VERTEX_H
