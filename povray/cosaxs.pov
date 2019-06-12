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

#declare view = 0;

#switch ( view )
#case(0) // Exp hutch towards the end
 #declare cameraLocation = <3408, 8580, 50>;
// #declare cameraLookAt   = <3273, 8407 0> ;
 #declare cameraLookAt   = <3943, 8407 0> ;
 #declare cameraAngle = 90;
  light_source {
    <3187, 8568, 100> White
//    shadowless
  }
#break
  #else
 #declare cameraLocation = <-250, -160, 0>;
 #declare cameraLookAt   = <0, -1000, 0>;
 #declare cameraAngle = 20;
#break
#end

camera {
     location  cameraLocation
     look_at cameraLookAt
     angle cameraAngle
     sky z
}

#include "b.inc"
