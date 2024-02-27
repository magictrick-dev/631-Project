
Display "CircleTests" "Screen" "rgbsingle"
Format 640 480

# The next three lines will be ignored by your renderer
CameraEye 320 240 -240
CameraAt  320 240 0
CameraUp  0.0 -1.0 0.0

FrameBegin 1
WorldBegin

#odds
Color 0.1 0.4 0.4
Circle 40 200 0 1 

Color 0.51 0.54 0.54
Circle 40 200 0 3 


Color 0.3 0.45 0.3
Circle 40 200 0 5 


Color 0.1 0.1 0.9
Circle 40 200 0 7 

Color 0.9 0.6 0.3
Circle 40 200 0 9

Color 0.71 0.0 0.81
Circle 40 200 0 11

#evens

Color 0.88 0.40 0.10
Circle 540 200 0 2 


Color 0.16 0.0 0.90
Circle 540 200 0 4 


Color 0.15 0.2 0.0
Circle 540 200 0 6 

Color 0.12 0.72 0.2
Circle 540 200 0 8 

Color 0.3 0.7 0.99
Circle 540 200 0 10

Color 0.9 0.9 0.4
Circle 540 200 0 12


WorldEnd
FrameEnd
