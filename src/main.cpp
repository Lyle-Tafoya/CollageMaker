#include <cstdlib>
#include <ctime>
#include <cstring>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <gflags/gflags.h>
#include <Magick++.h>

#include "tiler.hpp"

DEFINE_int32(outputWidth, 1920, "Width of the output image");
DEFINE_int32(outputHeight, 1080, "Height of the output image");
DEFINE_string(canvasColor, "black", "Color of the canvas background");
DEFINE_string(outputFilename, "collage.png", "Ouput filename for the collage image");
DEFINE_string(inputFilenames, "images.txt", "image filepath list");
DEFINE_int32(minDraw, 150, "Minimum amount of pixels to tile images in");
DEFINE_int32(corner, 0, "Specify which corner to place first image");
DEFINE_int32(size, 65, "Size of the first image as a percentage relative to the canvas");
DEFINE_string(tileType, "vertical", "Set which orthogonal direction to begin tiling");

int main(int argc, char *argv[])
{
  srand(time(NULL));

  gflags::SetUsageMessage("Generate image collages");
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  bool tileType = (FLAGS_tileType == "vertical" || FLAGS_tileType == "v") ? 1 : 0;

  CollageMaker::Tiler tiler(FLAGS_outputWidth, FLAGS_outputHeight, FLAGS_inputFilenames, FLAGS_canvasColor);

  size_t imageNum = CollageMaker::randNum(0, tiler.imageFilePaths.size()-1);
  Magick::Image mainImage(tiler.popImagePath(imageNum));
  // Downscale the image to the specified size but never upscale
  if(static_cast<float>(FLAGS_outputWidth)/mainImage.columns() < static_cast<float>(FLAGS_outputHeight)/mainImage.rows())
  {
    size_t imageWidth = static_cast<float>(FLAGS_size) / 100.f * FLAGS_outputWidth;
    if(mainImage.columns() > imageWidth)
    {
      mainImage.resize(Magick::Geometry(std::to_string(imageWidth)));
    }
  }
  else
  {
    size_t imageHeight = static_cast<float>(FLAGS_size) / 100.f * FLAGS_outputHeight;
    if(mainImage.rows() > imageHeight)
    {
      mainImage.resize(Magick::Geometry("x" + std::to_string(imageHeight)));
    }
  }
  const size_t mainImageWidth = mainImage.columns();
  const size_t mainImageHeight = mainImage.rows();

  size_t mainX = 0, mainY = 0;
  switch(FLAGS_corner)
  {
    // Top Left
    case 0:
      mainX = 0;
      mainY = 0;
      break;
    // Top Right
    case 1:
      mainX = FLAGS_outputWidth - mainImageWidth;
      mainY = 0;
      break;
    // Bottom Right
    case 2:
      mainX = FLAGS_outputWidth - mainImageWidth;
      mainY = FLAGS_outputHeight - mainImageHeight;
      break;
    // Bottom Left
    case 3:
      mainX = 0;
      mainY = FLAGS_outputHeight - mainImageHeight;
      break;
  }


  tiler.queueImage(mainX, mainY, std::move(mainImage));
  // Tile images horizontally first and then vertically
  if(tileType == 0)
  {
    tiler.tileImages(FLAGS_outputWidth, FLAGS_outputHeight-mainImageHeight, 0, (mainY == 0 ? mainImageHeight : 0), FLAGS_minDraw, 0);
    tiler.tileImages(FLAGS_outputWidth-mainImageWidth, mainImageHeight, (mainX == 0 ? mainImageWidth : 0), (mainY == 0 ? 0 : FLAGS_outputHeight - mainImageHeight), FLAGS_minDraw, 1);
  }
  // Tile images vertically first and then horizontally
  else
  {
    tiler.tileImages(FLAGS_outputWidth-mainImageWidth, FLAGS_outputHeight, (mainX == 0 ? mainImageWidth : 0), 0, FLAGS_minDraw, 1);
    tiler.tileImages(mainImageWidth, FLAGS_outputHeight-mainImageHeight, mainX,  (mainY == 0 ? mainImageHeight : 0), FLAGS_minDraw, 0);
  }
  tiler.draw();
  tiler.canvas.write(FLAGS_outputFilename);

  return 0;
}
