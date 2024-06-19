# This is a test/example rd file

Display "ObjectBeginEndTest"  "Screen" "rgbsingle"

Format 640 480
CameraFOV  60.0

ObjectBegin "CubeLayer"

XformPush
Scale 2.0 2.0 2.0
Cube
XformPop

XformPush
Scale 1.5 1.5 1.5
Cube
XformPop

XformPush
Scale 1.0 1.0 1.0
Cube
XformPop

XformPush
Scale 0.5 0.5 0.5
Cube
XformPop

ObjectEnd

FrameBegin 1

CameraEye  -12.0  -6.0  5.0
CameraAt    1.0   1.0  1.0
CameraUp    0.0   0.0  1.0

Clipping    1.0   1.0e+09


WorldBegin

XformPush

Translate 1.0 1.0 1.0

Scale 2.0 2.0 2.0

Color 1.0 0.5 0.7

ObjectInstance "CubeLayer"
XformPop

XformPush

Color 1.0 1.0 1.0

Translate 1.0 1.0 1.0

Sphere 1.0 -1.0 1.0 360.0

XformPop

WorldEnd
FrameEnd
