#include <cstdlib>
#include <ctime>
#include <cstring>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <Magick++.h>

// Generate a random number between from and to
size_t RandNum(size_t from, size_t to) { return (rand() % (to-from+1)) + from; }

// Information needed in order to queue a image draw operation
struct DrawOperation
{
  Magick::Image image;
  size_t x;
  size_t y;
  DrawOperation(Magick::Image &&image, size_t startX, size_t startY)
    : image(std::move(image)), x(startX), y(startY) {};

  static std::vector<std::unique_ptr<DrawOperation>> drawQueue;
  static void Draw(Magick::Image &canvas)
  {
    for(auto &&drawOperation : drawQueue)
    {
      canvas.composite(drawOperation->image, drawOperation->x, drawOperation->y);
    }
  }
};
std::vector<std::unique_ptr<DrawOperation>> DrawOperation::drawQueue;

// Read a text file into memory
struct textfile
{
private:
  std::vector<std::string> lines;

public:
  textfile(std::string fileName)
  {
    std::ifstream pFile(fileName);
    std::string line;
    while(getline(pFile, line))
      lines.push_back(line);
    pFile.close();
  }

  std::string GetLine(size_t lineNum) const { return lines[lineNum]; }
  void DelLine(size_t lineNum) { lines.erase(lines.begin()+lineNum); }
  size_t NumLines() const { return lines.size(); }
};

//
// Global Variables
//

// Default resolution of output image
size_t screenWidth = 1920, screenHeight = 1080;
// Default background color
std::string canvasColor = "black";
// Default size of main image as a percentage
unsigned char size = 65;
// Corner to place the first image
unsigned char corner;
// Do not attempt to draw in areas less than minDraw number of pixels
size_t minDraw = 150;
bool tileType;
// File extension
std::string filename = "collage.png";
// Path to image database
std::string database = "images.txt";

std::unique_ptr<textfile> images;

// Tile images in a width x height rectangle.
// xOrigin and yOrigin represent the location of this rectangle relative to the canvas
// minDraw represents the minimum number of pixels to scale an image down to
// tileType determines whether it tiles horizontally or vertically first
void TileImages(size_t width, size_t height, size_t xOrigin, size_t yOrigin, size_t minDraw, bool tileType)
{
  size_t x = xOrigin, y = yOrigin;
  std::vector<std::unique_ptr<DrawOperation>> drawQueue;
  size_t imageNum;

  // Tile Horizontally
  if(tileType == 0)
  {
    while(1)
    {
      imageNum = RandNum(0, images->NumLines()-1);
      Magick::Image image(images->GetLine(imageNum));
      images->DelLine(imageNum);
      image.resize(Magick::Geometry("x" + std::to_string(height)));
      const size_t imageWidth = image.columns();
      // If the last image wont fit, tile images in the empty space
      if(((x-xOrigin) + imageWidth) > width)
      {
        TileImages(width-(x-xOrigin), height, x, y, minDraw, 1);
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
      imageNum = RandNum(0, images->NumLines()-1);
      Magick::Image image(images->GetLine(imageNum));
      images->DelLine(imageNum);
      image.resize(Magick::Geometry(std::to_string(width)));
      const size_t imageHeight = image.rows();
      // If the last image wont fit, tile images in the empty space
      if((y-yOrigin+imageHeight) > height)
      {
        TileImages(width, height-(y-yOrigin), x, y, minDraw, 0);
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

int main(int argc, char *argv[])
{
  srand(time(NULL));

  corner = RandNum(0, 3);
  tileType = RandNum(0, 1);

  // Handle our command line arguments
  for(int i = 1; i < argc; i++)
  {
    if(strcmp(argv[i], "--width") == 0 || strcmp(argv[i], "-w") == 0)
      screenWidth = std::stoi(argv[++i]);
    else if(strcmp(argv[i], "--height") == 0 || strcmp(argv[i], "-h") == 0)
      screenHeight = std::stoi(argv[++i]);
    else if(strcmp(argv[i], "--color") == 0 || strcmp(argv[i], "-c") == 0)
      canvasColor = argv[++i];
    else if(strcmp(argv[i], "--minDraw") == 0 || strcmp(argv[i], "-d") == 0)
      minDraw = std::stoi(argv[++i]);
    else if(strcmp(argv[i], "--file") == 0 || strcmp(argv[i], "-f") == 0)
      filename = argv[++i];
    else if(strcmp(argv[i], "--size") == 0 || strcmp(argv[i], "-s") == 0)
      size = std::stoi(argv[++i]);
    else if(strcmp(argv[i], "--type") == 0 || strcmp(argv[i], "-t") == 0)
    {
      std::string tiletype = argv[++i];
      if(tiletype == "vertical" || tiletype == "v")
        tileType = 1;
      else // Horizontal
        tileType = 0;
    }
    else if(strcmp(argv[i], "--corner") == 0 || strcmp(argv[i], "-r") == 0)
      corner = std::stoi(argv[++i]);
    else if(strcmp(argv[i], "--path") == 0 || strcmp(argv[i], "-p") == 0)
      database = argv[++i];
  }

  images = std::make_unique<textfile>(database);

  // Set up our blank canvas
  Magick::Geometry geometry(std::to_string(screenWidth)+"x"+std::to_string(screenHeight));
  Magick::Image canvas(geometry, canvasColor.c_str());

  size_t imageNum = RandNum(0, images->NumLines()-1);
  Magick::Image mainImage(images->GetLine(imageNum));
  images->DelLine(imageNum);

  // Downscale the image to the specified size but never upscale
  if((float)screenWidth/(float)mainImage.columns() < (float)screenHeight/(float)mainImage.rows())
  {
    size_t imageWidth = (float)size / 100.f * (float)screenWidth;
    if(mainImage.columns() > imageWidth)
      mainImage.resize(Magick::Geometry(std::to_string(imageWidth)));
  }
  else
  {
    size_t imageHeight = (float)size / 100.f * (float)screenHeight;
    if(mainImage.rows() > imageHeight)
      mainImage.resize(Magick::Geometry("x" + std::to_string(imageHeight)));
  }
  const size_t mainImageWidth = mainImage.columns();
  const size_t mainImageHeight = mainImage.rows();

  // Randomly pick a corner to draw the first image
  // TODO mainX and mainY should replace corner as CL argument
  size_t mainX = 0, mainY = 0;
  switch(corner)
  {
    // Top Left
    case 0:
      mainX = 0;
      mainY = 0;
      break;
    // Top Right
    case 1:
      mainX = screenWidth - mainImageWidth;
      mainY = 0;
      break;
    // Bottom Right
    case 2:
      mainX = screenWidth - mainImageWidth;
      mainY = screenHeight - mainImageHeight;
      break;
    // Bottom Left
    case 3:
      mainX = 0;
      mainY = screenHeight - mainImageHeight;
      break;
  }

  // Draw the first image
  DrawOperation::drawQueue.emplace_back(std::make_unique<DrawOperation>(std::move(mainImage), mainX, mainY));

  // Tile images horizontally first and then vertically
  if(tileType == 0)
  {
    TileImages(screenWidth, screenHeight-mainImageHeight,
              0, (mainY == 0 ? mainImageHeight : 0),
              minDraw, 0);
    TileImages(screenWidth-mainImageWidth, mainImageHeight,
              (mainX == 0 ? mainImageWidth : 0), (mainY == 0 ? 0 : screenHeight - mainImageHeight),
               minDraw, 1);
  }
  // Tile images vertically first and then horizontally
  else
  {
    TileImages(screenWidth-mainImageWidth, screenHeight, // width, height
              (mainX == 0 ? mainImageWidth : 0), 0,      // x,y
              minDraw, 1);
    TileImages(mainImageWidth, screenHeight-mainImageHeight,
               mainX,  (mainY == 0 ? mainImageHeight : 0),
               minDraw, 0);
  }

  DrawOperation::Draw(canvas);
  canvas.write(filename);
  return 0;
}
