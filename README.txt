CollageMaker is a program to do exactly what it sounds like. It creats collages out of images. The way it works is by picking a a random corner to place the first image, and then recursively tiles horizontally and vertically around the edges of the image untill most of the space is filled.

Command Line Arguments:
--color, -c   Background color for the canvas. Defaults to "#000".
--corner, -r  Corner to place the first image. Defaults to 0(top-left going clockwise)
--file, -f    File name. Format is set based on extension. Defaults to "collage.png".
--height, -h  Height of the canvas in pixels. Defaults to 1280.
--minDraw, -d Minimum number of pixels needed to tile images in. Defaults to 150.
--size, -s    Set the size of the first image as a percentage relative to the canvas.
--type, -t    Choose whether to tile "horizontal" or "vertical" first.
--width, -w   Set the width of the final collage in pixels. Defaults to 1920.

In order to use this program you must have a folder "./Wallpapers/" containing files "wallpaper-1.jpg", "wallpaper-2.jpg", "wallpaper-3.jpg", ...
This is a cheap hack added for testing purposes. You MUST specify the number of images with the --numImages argument. This will be removed shortly. See TODO.txt for more information.
