#pragma once
#include "../raymath/Ray.hpp"
#include "Intersection.hpp"
#include "Material.hpp"
#include "../raymath/Transform.hpp"
#include "../raymath/AABB.hpp"
enum CullingType
{
  CULLING_FRONT, // Only intersect where normal facing the ray
  CULLING_BACK,  // Only intersect where normal facing away from the ray
  CULLING_BOTH   // Always interest
};

class SceneObject
{
private:
public:
  std::string name = "";
  Material *material = NULL;
  Transform transform;
  AABB boundingBox;
  SceneObject();
  ~SceneObject();


  virtual void updateBoundingBox() = 0;
  virtual void applyTransform();
  virtual bool intersects(Ray &r, Intersection &intersection, CullingType culling);
};
