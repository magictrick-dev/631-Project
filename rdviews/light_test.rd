# This is a test/example rd file
# Two spheres, pure diffuse surface, one distant light behind eye.


Display "Simple Shading"  "Screen" "rgbsingle"
Format 640 480
CameraFOV 5
CameraEye 0 0 50

FrameBegin 1
WorldBegin

FarLight 0.9 0.1 -1  1.0  0.3  0.3  1.0
AmbientLight 0.3 0.3 0.3 1.0

Ka 0.2
Kd 0.8

XformPush
Translate -2.0 0.0 -2
Rotate "X" 50
Rotate "Y" 0
Sphere 1 0 1 360
XformPop

XformPush
Translate 1.2 0.5 -2
Rotate "X" 40
Rotate "Y" 30
Scale 0.75 0.75 0.75
Cube
XformPop


WorldEnd
FrameEnd
