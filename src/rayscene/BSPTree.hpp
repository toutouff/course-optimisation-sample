#include <vector>
#include "../raymath/AABB.hpp"
#include "SceneObject.hpp"
#include "../raymath/Ray.hpp"

class BSPTree {
private:
    struct SplitInfo {
        int axis;
        double splitPos;
        double cost;
    };
    
    AABB boundingBox;
    BSPTree* left;
    BSPTree* right;
    std::vector<SceneObject*> planes;   
    std::vector<SceneObject*> objects;  
    static const int MAX_OBJECTS = 8;
    static const int MAX_DEPTH = 12;

    double computeNodeCost() const;
    SplitInfo findBestSplit(const std::vector<SceneObject*>& objects);

public:
    BSPTree(std::vector<SceneObject*>& objs, int depth);
    ~BSPTree();
    void query(Ray& r, std::vector<SceneObject*>& result);
};