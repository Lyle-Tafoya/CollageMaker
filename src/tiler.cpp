#include "tiler.hpp"

namespace CollageMaker
{
  size_t randNum(size_t from, size_t to) { return (rand() % (to-from+1)) + from; }

  std::vector<std::unique_ptr<Tiler::DrawOperation>> Tiler::DrawOperation::drawQueue;

  Tiler::DrawOperation::DrawOperation(Magick::Image &&image, size_t x, size_t y)
    : image(std::move(image)), x(x), y(y) {}

  void Tiler::DrawOperation::Draw(Magick::Image &canvas)
  {
    for(auto &&drawOperation : drawQueue)
    {
      canvas.composite(drawOperation->image, drawOperation->x, drawOperation->y);
    }
  }

  Tiler::Tiler(size_t width, size_t height, const std::string &imageFilePathDatabase, const std::string &canvasColor)
  {
    std::ifstream file(imageFilePathDatabase);
    std::string filePath;
    while(std::getline(file, filePath))
    {
      imageFilePaths.emplace_back(filePath);
    }
    file.close();

    Magick::Geometry geometry(std::to_string(width)+"x"+std::to_string(height));
    canvas = std::move(Magick::Image(geometry, canvasColor.c_str()));
  }

  std::string Tiler::popImagePath(size_t imageNum)
  {
    const std::string &path = imageFilePaths[imageNum];
    imageFilePaths.erase(imageFilePaths.begin() + imageNum);
    return path;
  }

  void Tiler::draw()
  {
    DrawOperation::Draw(canvas);
  }

  // Tile images in a width x height rectangle.
  // xOrigin and yOrigin represent the location of this rectangle relative to the canvas
  // minDraw represents the minimum number of pixels to scale an image down to
  // tileType determines whether it tiles horizontally or vertically first
  void Tiler::tileImages(size_t width, size_t height, size_t xOrigin, size_t yOrigin, size_t minDraw, bool tileType)
  {
    size_t x = xOrigin, y = yOrigin;
    std::vector<std::unique_ptr<DrawOperation>> drawQueue;
    size_t imageNum;

    // Tile Horizontally
    if(tileType == 0)
    {
      while(1)
      {
        imageNum = randNum(0, imageFilePaths.size()-1);
        Magick::Image image(popImagePath(imageNum));
        image.resize(Magick::Geometry("x" + std::to_string(height)));
        const size_t imageWidth = image.columns();
        // If the last image wont fit, tile images in the empty space
        if(((x-xOrigin) + imageWidth) > width)
        {
          tileImages(width-(x-xOrigin), height, x, y, minDraw, 1);
          break;
        }

        // Put this draw operation in the queue
        drawQueue.emplace_back(std::make_unique<DrawOperation>(std::move(image), x, y));

        x  += imageWidth;
        // If we run out of room to draw, center what we have then stop
        if(((x-xOrigin) + minDraw) > width)
        {
          for(auto &&drawOperation : drawQueue)
          {
            drawOperation->x += (width-(x-xOrigin))/2;
          }
          break;
        }
      }
    }
    // Tile Vertically
    else
    {
      while(1)
      {
        imageNum = randNum(0, imageFilePaths.size()-1);
        Magick::Image image(popImagePath(imageNum));
        image.resize(Magick::Geometry(std::to_string(width)));
        const size_t imageHeight = image.rows();
        // If the last image wont fit, tile images in the empty space
        if((y-yOrigin+imageHeight) > height)
        {
          tileImages(width, height-(y-yOrigin), x, y, minDraw, 0);
          break;
        }

        // Put this draw operation in the queue
        drawQueue.emplace_back(std::make_unique<DrawOperation>(std::move(image), x, y));

        y += imageHeight;
        // If we run out of room to draw, center what we have then stop
        if((y-yOrigin + minDraw) > height)
        {
          for(auto &&drawOperation : drawQueue)
          {
            drawOperation->y += (height-(y-yOrigin))/2;
          }
          break;
        }
      }
    }
    DrawOperation::drawQueue.insert(DrawOperation::drawQueue.end(),
        std::make_move_iterator(drawQueue.begin()),
        std::make_move_iterator(drawQueue.end()));
  }
}
