#version 3.7;
#include  "colors.inc"
#include "metals.inc"

global_settings {
assumed_gamma 1.0
}

#declare cameraNumber = 1; // 0: DTL global view;  1: beam dump view
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
 #declare cameraAngle = 45;
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


// light source
#switch ( lightSource )
#case(0) // no light source here -> use the one near the beam dump defined after this switch section
#break
#case(1) // simple set of point lights
 #for (lightY, -5000, 0, 1000)
 light_source {
      < 0, lightY, 190 > White
      fade_distance 10.75
      fade_power 2
 }
 #end
#break
#case(2)
 #for (lightY, -5000, 0, 1000)
 light_source {
     < 0, lightY, 190 >
     color rgb <1,1,1>
     area_light
     <0.03, 0, 0> <1.47,0.0>
     1,10
     adaptive 0
     jitter // random softening
     translate <0,1.5,0>
//     fade_distance 10.75
//     fade_power 2
 }
 #end
#break
#else
#break
#end

#light_source {
#     < 260,    140, 190 >  White
#}

#include "b1.x"
