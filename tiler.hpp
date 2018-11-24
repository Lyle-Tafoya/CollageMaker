#ifndef COLLAGEMAKER_TILER_HPP_
#define COLLAGEMAKER_TILER_HPP_

#include <fstream>
#include <memory>
#include <string>
#include <vector>
#include <Magick++.h>

namespace CollageMaker
{
  size_t randNum(size_t from, size_t to);

  class Tiler
  {
  public:
    // Information needed in order to queue a image draw operation
    struct DrawOperation
    {
    public:
      DrawOperation(Magick::Image &&image, size_t x, size_t y);
      static std::vector<std::unique_ptr<DrawOperation>> drawQueue;
      static void Draw(Magick::Image &canvas);
    private:
      Magick::Image image;
      size_t x;
      size_t y;

      friend class Tiler;
    };

    Magick::Image canvas;
    std::vector<std::string> imageFilePaths;

    Tiler(size_t width, size_t height, const std::string &imageFilepathDatabase, const std::string &canvasColor="black");
    void tileImages(size_t width, size_t height, size_t xOrigin, size_t yOrigin, size_t minDraw, bool tileType);
    void draw();
    std::string popImagePath(size_t imageNum);
  };
}

#endif
