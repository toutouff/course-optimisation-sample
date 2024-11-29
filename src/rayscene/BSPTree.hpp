#pragma once
#include <vector>
#include "../raymath/Ray.hpp"
#include "../raymath/AABB.hpp"
#include "../raymath/Vector3.hpp"
#include "SceneObject.hpp"

class BSPTree 
{
private:
    static const int MAX_OBJECTS = 8;   // Maximum objects before splitting
    static const int MAX_DEPTH = 12;    // Maximum tree depth

    AABB boundingBox;                   // Bounding box for finite objects
    BSPTree* left;                      // Left child
    BSPTree* right;                     // Right child
    std::vector<SceneObject*> planes;   // Infinite planes at this node
    std::vector<SceneObject*> objects;  // Finite objects at this node

    void buildTree(std::vector<SceneObject*>& finiteObjects, int depth);
    int chooseSplitAxis() const;
    double calculateSplitValue(int axis) const;

public:
    BSPTree(std::vector<SceneObject*>& objects, int depth);
    virtual  ~BSPTree() = default;

    void query(Ray& r, std::vector<SceneObject*>& result);
    bool intersectsBoundingBox(const Ray& r) const;
    
    // Getters for testing/debugging
    const AABB& getBoundingBox() const { return boundingBox; }
    const std::vector<SceneObject*>& getPlanes() const { return planes; }
    const std::vector<SceneObject*>& getObjects() const { return objects; }
};