# This is a test/example rd file
# Two spheres, pure diffuse surface, one distant light behind eye.


Display "Light Test"  "Screen" "rgbsingle"
Format 640 480
CameraFOV 40
CameraEye 0 2 10
Surface "plastic"

ObjectBegin "CoordsLine"
XformPush
Color 1 1 1 

Scale .01 .01 .01
Cylinder 1.0 0.0 180.0 360
XformPop
ObjectEnd

# The Grid
ObjectBegin "CoordsGrid"
XformPush
Rotate "Z" 90
ObjectInstance "CoordsLine"
XformPop

XformPush
Rotate "X" 90
ObjectInstance "CoordsLine"
XformPop

XformPush
Rotate "Y" 90
ObjectInstance "CoordsLine"
XformPop

ObjectEnd

FrameBegin 1
WorldBegin

FarLight -1 0 -1  0.0  0.9  0.9  1.0
PointLight -2 0 0  0.0  0.3  0.3  3.0
PointLight 1.1 0.8 1    1 1 1 5.0
PointLight 1.1 -5.3 1    1 1 1 5.0

AmbientLight 1 1 1 1.0
Specular 1.0 1.0 1.0 30.0

Ka 0.3
Kd 0.6
Ks 1.0

XformPush
Translate -1.5 0.0 -2
Rotate "X" 30
Rotate "Y" 20
Sphere 1 0 1 360
XformPop

XformPush
Translate -4.5 -2.0 -1
Rotate "X" 40
Rotate "Y" 90
Scale 2.0 2.0 2.0
Sphere 1 0 1 360
XformPop

XformPush
Translate 1.2 0.5 -2
Rotate "X" 40
Rotate "Y" 30
Scale 0.75 0.75 0.75
Cube

Translate -1.8 -1 3
Rotate "X" 240
Cone 1.3 1 360

Rotate "X" 180
Cone 1.3 1 360
XformPop


WorldEnd
FrameEnd
