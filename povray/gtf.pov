// Usage example:
// povray povray/gtf.pov
// (it is expected that /tmp/gtf.sh contains the item to show, e.g. "Line")

#version 3.7;
#include "colors.inc"
#include "metals.inc"
#include "stones.inc"
#include "woods.inc"
#include "textures.inc"
#include "shapes3.inc"

#fopen STDIN "/tmp/gtf.txt" read
#read (STDIN, ITEM)

// PROJECTION:
// 0=perspective
// 1=panoramic - good to display whole beam line
// 2=omnimax
// 3=cylindrical
#declare projection = 1;
#declare cameraAngle = 150;

#declare quick=1; // 0=quick but low quality, 1=slow but somewhat better quality
// another possibility to affect speed is command argument -q0 ... -q11
// so one can set quick=0 and play with -q0

#declare C_Sun= rgb <1,0.98,0.96>;

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

#switch ( 0 )
  #case(strcmp(ITEM,"Door"))
    #declare cameraLocation = <169, 271, 40>;
    #declare cameraLookAt   = <152, 525, 0>;
    #declare cameraAngle = 90;
    #declare projection = 0;
  #break
  #case(strcmp(ITEM,"Gate"))
    #declare cameraLocation = <-90, 245, 150>;
    #declare cameraLookAt   = <0, 145, 0>;
    #declare cameraAngle = 35;
    #declare projection = 0;
  #break
  #case(strcmp(ITEM,"RFGun"))
    #declare cameraLocation = <-215, 250, 40>;
    #declare cameraLookAt   = <0, 195, 0>;
    #declare cameraAngle = 35;
    #declare projection = 0;
  #break
  #case(strcmp(ITEM,"LaserChamber"))
    #declare cameraLocation = <-215, 250, 40>;
    #declare cameraLookAt   = <0, 110, 0>;
    #declare cameraAngle = 20;
    #declare projection = 0;
  #break
  #case(strcmp(ITEM,"Line"))
    #declare cameraLocation = <-215, 271, 40>;
    #declare cameraLookAt   = <0, 270, 3>;
    #declare cameraAngle = 35;
    #declare projection = 0;
  #break
  #case(strcmp(ITEM,"CurrentTransformer"))
    #declare cameraLocation = <90, 170, 40>;
    #declare cameraLookAt   = <0, 216, 3>;
    #declare cameraAngle = 35;
    #declare projection = 0;
    light_source { // LaserChamber centre (coordinate based on ObjectRegister.txt)
      <0,210.9,0> Violet
    }
  #break
  #else // default view
    #declare cameraLocation = <175, -275, 0>;
    #declare cameraLookAt   = <0, 245, 0>;
    #declare projection = 1;
#break
#end


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

#include "a.inc"
