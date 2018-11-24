#include <cstdlib>
#include <ctime>
#include <cstring>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <Magick++.h>

#include "tiler.hpp"

// Default resolution of output image
size_t outputWidth = 1920, outputHeight = 1080;
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

int main(int argc, char *argv[])
{
  srand(time(NULL));

  corner = CollageMaker::randNum(0, 3);
  tileType = CollageMaker::randNum(0, 1);

  // Handle our command line arguments
  for(int i = 1; i < argc; i++)
  {
    if(strcmp(argv[i], "--width") == 0 || strcmp(argv[i], "-w") == 0)
      outputWidth = std::stoi(argv[++i]);
    else if(strcmp(argv[i], "--height") == 0 || strcmp(argv[i], "-h") == 0)
      outputHeight = std::stoi(argv[++i]);
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

  CollageMaker::Tiler tiler(outputWidth, outputHeight, database);

  size_t imageNum = CollageMaker::randNum(0, tiler.imageFilePaths.size()-1);
  Magick::Image mainImage(tiler.popImagePath(imageNum));

  // Downscale the image to the specified size but never upscale
  if((float)outputWidth/(float)mainImage.columns() < (float)outputHeight/(float)mainImage.rows())
  {
    size_t imageWidth = (float)size / 100.f * (float)outputWidth;
    if(mainImage.columns() > imageWidth)
      mainImage.resize(Magick::Geometry(std::to_string(imageWidth)));
  }
  else
  {
    size_t imageHeight = (float)size / 100.f * (float)outputHeight;
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
      mainX = outputWidth - mainImageWidth;
      mainY = 0;
      break;
    // Bottom Right
    case 2:
      mainX = outputWidth - mainImageWidth;
      mainY = outputHeight - mainImageHeight;
      break;
    // Bottom Left
    case 3:
      mainX = 0;
      mainY = outputHeight - mainImageHeight;
      break;
  }

  // Draw the first image
  CollageMaker::Tiler::DrawOperation::drawQueue.emplace_back(std::make_unique<CollageMaker::Tiler::DrawOperation>(std::move(mainImage), mainX, mainY));

  // Tile images horizontally first and then vertically
  if(tileType == 0)
  {
    tiler.tileImages(outputWidth, outputHeight-mainImageHeight,
              0, (mainY == 0 ? mainImageHeight : 0),
              minDraw, 0);
    tiler.tileImages(outputWidth-mainImageWidth, mainImageHeight,
              (mainX == 0 ? mainImageWidth : 0), (mainY == 0 ? 0 : outputHeight - mainImageHeight),
               minDraw, 1);
  }
  // Tile images vertically first and then horizontally
  else
  {
    tiler.tileImages(outputWidth-mainImageWidth, outputHeight,
              (mainX == 0 ? mainImageWidth : 0), 0,
              minDraw, 1);
    tiler.tileImages(mainImageWidth, outputHeight-mainImageHeight,
               mainX,  (mainY == 0 ? mainImageHeight : 0),
               minDraw, 0);
  }

  tiler.draw();
  tiler.canvas.write(filename);
  return 0;
}
