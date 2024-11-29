
#pragma once
#include <vector>
#include "../raymath/Ray.hpp"
#include "../raymath/AABB.hpp"
#include "SceneObject.hpp"

class BSPTree
{
public:
    AABB boundingBox;
    BSPTree* left;
    BSPTree* right;
    std::vector<SceneObject*> objects;

    BSPTree(std::vector<SceneObject*>& objects, int depth);
    ~BSPTree();

    void buildTree(int depth);
    void query(Ray& r, std::vector<SceneObject*>& result);
};