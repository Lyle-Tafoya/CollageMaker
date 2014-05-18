## CollageMaker

CollageMaker creates collages out of images by picking a random corner to place the first image, and then recursively tiling horizontally and vertically around the edges of the image untill most of the space is filled.

Here is a downscaled sample output file
![Screenshot](https://raw.githubusercontent.com/Lyle-Tafoya/CollageMaker/master/screenshot.jpg)

#### Command Line Arguments:
```
Argument      | Effect
--------------|----------------------------------------------------------------
--color,   -c | Background color for the canvas. Defaults to "#000".
--corner,  -r | Corner to place the first image. Defaults to 0(top-left going clockwise)
--file,    -f | File name. Format is set based on extension. Defaults to "collage.png".
--height,  -h | Height of the canvas in pixels. Defaults to 1280.
--path,    -p | Specify path to a file containing a list of image path names
--minDraw, -d | Minimum number of pixels needed to tile images in. Defaults to 150.
--size,    -s | Set the size of the first image as a percentage relative to the canvas.
--type,    -t | Choose whether to tile "horizontal" or "vertical" first.
--width,   -w | Set the width of the final collage in pixels. Defaults to 1920.
```

**If --path is not set, CollageMaker will look for a file "./images.txt".** This is simply a plain text file containing one absolute path to an image per line. To generate this file, run the following command in the root of your image folder _OSX/Linux_
````
ls -d -1 $PWD/**/*.[jpg,png,bmp]* > images.txt
````

If you are using Windows, you will have to find another way to generate this file.
