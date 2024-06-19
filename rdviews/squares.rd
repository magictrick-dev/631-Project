# This tests the line algorithm.
Display "Squares" "Screen" "rgbsingle"
Format 640 480

# The next three lines will be ignored by your renderer
CameraEye 320 240 -240
CameraAt  320 240 0
CameraUp  0.0 -1.0 0.0

FrameBegin 1
WorldBegin

Color 1.0 0.0 0.0
Line 200 200 0 300 200 0
Line 200 200 0 200 300 0
Line 300 200 0 300 300 0
Line 200 300 0 300 300 0

Color 1.0 1.0 0.0
Line 250 150 0 250 350 0
Line 150 250 0 350 250 0

Color 0.0 0.0 0.0
Point 250 250 0

Color 0.0 1.0 1.0
Fill 220 220 0

Color 1.0 0.0 1.0
Fill 280 220 0

Color 0.0 0.0 1.0
Fill 220 280 0

Color 1.0 1.0 0.0
Fill 280 280 0

# Circles
Color 1.0 1.0 1.0
Circle 440 250 0 20
Circle 440 250 0 30
Circle 440 250 0 40
Circle 440 250 0 50
Circle 440 250 0 60
Circle 440 250 0 70
Circle 440 250 0 80
Circle 440 250 0 90

# Fill Colors
Color 0.4 0.2 0.0
Fill 440 250 0

Color 0.19 0.19 0.19
Fill 465 250 0

Color 0.77 0.77 0.77
Fill 475 250 0

Color 0.95 0.20 0.70
Fill 485 250 0

Color 0.9 0.45 0.0
Fill 495 250 0

Color 0.98 0.98 0.05
Fill 505 250 0

Color 0.01 0.11 0.88
Fill 515 250 0

Color 0.56 0.0 1.0
Fill 525 250 0

WorldEnd
FrameEnd
