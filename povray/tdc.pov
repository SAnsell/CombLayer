#version 3.7;
#include "colors.inc"
#include "metals.inc"
#include "stones.inc"
#include "woods.inc"
#include "textures.inc"
#include "shapes3.inc"

#declare view = 19;
// PROJECTION:
// 0=perspective
// 1=panoramic - good to display whole beam line
// 2=omnimax
// 3=cylindrical
#declare projection = 0;
#declare cameraAngle = 150;

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
  #case(0)
    #declare cameraLocation = <80, 17, 0>;
    #declare cameraLookAt   = <130, 4, 0>;
    #declare cameraAngle = 30;
  #break
  #case(1)
    #declare cameraLocation = <80, 130, 40>;
    #declare cameraLookAt   = <130, 80, 0>;
    #declare cameraAngle = 90;
  #break
  #case(15) // TDC segment 15
    #declare cameraLocation = <-433, 4980, 40>;
    #declare cameraLookAt   = <-485, 5030, 10>;
    #declare cameraAngle = 70;
    #declare projection = 0;
  #break
  #case(16) // TDC segment 16
    #declare cameraLocation = <-400, 5320, 40>;
    #declare cameraLookAt   = <-485, 5320, 0>;
    #declare projection = 1;
  #break
  #case(1604) // TDC segment 16 - QuadA
    #declare cameraLocation = <-430, 4890, 40>;
    #declare cameraLookAt   = <-485, 4867, 0>;
    #declare cameraAngle = 50;
  #break
  #case(17) // TDC segment 17
    #declare cameraLocation = <-430, 5450, 40>;
    #declare cameraLookAt   = <-485, 5520, 0>;
    #declare projection = 0;
    #declare cameraAngle = 70;
  #break
  #case(18) // TDC segment 18
    #declare cameraLocation = <-400, 6300, 40>;
    #declare cameraLookAt   = <-485, 6350, 0>;
    #declare projection = 1;
  #break
  #case(19) // TDC segment 19
    #declare cameraLocation = <-480, 6500, 40>;
    #declare cameraLookAt   = <-485, 6500, 0>;
    #declare projection = 1;
  #break
  #case(1000) // beamline view
    #declare cameraLocation = <200, 160, 40>;
    #declare cameraLookAt   = <125, 160, 0>;
    #declare cameraAngle = 170;
    #declare projection = 1;
  #break
  #case(1001) // beamline back view
    #declare cameraLocation = <200, 550, 40>;
    #declare cameraLookAt   = <125, 400, 0>;
    #declare cameraAngle = 90;
  #break
  #else
    #declare cameraLocation = <100, 100, 100>;
    #declare cameraLookAt   = <0, 0, 0>;
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
