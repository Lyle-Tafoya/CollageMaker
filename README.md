CollageMaker creates collages out of images in a directory. The way it works is by picking a a random corner to place the first image, and then recursively tiles horizontally and vertically around the edges of the image untill most of the space is filled.

Command Line Arguments:
Argument      | Effect
-------------------------------------------------------------------------------
--color, -c   | Background color for the canvas. Defaults to "#000".

--corner, -r  | Corner to place the first image. Defaults to 0(top-left going clockwise)

--file, -f    | File name. Format is set based on extension. Defaults to "collage.png".

--height, -h  | Height of the canvas in pixels. Defaults to 1280.

--path, -p    | Specify path to a file containing a list of image path names

--minDraw, -d | Minimum number of pixels needed to tile images in. Defaults to 150.

--size, -s    | Set the size of the first image as a percentage relative to the canvas.

--type, -t    | Choose whether to tile "horizontal" or "vertical" first.

--width, -w   | Set the width of the final collage in pixels. Defaults to 1920.

If --path is not set, CollageMaker will look for a file "./images.txt". To generate this file, run the following command in the root of your image folder
````
ls -d -1 $PWD/**/*.[jpg,png,bmp]* > images.txt
````

Here is a downscaled sample output file
![ScreenShot](https://raw.githubusercontent.com/Lyle-Tafoya/CollageMaker/master/screenshot.jpg)
