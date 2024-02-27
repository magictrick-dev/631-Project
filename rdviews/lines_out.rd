# This tests the line algorithm.
Display "LineBoundaries" "PNM" "rgb"
Format 640 480

# The next three lines will be ignored by your renderer
CameraEye 320 240 -240
CameraAt  320 240 0
CameraUp  0.0 -1.0 0.0

FrameBegin 1
WorldBegin

# These lines are opposing horizontals.
Line 330 240 0      380 240 0
Line 310 240 0      260 240 0

# These lines are opposing l/r horizontals.
Line 330 240 0      370 220 0
Line 330 240 0      370 260 0
Line 310 240 0      270 220 0
Line 310 240 0      270 260 0

# These lines are over the 45.
Line 330 240 0      350 200 0
Line 330 240 0      350 280 0
Line 310 240 0      290 200 0
Line 310 240 0      290 280 0

# This line goes out of bounds.
Line 500 240 0      700 240 0
Line 100 200 0     -100 200 0
Line 300  40 0      300 -30 0
Line 420 440 0      420 500 0


WorldEnd
FrameEnd
