// Usage:
// povray povray/singleItem.pov <<< '"CorrectorMag"'
#version 3.7;
#include "colors.inc"
#include "metals.inc"
#include "stones.inc"
#include "woods.inc"
#include "textures.inc"
#include "shapes3.inc"

#fopen STDIN "/dev/stdin" read
#read (STDIN, ITEM)

// #fopen STDOUT "/dev/stdout" write
// #write (STDOUT, ITEM)

// PROJECTION:
// 0=perspective
// 1=panoramic - good to display whole beam line
// 2=omnimax
// 3=cylindrical
#declare projection = 0;
#declare cameraAngle = 90;

#declare quick=0; // 0=quick but low quality, 1=slow but somewhat better quality
// another possibility to affect speed is command argument -q0 ... -q11
// so one can set quick=0 and play with -q0

#switch (0)
  #case (strcmp(ITEM,"ButtonBPM"))
    #declare cameraLocation = <-10, -10.0, 10.0>;
    #declare cameraLookAt   = <0.0, 0.0, 0.0>;
    #declare cameraAngle = 40;
  #break
  #case (strcmp(ITEM,"CeramicGap"))
    #declare cameraLocation = <-15, 20.0, 17.0>;
    #declare cameraLookAt   = <3.0, 5, -4.0>;
    #declare cameraAngle = 40;
  #break
  #case (strcmp(ITEM,"CleaningMagnet"))
    #declare cameraLocation = <50, 70, 50>;
    #declare cameraLookAt   = <0, 20, -5>;
    #declare cameraAngle = 35;
  #break
  #case(strcmp(ITEM,"CorrectorMag"))
    #declare cameraLocation = <-30, -30.0, 20.0>;
    #declare cameraLookAt   = <0, -3, 0.0>;
    #declare cameraAngle = 50;
  #break
  #case(strcmp(ITEM,"Jaws"))
    #declare cameraLocation = <50, -30, 50>;
    #declare cameraLookAt   = <0, 13, -3>;
    #declare cameraAngle = 30;
  #break
  #case (strcmp(ITEM,"LSexupole"))
    #declare cameraLocation = <-30, 70.0, 30.0>;
    #declare cameraLookAt   = <10.0, 0.0, -10.0>;
    #declare cameraAngle = 40;
  #break
  #case (strcmp(ITEM,"Sexupole"))
    #declare cameraLocation = <-30, 70.0, 30.0>;
    #declare cameraLookAt   = <10.0, 5.0, -11.0>;
    #declare cameraAngle = 25;
  #break
  #case (strcmp(ITEM,"uVac")) // UndulatorVacuum
    #declare cameraLocation = <-200, -50.0, 100.0>;
    #declare cameraLookAt   = <0.0, 140.0, 0.0>;
    #declare cameraAngle = 60;
  #break
  #case (strcmp(ITEM,"YagUnit"))
    #declare cameraLocation = <-30, -30.0, 40.0>;
    #declare cameraLookAt   = <0.0, 0.0, 10.0>;
    #declare cameraAngle = 50;
  #break
  #case (strcmp(ITEM,"YagUnitBig"))
    #declare cameraLocation = <-30, -30.0, 40.0>;
    #declare cameraLookAt   = <0.0, 0.0, 10.0>;
    #declare cameraAngle = 50;
  #break
  #case (strcmp(ITEM,"LocalShielding"))
    #declare cameraLocation = <-100, 100.0, 30.0>;
    #declare cameraLookAt   = <0.0, 40.0, -5.0>;
    #declare cameraAngle = 40;
  #break
  #case (strcmp(ITEM,"EBeamStop"))
    #declare cameraLocation = <-150, 120.0, 30.0>;
    #declare cameraLookAt   = <0.0, 30.0, -10.0>;
    #declare cameraAngle = 40;
  #break
#else
    #declare cameraLocation = <100, 100, 100>;
    #declare cameraLookAt   = <0, 0, 0>;
#end

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
