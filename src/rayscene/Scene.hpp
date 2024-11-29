#pragma once

#include <vector>
#include "../raymath/Ray.hpp"
#include "../raymath/Color.hpp"
#include "Light.hpp"
#include "SceneObject.hpp"
#include "BSPTree.hpp"


class Scene
{
private:
  std::vector<SceneObject *> objects;
  std::vector<Light *> lights;
  BSPTree* bspTree; 


public:
  Scene();
  ~Scene();

  Color globalAmbient;

  void add(SceneObject *object);
  void addLight(Light *light);
  std::vector<Light *> getLights();

  void prepare();
  Color raycast(Ray &r, Ray &camera, int castCount, int maxCastCount);

  bool closestIntersection(Ray &r, Intersection &closest, CullingType culling);
  Color handleReflection(Ray &r, Ray &camera, int castCount, int maxCastCount, Intersection &intersection);
};
