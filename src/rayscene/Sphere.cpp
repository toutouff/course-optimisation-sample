#include <iostream>
#include <cmath>
#include "Sphere.hpp"
#include "../raymath/Vector3.hpp"

Sphere::Sphere(double r) : SceneObject(), radius(r)
{
}

Sphere::~Sphere()
{
}

void Sphere::applyTransform()
{
  Vector3 c;
  this->center = this->transform.apply(c);
  updateBoundingBox();
}


bool Sphere::intersects(Ray &r, Intersection &intersection, CullingType culling)
{
    if(!boundingBox.intersects(r))
    {
        return false;
    }

    // Vector from ray origin to center of sphere
    Vector3 OC = center - r.GetPosition();
    
    // Project OC onto the ray direction
    double tca = OC.dot(r.GetDirection());  // Projection length of OC onto the ray direction

    // If the projection is negative, the sphere is behind the ray, so no intersection
    if (tca < 0)
    {
        return false;
    }

    // Calculate the distance squared from the sphere's center to the perpendicular point on the ray
    double d2 = OC.lengthSquared() - tca * tca;  // Squared distance from sphere center to ray
    double radius2 = radius * radius;  // Squared radius

    // If the distance is greater than the radius, no intersection
    if (d2 > radius2)
    {
        return false;
    }

    // Compute the distance squared from the point of closest approach to the intersection points
    double thc2 = radius2 - d2;  // Squared dist²ance from closest approach to the intersection point

    // If thc2 is negative, there's no real intersection
    if (thc2 < 0)
    {
        return false;
    }

    // Now calculate the exact intersection points: t1 and t2 are the distances along the ray to the intersection points
    double thc = sqrt(thc2);  // Calculate the square root of thc2 (only now)
    double t = tca - thc;  // The closest intersection (first hit)
    if (t < 0) 
    {
        t = tca + thc;  // If the closest intersection is behind the ray, take the farthest one
        if (t < 0) 
        {
            return false;  // If both intersections are behind the ray, no intersection
        }
    }

    // Calculate the exact intersection point
    Vector3 P1 = r.GetPosition() + r.GetDirection() * t;

    // Pre-calculate some useful values for rendering
    intersection.Position = P1;
    intersection.Mat = this->material;
    intersection.Normal = (P1 - center).normalize();

    return true;
}


void Sphere::updateBoundingBox()
{
    boundingBox =  AABB(center - Vector3(radius, radius, radius),  center + Vector3(radius, radius, radius));
}
