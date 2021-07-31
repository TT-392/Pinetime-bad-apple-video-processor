#!/bin/bash

#ffmpeg -i ../bad_apple.mp4 apple%05d.png

threshold=50
for file in *.png; do
    fileNr=${file#apple}
    fileNr=${fileNr%\.png}
    # area with faint stars
    if [ "$fileNr" = "00445" ]; then
        threshold=20
    elif [ $fileNr -gt 00755 -a $fileNr -lt 00771 ]; then
        threshold=$((threshold+2))
    fi
    echo $fileNr $threshold
    convert $file -resize 66.6666666666666666666666666667% -gravity center -threshold $threshold% -extent 240x240 $file.mono& # < super advanced multithreading
    sleep 0.001 # < make pc not crash when multithreading (tested with 16 threads on a ryzen 7 1700, might want to start off with a big bigger value for testing if you are running a slower processor)
done
