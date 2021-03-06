# Camera on BBB

### Table of Contents

  * [Software required](#software-required)
  * [Camera configuration](#camera-configuration)
  * [Troubleshoot](#troubleshoot)
  * [Reference](#reference)
  
### Software required

  * v4l-utils
    * `apt-get install v4l-utils`
  * avconv
    * `sudo apt-get install libav-tools`
  * BoneCV
    * `git clone https://github.com/derekmolloy/boneCV`
    * Build the project: `cd boneCV && ./build`

### Camera configuration

*Make sure your USB camera is plugged to the beaglebone before proceeding*

  * To check what are the format supported by the camera:  
    `v4l2-ctl --list-formats`
    
  * To check which format is activated:  
    `v4l2-ctl --all`
    
  * To change video format to index 1 (just an example):  
    `v4l2-ctl --set-fmt-video=pixelformat=1`  
    
  * To change the resolution captured  
    `v4l2-ctl --set-fmt-video=width=740,height=420`

### Streaming

#### Streaming from BBB to my PC on port 1234 over UDP

##### BBB
*In this example I am running MJPEG video format. Other format require different parameters. For H265 check the tutorial website in the reference.*  

Capture using the camera found at `/dev/video0` and output the raw data on the STDOUT, then pipe the output to `avconv` which will take care of streaming it to your PC using UDP  
`./capture -o -c0 | avconv -f mjpeg -i pipe:0 -f mpegts udp://192.168.1.187:1234`

* `capture` The capsture library modified to easily handle H265 video format
* `-o` Output to the STDOUT
* `-c0` Infinite frames
* `avconv` Used to stream in this case. Can also store to files.
* `-f mjpeg` format MJPEG
* `-i pipe:0` Read input from the STDOUT of the piping program
* `-f mpegts` ???
* `udp://192.168.1.187:1234` Example of IP and port

Speeding up the streaming:  
https://trac.ffmpeg.org/wiki/How%20to%20speed%20up%20/%20slow%20down%20a%20video

##### Take a picture every 10 seconds
```
avconv -f video4linux2 -i /dev/v4l/by-id/usb-046d_HD_Pro_Webcam_C920_2A4F7FDF-video-index0 -vf fps=1/10 test%3d.jpeg
```
- To change the delay to every 15 seconds, update fps=1/15
- It doesn't work if we use /dev/video0 instead of /dev/v4l/by-id/usb-046d_HD_Pro_Webcam_C920_2A4F7FDF-video-index0

##### PC

Listen for UDP data on port 1234   
`vlc udp://@:1234`

Now running `netstat` should show an entry for port 1234 udp  
`netstat -na | grep 1234`

### Troubleshoot

  * If you get **select timeout** when running `./capture`, run the following in your BBB terminal:
```
# Reference: https://www.raspberrypi.org/forums/viewtopic.php?t=35689&p=300710
rmmod uvcvideo
modprobe uvcvideo nodrop=1 timeout=5000 quirks=0x80
```

  * If you get **pipe:: Invalid data found when processing input** when running `avconv`, then make sure you have the avconv flags set in the correct order first, then try different arguments. Docs: https://libav.org/avconv.html

  * If you are not sure you are receiving data on your PC, run Wireshark or `sudo tcpdump udp port 1234`

### Reference

  Derek Molloy:

  * Website: http://derekmolloy.ie/beaglebone/beaglebone-black-streaming-video-tutorial/#Introduction
  * Youtube: https://www.youtube.com/watch?v=8QouvYMfmQo and https://www.youtube.com/watch?v=-6DBR8PSejw
