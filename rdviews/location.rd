# This is a test/example rd file

Display "Circle Test"  "Screen" "rgbsingle"
Format 500 300 

# Testing circles

# The next three lines will be ignored by your renderer
CameraEye 250 150 -150
CameraAt  250 150 0
CameraUp  0.0 -1.0 0.0

FrameBegin 1

Background 0.3 0.3 0.3

WorldBegin

# x y z of center, then radius

Circle 0 0 0 10


Color 1.0 0.0 0.0
Circle 500 0 0 10

Color 0.0 1.0 0.0
Circle 0 300 0 10

WorldEnd
FrameEnd
