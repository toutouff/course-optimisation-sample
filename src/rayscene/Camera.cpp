#include <iostream>
#include <cmath>
#include "Camera.hpp"
#include "../raymath/Ray.hpp"
#include <thread>

struct RenderSegment
{
public:
  int rowMin;
  int rowMax;
  Image *image;
  double height;
  double intervalX;
  double intervalY;
  int reflections;
  Scene *scene;
};

Camera::Camera() : position(Vector3())
{
}

Camera::Camera(Vector3 pos) : position(pos)
{
}

Camera::~Camera()
{
}

Vector3 Camera::getPosition()
{
  return position;
}

void Camera::setPosition(Vector3 &pos)
{
  position = pos;
}

/**
 * Render a segment (set of rows) of the image
 */
void renderSegment(RenderSegment *segment)
{

  for (int y = segment->rowMin; y < segment->rowMax; ++y)
  {
    double yCoord = (segment->height / 2.0) - (y * segment->intervalY);

    for (int x = 0; x < segment->image->width; ++x)
    {
      double xCoord = -0.5 + (x * segment->intervalX);

      Vector3 coord(xCoord, yCoord, 0);
      Vector3 origin(0, 0, -1);
      Ray ray(origin, coord - origin);

      Color pixel = segment->scene->raycast(ray, ray, 0, segment->reflections);
      segment->image->setPixel(x, y, pixel);
    }
  }
}

void Camera::render(Image &image, Scene &scene)
{

#ifdef USE_MULTITHREADING
  unsigned int numThreads = std::thread::hardware_concurrency();
  std::vector<std::thread> threads;

  // Log the number of threads being used
  std::cout << "Using " << numThreads << " threads for rendering." << std::endl;
#else
  unsigned int numThreads = 1;
  std::cout << "Multi-threading is disabled. Using a single thread for rendering." << std::endl;
#endif

  double ratio = (double)image.width / (double)image.height;
  double height = 1.0 / ratio;

  double intervalX = 1.0 / (double)image.width;
  double intervalY = height / (double)image.height;

  scene.prepare();

  for (unsigned int i = 0; i < numThreads; ++i)
  {
    RenderSegment *seg = new RenderSegment();
    seg->height = height;
    seg->image = &image;
    seg->scene = &scene;
    seg->intervalX = intervalX;
    seg->intervalY = intervalY;
    seg->reflections = Reflections;
    seg->rowMin = i * (image.height / numThreads);
    seg->rowMax = (i + 1) * (image.height / numThreads);
#ifdef USE_MULTITHREADING
    threads.push_back(std::thread(renderSegment, seg));
#else
    renderSegment(seg);
#endif
  }

#ifdef USE_MULTITHREADING
  for (auto& thread : threads)
  {
    thread.join();
  }
#endif
}

std::ostream &operator<<(std::ostream &_stream, Camera &cam)
{
  Vector3 pos = cam.getPosition();
  return _stream << "Camera(" << pos << ")";
}
