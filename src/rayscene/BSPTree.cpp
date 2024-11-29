#include "BSPTree.hpp"
#include <algorithm>
#include <iostream>

#define MAX_OBJECTS 8  // Reduced from 10
#define MAX_DEPTH 12   // Increased from 10

BSPTree::BSPTree(std::vector<SceneObject*>& objs, int depth)
    : left(nullptr), right(nullptr), objects()
{
    // Separate infinite planes from regular objects
    std::vector<SceneObject*> finiteObjects;
    for (auto obj : objs) {
        if (obj->isPlane()) {
            objects.push_back(obj);  // Store planes at current node
        } else {
            finiteObjects.push_back(obj);
        }
    }

    // Calculate bounding box only for finite objects
    if (!finiteObjects.empty()) {
        boundingBox = finiteObjects[0]->boundingBox;
        for (size_t i = 1; i < finiteObjects.size(); ++i) {
            boundingBox.subsume(finiteObjects[i]->boundingBox);
        }

        // Only split if we have enough objects and haven't reached max depth
        if (finiteObjects.size() > MAX_OBJECTS && depth < MAX_DEPTH) {
            buildTree(finiteObjects, depth);
        } else {
            // Add finite objects to current node if we're not splitting
            objects.insert(objects.end(), finiteObjects.begin(), finiteObjects.end());
        }
    }
}

void BSPTree::buildTree(std::vector<SceneObject*>& finiteObjects, int depth)
{
    // Choose best splitting axis based on the longest axis
    Vector3 extent = boundingBox.getMax() - boundingBox.getMin();
    int axis = 0;
    if (extent.y > extent.x && extent.y > extent.z) axis = 1;
    if (extent.z > extent.x && extent.z > extent.y) axis = 2;

    double split = 0.0;
    if (axis == 0) {
        split = (boundingBox.getMin().x + boundingBox.getMax().x) * 0.5;
    } else if (axis == 1) {
        split = (boundingBox.getMin().y + boundingBox.getMax().y) * 0.5;
    } else {
        split = (boundingBox.getMin().z + boundingBox.getMax().z) * 0.5;
    }

    std::vector<SceneObject*> leftObjects, rightObjects;

    // Partition objects
    for (auto obj : finiteObjects) {
        double minVal, maxVal;
        if (axis == 0) {
            minVal = obj->boundingBox.getMin().x;
            maxVal = obj->boundingBox.getMax().x;
        } else if (axis == 1) {
            minVal = obj->boundingBox.getMin().y;
            maxVal = obj->boundingBox.getMax().y;
        } else {
            minVal = obj->boundingBox.getMin().z;
            maxVal = obj->boundingBox.getMax().z;
        }

        if (minVal <= split) leftObjects.push_back(obj);
        if (maxVal > split) rightObjects.push_back(obj);
    }

    // Create child nodes if partition is effective
    if (leftObjects.size() < finiteObjects.size() && !leftObjects.empty()) {
        left = new BSPTree(leftObjects, depth + 1);
    }
    if (rightObjects.size() < finiteObjects.size() && !rightObjects.empty()) {
        right = new BSPTree(rightObjects, depth + 1);
    }
}

void BSPTree::query(Ray& r, std::vector<SceneObject*>& result)
{
    
    // Always test planes at current node first
    result.insert(result.end(), objects.begin(), objects.end());

    // Early exit if no finite objects in this subtree
    if (left == nullptr && right == nullptr) {
        return;
    }

    // Check bounding box intersection for finite objects
    if (!boundingBox.intersects(r)) {
        return;
    }

    // Traverse children
    if (left) left->query(r, result);
    if (right) right->query(r, result);
}