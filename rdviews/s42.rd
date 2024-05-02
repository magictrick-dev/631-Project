# This is a test/example rd file
# Two diffuse spheres, 4 light sources
# Same geometry, but left sphere has shading interpolation,
# Right sphere has no interpolation

Display "Interpolation"  "Screen" "rgbsingle"
Format 640 480
CameraFOV 5
CameraEye 0 0 50

FrameBegin 1
WorldBegin

FarLight 0 0 -1  1.0  1.0  1.0  1.0

FarLight  1  1 -0.5  1.0 0.0 0.0  0.5
FarLight  0 -1 -0.5  0.0 1.0 0.0  0.5
FarLight -1  1 -0.5  0.0 0.0 1.0  0.5

Ka 0.0
Kd 1.0

Translate -1.25 0 0

Sphere 1.0 -1.0 1.0 360.0

Translate 2.5 0 0 

OptionBool "Interpolate" off

Sphere 1.0 -1.0 1.0 360.0


WorldEnd
FrameEnd
