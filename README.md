# Pinetime bad apple video processor
This repo contains the code to process a bad apple .mp4 file into an optimized file to be flashed into the spiflash of the pinetime. If you don't care about processing the video, and just care about what is actually running on the pinetime, go to: https://github.com/TT-392/pinetime-bad-apple.

## Running on your own pinetime
For instructions on this step, go to: https://github.com/TT-392/pinetime-bad-apple.

## Compiling
(This guide assumes you are running linux, it might work on a different platform, but I can't guarantee it)\
While I was creating this program, I didn't really intend for anyone to use it with a different video, so if you really want to do this, I expect you to somewhat know what you are doing.

To compile a video file, you first gotta turn your file into a 30 fps image sequence (I used ffmpeg for this).

Next, you are gonna want to turn this image sequence into a sequence of 240x240 .mono images, I used imagemagick for this, the final script I created to do this can be found in: video/30fps/convert.sh.

(The following 4 steps are done by just running make, you might want to adjust the thread count though, see the first of the 4 steps)\
First, the optimizer needs to be run, this is a program which finds the most optimal blocks of pixels for drawing to the pinetime's display. This step will probably take a few hours depending on your processor. The default thread count is 16, there is a define in optimizer/main.c through which you can change the thread count.

After this, the compressor needs to be run. This turns the intermediate file format of optimizer/output/full, which is relatively human readable, into one that is more compact.

The resulting file, compressor/output_compressed, then needs to be compressed more, it is compressed using lz4, because this is an easy format to decompress on the pinetime. To do this step, you need to make sure you have lz4 installed, and run the command:\
```lz4 -12 -B10000 compressor/output_compressed compressor/bad_apple.lz4```

You now have the file that needs to be flashed to the spiflash of the watch. If there are any tools out there to do this, or you want to write your own, feel free to use that, if you want to use my method, you need to run the formatter. The formatter will break your file into a bunch of .h files, which will fit in the program memory of the pinetime. 
