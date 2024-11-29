#include "BSPTree.hpp"

#define MAX_OBJECTS 10
#define MAX_DEPTH 10

BSPTree::BSPTree(std::vector<SceneObject*>& objs, int depth)
    : left(nullptr), right(nullptr), objects(objs)
{
    // Calcul du AABB englobant tous les objets
    if (!objects.empty())
    {
        boundingBox = objects[0]->boundingBox;
        for (size_t i = 1; i < objects.size(); ++i)
        {
            boundingBox.subsume(objects[i]->boundingBox);
        }
    }

    if (objects.size() > MAX_OBJECTS && depth < MAX_DEPTH)
    {
        buildTree(depth);
    }
}

BSPTree::~BSPTree()
{
    delete left;
    delete right;
}

void BSPTree::buildTree(int depth)
{
    // Séparation selon l'axe le plus long
    Vector3 size = boundingBox.getMax() - boundingBox.getMin();
    int axis = 0;
    if (size.y > size.x && size.y > size.z)
        axis = 1;
    else if (size.z > size.x && size.z > size.y)
        axis = 2;

    double split = 0.0;
    if (axis == 0)
        split = (boundingBox.getMin().x + boundingBox.getMax().x) * 0.5;
    else if (axis == 1)
        split = (boundingBox.getMin().y + boundingBox.getMax().y) * 0.5;
    else
        split = (boundingBox.getMin().z + boundingBox.getMax().z) * 0.5;

    std::vector<SceneObject*> leftObjects;
    std::vector<SceneObject*> rightObjects;

    for (auto obj : objects)
    {
        if (axis == 0) {
            if (obj->boundingBox.getMin().x < split)
                leftObjects.push_back(obj);
            if (obj->boundingBox.getMax().x > split)
                rightObjects.push_back(obj);
        } else if (axis == 1) {
            if (obj->boundingBox.getMin().y < split)
                leftObjects.push_back(obj);
            if (obj->boundingBox.getMax().y > split)
                rightObjects.push_back(obj);
        } else {
            if (obj->boundingBox.getMin().z < split)
                leftObjects.push_back(obj);
            if (obj->boundingBox.getMax().z > split)
                rightObjects.push_back(obj);
        }
    }

    if (!leftObjects.empty())
        left = new BSPTree(leftObjects, depth + 1);
    if (!rightObjects.empty())
        right = new BSPTree(rightObjects, depth + 1);

    objects.clear();
}

void BSPTree::query(Ray& r, std::vector<SceneObject*>& result)
{
    if (!boundingBox.intersects(r))
        return;

    if (left == nullptr && right == nullptr)
    {
        // Nœud feuille, ajouter les objets
        result.insert(result.end(), objects.begin(), objects.end());
    }
    else
    {
        if (left)
            left->query(r, result);
        if (right)
            right->query(r, result);
    }
}