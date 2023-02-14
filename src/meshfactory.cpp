
#include "meshfactory.h"

#include <algorithm>
#include <cassert>
#include <math.h>

namespace pinta {

static const float EPSILON = 1.0;

static void arc(float x, float y, float cornerRadius, float startingAngle, float angle, int segments,
    std::vector<Vertex> &vertices, std::vector<GLushort> &indices);

static Mesh * createPlainRectangle(float x, float y, float w, float h, const Color &color);
static Mesh * createRoundRectangle(float x, float y, float w, float h, float cornerRadius, const Color &color,
    int segments, bool widthCollapsed, bool heightCollapsed);

Mesh * rectangle(float x, float y, float w, float h, float cornerRadius, const Color &color, int segments)
{
    assert(w > 0 && h > 0);

    if (cornerRadius <= 0) {
        return createPlainRectangle(x, y, w, h, color);
    } else {
        assert(segments > 0);
        cornerRadius = std::min(std::min(w, h)/2.0f, cornerRadius);
        bool widthCollapsed = std::abs(cornerRadius - w/2.0) < EPSILON;
        bool heightCollapsed = std::abs(cornerRadius - h/2.0) < EPSILON;
        if (widthCollapsed && heightCollapsed) {
            return circle(x, y, cornerRadius, color, segments * 4);
        } else {
            return createRoundRectangle(x, y, w, h, cornerRadius, color, segments, widthCollapsed, heightCollapsed);
        }
    }
}

Mesh * circle(float x, float y, float radius, const Color &color, int segments)
{
    Mesh *mesh = new Mesh(x, y, GL_TRIANGLE_FAN);
    std::vector<Vertex> vertices;
    std::vector<GLushort> indices;

    vertices.push_back(Vertex(0, 0));
    for (int i = 0; i < segments; i++) {
        float angle = (M_PI*2) * (static_cast<float>(i)/static_cast<float>(segments));
        vertices.push_back(Vertex(cos(angle) * radius, sin(angle) * radius));
    }
    indices.push_back(0);
    indices.push_back(1);
    for (int i = segments; i > 0; i--) {
        indices.push_back(i);
    }

    mesh->setVertices(vertices);
    mesh->setIndices(indices);
    mesh->setColor(color);
    return mesh;
}

void arc(float x, float y, float radius, float startAngle, float angle, int segments, std::vector<Vertex> &vertices, std::vector<GLushort> &indices)
{
    int firstIndex = vertices.size();
    vertices.push_back(Vertex(x, y));
    vertices.push_back(Vertex(x + cos(startAngle) * radius, y + sin(startAngle) * radius));
    for (int i = 0; i < segments; i++) {
        float currentAngle = startAngle + angle * ((i + 1.0)/segments);
        vertices.push_back(Vertex(x + cos(currentAngle) * radius, y + sin(currentAngle) * radius));
        indices.push_back(firstIndex);
        indices.push_back(firstIndex + i + 1);
        indices.push_back(firstIndex + i + 2);
    }
}

Mesh * createPlainRectangle(float x, float y, float w, float h, const Color &color)
{
    Mesh *mesh = new Mesh(x, y, GL_TRIANGLE_STRIP);
    mesh->setVertices({Vertex(-w/2.0, -h/2.0), Vertex(-w/2.0, h/2.0), Vertex(w/2.0, -h/2.0), Vertex(w/2.0, h/2.0)});
    mesh->setIndices({0, 1, 2, 3});
    mesh->setColor(color);
    return mesh;
}

Mesh * createRoundRectangle(float x, float y, float w, float h, float cornerRadius, const Color &color, int segments, bool widthCollapsed, bool heightCollapsed)
{
    Mesh *mesh = new Mesh(x, y, GL_TRIANGLES);
    std::vector<Vertex> vertices;
    std::vector<GLushort> indices;

    if (widthCollapsed || heightCollapsed) {
        float angle0;
        float angle1;
        if (widthCollapsed) {
            angle0 = 0;
            angle1 = M_PI;
        } else {
            angle0 = 3 * M_PI / 2.0;
            angle1 = M_PI / 2.0;
        }
        arc(w/2.0 - cornerRadius, h/2.0 - cornerRadius, cornerRadius, angle0, M_PI, segments * 2, vertices, indices);
        int middleIndex = vertices.size();
        arc(-w/2.0 + cornerRadius, -h/2.0 + cornerRadius, cornerRadius, angle1, M_PI, segments * 2, vertices, indices);
        indices.push_back(1);
        indices.push_back(0);
        indices.push_back(middleIndex);
        indices.push_back(middleIndex);
        indices.push_back(vertices.size() - 1);
        indices.push_back(1);
        indices.push_back(0);
        indices.push_back(middleIndex - 1);
        indices.push_back(middleIndex + 1);
        indices.push_back(middleIndex + 1);
        indices.push_back(middleIndex);
        indices.push_back(0);
    } else {
        arc(w/2.0 - cornerRadius, h/2.0 - cornerRadius, cornerRadius, 0, M_PI/2.0, segments, vertices, indices);
        int vertex0 = vertices.size();
        arc(-w/2.0 + cornerRadius, h/2.0 - cornerRadius, cornerRadius, M_PI/2.0, M_PI/2.0, segments, vertices, indices);
        int vertex1 = vertices.size();
        arc(-w/2.0 + cornerRadius, -h/2.0 + cornerRadius, cornerRadius, M_PI, M_PI/2.0, segments, vertices, indices);
        int vertex2 = vertices.size();
        arc(w/2.0 - cornerRadius, -h/2.0 + cornerRadius, cornerRadius, 3*M_PI/2.0, M_PI/2.0, segments, vertices, indices);
        indices.push_back(0);
        indices.push_back(vertex0 - 1);
        indices.push_back(vertex0 + 1);
        indices.push_back(vertex0 + 1);
        indices.push_back(vertex0);
        indices.push_back(0);
        indices.push_back(vertex0);
        indices.push_back(vertex1 - 1);
        indices.push_back(vertex1 + 1);
        indices.push_back(vertex1 + 1);
        indices.push_back(vertex1);
        indices.push_back(vertex0);
        indices.push_back(vertex1);
        indices.push_back(vertex2 - 1);
        indices.push_back(vertex2 + 1);
        indices.push_back(vertex2 + 1);
        indices.push_back(vertex2);
        indices.push_back(vertex1);
        indices.push_back(vertex2);
        indices.push_back(vertices.size() - 1);
        indices.push_back(1);
        indices.push_back(1);
        indices.push_back(0);
        indices.push_back(vertex2);
        indices.push_back(0);
        indices.push_back(vertex0);
        indices.push_back(vertex1);
        indices.push_back(vertex1);
        indices.push_back(vertex2);
        indices.push_back(0);
    }
    mesh->setVertices(vertices);
    mesh->setIndices(indices);
    mesh->setColor(color);
    return mesh;
}

}