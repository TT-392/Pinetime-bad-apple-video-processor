#!/bin/bash

#ffmpeg -i ../bad_apple.mp4 apple%05d.png

for file in *.png; do
    convert $file -resize 66.6666666666666666666666666667% -gravity center -threshold 50% -extent 240x240 $file.mono& # < super advanced multithreading
    sleep 0.001 # < make pc not crash when multithreading (tested with 16 threads on a ryzen 7 1700, might want to start off with a big bigger value for testing if you are running a slower processor)
done
