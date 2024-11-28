#include <iostream>
#include "Mesh.hpp"
#include "../raymath/Vector3.hpp"
#include "../objloader/OBJ_Loader.h"

Mesh::Mesh() : SceneObject()
{
}

Mesh::~Mesh()
{
    for (int i = 0; i < triangles.size(); ++i)
    {
        delete triangles[i];
    }
}

void Mesh::loadFromObj(std::string path)
{

    objl::Loader *loader = new objl::Loader();
    bool loadout = loader->LoadFile(path);

    if (loadout)
    {
        for (int i = 0; i < loader->LoadedMeshes.size(); i++)
        {
            objl::Mesh curMesh = loader->LoadedMeshes[i];

            for (int j = 0; j < curMesh.Indices.size(); j += 3)
            {
                Vector3 v1(
                    curMesh.Vertices[curMesh.Indices[j]].Position.X,
                    curMesh.Vertices[curMesh.Indices[j]].Position.Y,
                    curMesh.Vertices[curMesh.Indices[j]].Position.Z);
                Vector3 v2(
                    curMesh.Vertices[curMesh.Indices[j + 1]].Position.X,
                    curMesh.Vertices[curMesh.Indices[j + 1]].Position.Y,
                    curMesh.Vertices[curMesh.Indices[j + 1]].Position.Z);
                Vector3 v3(
                    curMesh.Vertices[curMesh.Indices[j + 2]].Position.X,
                    curMesh.Vertices[curMesh.Indices[j + 2]].Position.Y,
                    curMesh.Vertices[curMesh.Indices[j + 2]].Position.Z);

                Triangle *triangle = new Triangle(
                    v1,
                    v2,
                    v3);
                triangle->name = "T:" + std::to_string(j);
                triangle->ID = j;
                triangles.push_back(triangle);
            }
        }
    }

    this->applyTransform();
    delete loader;
}

void Mesh::applyTransform()
{
    for (int i = 0; i < triangles.size(); ++i)
    {
        triangles[i]->material = this->material;
        triangles[i]->transform = transform;
        triangles[i]->applyTransform();
    }
    updateBoundingBox();
}

bool Mesh::intersects(Ray &r, Intersection &intersection, CullingType culling)
{
    if (!boundingBox.intersects(r))
    {
        return false;
    }
    Intersection tInter;
    double closestDistanceSquared = -1;  // Store squared distance for comparison
    bool hit = false;

    for (int i = 0; i < triangles.size(); ++i)
    {
      

        if (triangles[i]->intersects(r, tInter, culling))
        {
            // Compute squared distance to avoid using sqrt
            double distanceSquared = (tInter.Position - r.GetPosition()).lengthSquared();

            // Check if this intersection is closer than the previous ones
            if (closestDistanceSquared < 0 || distanceSquared < closestDistanceSquared)
            {
                closestDistanceSquared = distanceSquared;
                intersection = tInter;  // Update the closest intersection
                hit = true;  // Mark that we found an intersection

                // Early exit if only the first intersection matters
                // Comment out or remove this if you need to continue processing all triangles
                // break;
            }
        }
    }

    return hit;  // Return true if we found at least one intersection
}

void Mesh::updateBoundingBox()
{
    if (triangles.empty())
    {
        boundingBox = AABB();
        return;
    }

   
    boundingBox = AABB(triangles[0]->boundingBox);

    for (int i = 1; i < triangles.size(); ++i)
    {
        
        boundingBox.subsume(triangles[i]->boundingBox);
    }
    std::cout << "Bounding box updated. Min: " << boundingBox << ", Max: " << boundingBox << std::endl;
}
