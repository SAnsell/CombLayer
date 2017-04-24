#version 3.7;
#include  "colors.inc"
#include "metals.inc"

global_settings {
assumed_gamma 1.0
}

#declare cameraNumber = 1;
#declare lightSource  = 0; // 0: only the beam dump bulb; 1: simple array of point lights; 2: array of area_light

#switch ( cameraNumber )
#case(0) // DTL global view
 #declare cameraLocation = <250, -160, 0>;
 #declare cameraLookAt   = <0, -1000, 0>;
 #declare cameraAngle = 90;
#break
#case(1) // beam dump view
 #declare cameraLocation = <250, -160, 0>;
 #declare cameraLookAt   = <0, 0, 0>;
 #declare cameraAngle = 90;
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

light_source {
     < 260,    140, 190 >  White
}

#declare mat0 = texture {
  pigment{color White}
};

#declare mat38 = texture {
  pigment{color Green}
};

#include "b1.x"
