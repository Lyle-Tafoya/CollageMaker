#!/bin/sh
g++ `Magick++-config --cxxflags --cppflags` -std=c++11 -g -o CollageMaker main.cpp `Magick++-config --ldflags --libs`
