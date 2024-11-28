#pragma once
#include <vector>
#include "SceneObject.hpp"
#include "../raymath/Transform.hpp"
#include "../raymath/Vector3.hpp"
#include "../raymath/Color.hpp"
#include "../raymath/Ray.hpp"
#include "./Triangle.hpp"
#include "../raymath/AABB.hpp"

class Mesh : public SceneObject
{
private:
  std::vector<Triangle *> triangles;

public:
  Mesh();
  ~Mesh();
  AABB boundingBox;

  void loadFromObj(std::string path);
  virtual void updateBoundingBox() override;
  virtual void applyTransform() override;
  virtual bool intersects(Ray &r, Intersection &intersection, CullingType culling) override;
};
