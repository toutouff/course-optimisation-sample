#include "BSPTree.hpp"
#include <algorithm>
#include <iostream>
#include <limits>

double BSPTree::computeNodeCost() const {
   Vector3 extent = boundingBox.getMax() - boundingBox.getMin();
   return 2.0 * (extent.x * extent.y + extent.y * extent.z + extent.z * extent.x);
}

BSPTree::SplitInfo BSPTree::findBestSplit(const std::vector<SceneObject*>& objects) {
   SplitInfo best = {0, 0.0, std::numeric_limits<double>::max()};
   const int NUM_BINS = 32;
   
   for (int axis = 0; axis < 3; axis++) {
       double start = (axis == 0) ? boundingBox.getMin().x : (axis == 1) ? boundingBox.getMin().y : boundingBox.getMin().z;
       double end = (axis == 0) ? boundingBox.getMax().x : (axis == 1) ? boundingBox.getMax().y : boundingBox.getMax().z;
       double step = (end - start) / NUM_BINS;
       
       for (int i = 1; i < NUM_BINS; i++) {
           double splitPos = start + i * step;
           int leftCount = 0, rightCount = 0, straddleCount = 0;
           AABB leftBox, rightBox;
           
           for (const auto& obj : objects) {
               double minVal = (axis == 0) ? obj->boundingBox.getMin().x : 
                             (axis == 1) ? obj->boundingBox.getMin().y : 
                                         obj->boundingBox.getMin().z;
               double maxVal = (axis == 0) ? obj->boundingBox.getMax().x :
                             (axis == 1) ? obj->boundingBox.getMax().y :
                                         obj->boundingBox.getMax().z;
               
               if (maxVal <= splitPos) {
                   leftCount++;
                   leftBox.subsume(obj->boundingBox);
               } else if (minVal >= splitPos) {
                   rightCount++;
                   rightBox.subsume(obj->boundingBox);
               } else {
                   straddleCount++;
                   leftBox.subsume(obj->boundingBox);
                   rightBox.subsume(obj->boundingBox);
               }
           }
           
           if (leftCount == 0 || rightCount == 0) continue;
           
           double surfaceArea = computeNodeCost();
           double pLeft = leftBox.getSurfaceArea() / surfaceArea;
           double pRight = rightBox.getSurfaceArea() / surfaceArea;
           double cost = 1.0 + (leftCount * pLeft + rightCount * pRight) + (straddleCount * (pLeft + pRight));
           
           if (cost < best.cost) {
               best = {axis, splitPos, cost};
           }
       }
   }
   return best;
}

BSPTree::BSPTree(std::vector<SceneObject*>& objs, int depth) 
   : left(nullptr), right(nullptr), objects(), planes() {
   
   std::vector<SceneObject*> finiteObjects;
   for (auto obj : objs) {
       if (obj->isPlane()) {
           planes.push_back(obj);
       } else {
           finiteObjects.push_back(obj);
       }
   }

   if (!finiteObjects.empty()) {
       boundingBox = finiteObjects[0]->boundingBox;
       for (size_t i = 1; i < finiteObjects.size(); ++i) {
           boundingBox.subsume(finiteObjects[i]->boundingBox);
       }

       if (finiteObjects.size() > MAX_OBJECTS && depth < MAX_DEPTH) {
           SplitInfo split = findBestSplit(finiteObjects);
           
           if (split.cost < std::numeric_limits<double>::max()) {
               std::vector<SceneObject*> leftObjects, rightObjects;
               
               for (auto obj : finiteObjects) {
                   double minVal = (split.axis == 0) ? obj->boundingBox.getMin().x :
                                 (split.axis == 1) ? obj->boundingBox.getMin().y :
                                                   obj->boundingBox.getMin().z;
                   double maxVal = (split.axis == 0) ? obj->boundingBox.getMax().x :
                                 (split.axis == 1) ? obj->boundingBox.getMax().y :
                                                   obj->boundingBox.getMax().z;
                   
                   if (maxVal <= split.splitPos) {
                       leftObjects.push_back(obj);
                   } else if (minVal >= split.splitPos) {
                       rightObjects.push_back(obj);
                   } else {
                       objects.push_back(obj);
                   }
               }

               if (!leftObjects.empty()) {
                   left = new BSPTree(leftObjects, depth + 1);
               }
               if (!rightObjects.empty()) {
                   right = new BSPTree(rightObjects, depth + 1);
               }
           } else {
               objects = finiteObjects;
           }
       } else {
           objects = finiteObjects;
       }
   }
}

void BSPTree::query(Ray& r, std::vector<SceneObject*>& result) {
   if (!boundingBox.intersects(r)) return;
   
   result.insert(result.end(), planes.begin(), planes.end());
   result.insert(result.end(), objects.begin(), objects.end());
   
   if (!left || !right) return;

   Vector3 dir = r.GetDirection();
   BSPTree *near = left, *far = right;
   if (dir.x < 0 || dir.y < 0 || dir.z < 0) {
       std::swap(near, far);
   }

   if (near) near->query(r, result);
   if (far) far->query(r, result);
}

BSPTree::~BSPTree() {
   delete left;
   delete right;
}