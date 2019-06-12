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
#case(0) // ExptHut start
 #declare cameraLocation = <3670, 8202, 50>;
 #declare cameraLookAt   = <3533, 8411, 0>;
 #declare cameraAngle = 30;
  light_source {
    < 3714, 8534, 200 >  White
//    shadowless
  }
#break
#case(1) // SPECIES 1 start
 #declare cameraLocation = <310, 2127, 50>;
 #declare cameraLookAt   = <340,  1947, 0>;
 #declare cameraAngle = 70;
  light_source {
    < 200, 2127, 100 >  White
//    shadowless
  }
#break
#case(2) // SPECIES 2 view towards end of optics hutch
 #declare cameraLocation = <180, 2068, 50>;
 #declare cameraLookAt   = <465, 2000, 0>;
 #declare cameraAngle = 70;
  light_source {
    < 711, 2223, 100 >  White
//    shadowless
  }
  light_source {
    < 350, 2100, 100 >  White
//    shadowless
  }
#break
#case(3) // SPECIES end of optics hutch - view from top
 #declare cameraLocation = <817, 2176, 30>;
 #declare cameraLookAt   = <724, 2100, 3>;
 #declare cameraAngle = 100;
  
  light_source{ <763, 2277, 100>
    color rgb<1,1,1>
    area_light
    <0.03, 0, 0> <1.47, 0, 0>
    1,10 // numbers in directions
    adaptive 0  // 0,1,2,3...
    jitter // random softening
    translate<0, 1.50,  0>
  }
#break
#case(4) // SPECIES monochromator view
 #declare cameraLocation = <400, 2150, 50>;
 #declare cameraLookAt   = <515, 2022, 3>;
 #declare cameraAngle = 50;
  
  light_source{ <763, 2277, 100>
    color rgb<1,1,1>
    area_light
    <0.03, 0, 0> <1.47, 0, 0>
    1,10 // numbers in directions
    adaptive 0  // 0,1,2,3...
    jitter // random softening
    translate<0, 1.50,  0>
  }
#break
#case(5) // SPECIES end of optics hutch - view from top and sky sphere light source
 #declare cameraLocation = <1090, 3566, 50>;
 #declare cameraLookAt   = <724, 2100, 3>;
 #declare cameraAngle = 30;
sky_sphere{
  pigment{
    image_map{ hdr "rnl_probe.hdr"
               gamma 1.1
               map_type 1 interpolate 2}
      }// end pigment
      rotate <0,40,0> //
}
#break
  #case(6) // SPECIES ring / optics hutch view
    #declare cameraLocation   = <650, 1025, 150>;
    #declare cameraLookAt   =    <50, 1838, 3>;
    #declare cameraAngle = 100;
    light_source {
      < -380, 1509, 1000 >  White
      shadowless
    }
    light_source {
      < 500, 2000, 1000 >  White
      shadowless
    }
    background { color rgb <0.1, 0.1, 0.1> }
  #break
  #case(7) // SPECIES ring / optics hutch view white background
    #declare cameraLocation   = <650, 1025, 150>;
    #declare cameraLookAt   =    <50, 1838, 3>;
    #declare cameraAngle = 100;
    light_source {
      < 156, 1835, 100 >  White
      shadowless
    }
    background { color rgb <1, 1, 1> }
  #break
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


// // light source
// #switch ( lightSource )
// #case(0) // no light source here -> use the one near the beam dump defined after this switch section
// #break
// #case(1) // simple set of point lights
//  #for (lightY, -5000, 0, 1000)
//  light_source {
//       < 0, lightY, 190 > White
//       fade_distance 10.75
//       fade_power 2
//  }
//  #end
// #break
// #case(2)
//  #for (lightY, -5000, 0, 1000)
//  light_source {
//      < 0, lightY, 190 >
//      color rgb <1,1,1>
//      area_light
//      <0.03, 0, 0> <1.47,0.0>
//      1,10
//      adaptive 0
//      jitter // random softening
//      translate <0,1.5,0>
// //     fade_distance 10.75
// //     fade_power 2
//  }
//  #end
// #break
// #else
// #break
// #end

#include "b.inc"
