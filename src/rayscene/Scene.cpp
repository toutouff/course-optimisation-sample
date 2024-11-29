#include <iostream>
#include <limits>
#include "Scene.hpp"
#include "Intersection.hpp"

Scene::Scene()  : bspTree(nullptr)
{
}

Scene::~Scene()
{
  delete bspTree;

  for (int i = 0; i < objects.size(); ++i)
  {
    delete objects[i];
  }

  for (int i = 0; i < lights.size(); ++i)
  {
    delete lights[i];
  }
}

void Scene::add(SceneObject *object)
{
  objects.push_back(object);
}

void Scene::addLight(Light *light)
{
  lights.push_back(light);
}

void Scene::prepare()
{
  for (int i = 0; i < objects.size(); ++i)
  {
    objects[i]->applyTransform();
  }
  bspTree = new BSPTree(objects, 0);

}

std::vector<Light *> Scene::getLights()
{
  return lights;
}

bool Scene::closestIntersection(Ray &r, Intersection &closest, CullingType culling)
{
    std::vector<SceneObject*> potentialObjects;

  if (bspTree)
  {
    bspTree->query(r, potentialObjects);
  }
  else
  {
    potentialObjects = objects;
  }

  double closestDistance = std::numeric_limits<double>::max();
  bool found = false;

  for (const auto& object : potentialObjects)
  {
    Intersection intersection;
    if (object->intersects(r, intersection, culling))
    {
      double distance = (intersection.Position - r.GetPosition()).length();
      if (distance < closestDistance)
      {
        closestDistance = distance;
        closest = intersection;
        found = true;
      }
    }
  }

  return found;  // If a valid intersection was found
}

Color Scene::handleReflection(Ray &r, Ray &camera, int castCount, int maxCastCount, Intersection &intersection)
{
    Color reflectionColor(0, 0, 0);
    if (castCount < maxCastCount && intersection.Mat->cReflection > 0)
    {
        Vector3 reflectDir = r.GetDirection().reflect(intersection.Normal);
        Vector3 origin = intersection.Position + (reflectDir * COMPARE_ERROR_CONSTANT);
        Ray reflectRay(origin, reflectDir);

        reflectionColor = raycast(reflectRay, camera, castCount + 1, maxCastCount);
        reflectionColor = reflectionColor * intersection.Mat->cReflection;
    }
    return reflectionColor;
}

Color Scene::raycast(Ray &r, Ray &camera, int castCount, int maxCastCount)
{
    Color pixel(0, 0, 0);
    Intersection intersection;

    if (closestIntersection(r, intersection, CULLING_FRONT))
    {
        intersection.View = (camera.GetPosition() - intersection.Position).normalize();

        if (intersection.Mat != NULL)
        {
            pixel = pixel + intersection.Mat->render(r, camera, &intersection, this);

            // Add reflection handling
            Color reflectColor = handleReflection(r, camera, castCount, maxCastCount, intersection);
            pixel = pixel + reflectColor;
        }
    }

    return pixel;
}
