#version 3.7;
#include "colors.inc"
#include "metals.inc"
#include "stones.inc"
#include "woods.inc"
#include "textures.inc"
#include "shapes3.inc"

#declare view = 101;
// PROJECTION:
// 0=perspective
// 1=panoramic - good to display whole beam line
// 2=omnimax
// 3=cylindrical
#declare projection = 0;
#declare cameraAngle = 90;

#declare quick=1; // 0=quick but low quality, 1=slow but somewhat better quality
// another possibility to affect speed is command argument -q0 ... -q11
// so one can set quick=0 and play with -q0


#if (quick=1)
  #declare RadOK=0;
  #declare AreaOK=0;
  #declare PhotonsOK=0;
#else
  #declare RadOK=2; // 0=no radiosity ; 1= low quality rad; 2= good quality
  #declare AreaOK=1; // area light
  #declare PhotonsOK=1; // photons
#end

global_settings {
  assumed_gamma 1.0
  noise_generator 1
  #if (RadOK>0)
    radiosity{
      #switch (RadOK)
	#case (1)
	  count 35 error_bound 1.8
	#break
	#case (2)
	  count 100 error_bound 0.2
	#break
      #end
      nearest_count 5
      recursion_limit 1
      low_error_factor 0.2
      gray_threshold 0
      minimum_reuse 0.015
      brightness 1
      adc_bailout 0.01/2
      normal on
      media off
    }
  #end
  #if (PhotonsOK=1)
    photons {
      count 4000000                 // specify the density of photons
      autostop 0
      jitter .4
    }
  #end
}

#switch ( view )
  #case(0) // Optics line towards the ratchet wall
    #declare cameraLocation = <3000, 8550, 40>;
    #declare cameraLookAt   = <2320, 8400, 0>;
    #declare cameraAngle = 40;
  #break
  #case(1) // trigger unit front end
    #declare cameraLocation = <2280, 8450, 10>;
    #declare cameraLookAt   = <2280, 8400, 0>;
  #break
  #case(2) // pump unit M1
    #declare cameraLocation = <2300, 8450, 10>;
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
    #declare cameraLocation = <2560, 8550, 50>;
    #declare cameraLookAt   = <2560, 8400, 0>;
  #break
  #case(7) // PumpTubeM3
    #declare cameraLocation = <2770, 8450, 10>;
    #declare cameraLookAt   = <2790, 8400, 0>;
  #break
  #case(8) // M3 CTXM Mirror
    #declare cameraLocation = <3070, 8450, 50>;
    #declare cameraLookAt   = <3070, 8400, 0>;
  #break
  #case(9) // Tungsten collimators towards back wall
    #declare cameraLocation = <3170, 8400, 20>;
    #declare cameraLookAt   = <3270, 8400, 0>;
  #break
  #case(100) // Front beam Undulator
    #declare cameraLocation = <-350, 8450, 10>;
    #declare cameraLookAt   = <-332, 8409, 3.2>;
  #case(101) // FM1
    #declare cameraLocation = <1570, 8380, 10>;
    #declare cameraLookAt   = <1584, 8409, 3.2>;
  #break
  #case(601) // slitTube
    #declare cameraLocation = <2420, 8450, 10>;
    #declare cameraLookAt   = <2500, 8400, 0>;
    #declare cameraAngle = 30;
  #break
  #case(602) // gateC
    #declare cameraLocation = <2670, 8450, 10>;
    #declare cameraLookAt   = <2628, 8409, 3.2>;
  #break
  #case(1000) // whole optics line side view
    #declare cameraLocation = <2400, 8600, 10>;
    #declare cameraLookAt   = <2400, 8408, 0>;
    #declare cameraAngle = 170;
  #break
  #case(2000) // front-end and the side door
    #declare cameraLocation = <1870, 8200, 50>;
    #declare cameraLookAt   = <1930, 8511, 0>;
  #break
  #else // whole optics line top view
    #declare cameraLocation = <2400, 8450, 160>;
    #declare cameraLookAt   = <2400, 8408, 0>;
#break
#end

#declare C_Sun= rgb <1,0.98,0.96>;

#if (quick=1)
  light_source { // front
    cameraLocation + <-100,0,50> C_Sun
   shadowless
  }
  light_source { // back
    cameraLocation + <100,0,70> C_Sun
    shadowless
  }
#else
  light_source{cameraLocation + <100,0,70>,color C_Sun
    photons {
      refraction off
      reflection on
    }
    #if (AreaOK=1)
      area_light 100*x,100*y,5,5 adaptive 1 jitter orient circular
    #end
  }
#end

camera {
  #switch ( projection )
    #case(0)
      perspective
    #break
    #case(1)
      panoramic // angle 180 constant
    #break
    #case(2)
      omnimax
    #break
    #case(3)
      cylinder 1
    #break
  #end
  location cameraLocation
  look_at  cameraLookAt
  angle    cameraAngle
  sky z
  right -x*image_width/image_height
}

background { color White }

#include "b.inc"
