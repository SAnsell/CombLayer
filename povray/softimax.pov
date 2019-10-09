#version 3.7;
#include "colors.inc"
#include "metals.inc"
#include "stones.inc"
#include "woods.inc"
#include "textures.inc"
#include "shapes3.inc"

global_settings {
  assumed_gamma 1.0
}

#declare view = 105;
#declare cameraAngle = 90;

#switch ( view )
  #case(0) // Optics line towards the ratchet wall
    #declare cameraLocation = <2500, 8550, 40>;
    #declare cameraLookAt   = <2320, 8400, 0>;
    #declare cameraAngle = 40;
  #break
  #case(1) // trigger unit front end
    #declare cameraLocation = <2280, 8450, 10>;
    #declare cameraLookAt   = <2280, 8400, 0>;
  #break
  #case(2) // pump unit M1
    #declare cameraLocation = <2320, 8450, 10>;
    #declare cameraLookAt   = <2320, 8400, 0>;
  #break
  #case(3) // Mirror system M1
    #declare cameraLocation = <2380, 8450, 10>;
    #declare cameraLookAt   = <2380, 8400, 0>;
  #break
  #case(5) // Tungsten collimator
    #declare cameraLocation = <2432, 8450, 10>;
    #declare cameraLookAt   = <2432, 8400, 0>;
  #break
  #case(6) // Monochromator
    #declare cameraLocation = <2514, 8550, 10>;
    #declare cameraLookAt   = <2514, 8400, 0>;
  #break
  #case(105) // slitTube
    #declare cameraLocation = <2420, 8450, 10>;
    #declare cameraLookAt   = <2500, 8400, 0>;
    #declare cameraAngle = 30;
  #break
  #else // whole optics line top view
    #declare cameraLocation = <2400, 8450, 160>;
    #declare cameraLookAt   = <2400, 8408, 0>;
#break
#end

light_source {
  <2300, 8460, 100> White
//  shadowless
}

camera {
  location cameraLocation
  look_at  cameraLookAt
  angle    cameraAngle
  sky z
  right -4/3*x
}

#include "b.inc"
