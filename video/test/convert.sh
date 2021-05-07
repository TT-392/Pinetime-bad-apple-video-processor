#!/bin/bash

#ffmpeg -i ../bad_apple.mp4 apple%05d.png

for file in *.png; do
    convert $file -resize 66.6666666666666666666666666667% -threshold 50% -gravity center -extent 240x240 $file.mono& # < super advanced multithreading
    sleep 0.001 # < make pc not crash when multithreading
done
