// Usage:
// povray povray/singleItem.pov <<< '"Segment15"'

#version 3.7;
#include "colors.inc"
#include "metals.inc"
#include "stones.inc"
#include "woods.inc"
#include "textures.inc"
#include "shapes3.inc"

#fopen STDIN "/dev/stdin" read
#read (STDIN, ITEM)

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

#switch ( 0 )
  // #case(0)
  //   #declare cameraLocation = <80, 17, 0>;
  //   #declare cameraLookAt   = <130, 4, 0>;
  //   #declare cameraAngle = 30;
  // #break
  // #case(1)
  //   #declare cameraLocation = <80, 130, 40>;
  //   #declare cameraLookAt   = <130, 80, 0>;
  //   #declare cameraAngle = 90;
  // #break
  #case(strcmp(ITEM,"Segment1")) // L2SPF segment 1
    #declare cameraLocation = <500, 630, 100>;
    #declare cameraLookAt   = <152, 670, 0>;
    #declare cameraAngle = 60;
    #declare projection = 0;
  #break
  #case(strcmp(ITEM,"Segment2")) // L2SPF segment 2
    #declare cameraLocation = <500, 1000, 100>;
    #declare cameraLookAt   = <152, 1100, 0>;
    #declare cameraAngle = 70;
    #declare projection = 0;
  #break
  #case(strcmp(ITEM,"Segment3")) // L2SPF segment 3
    #declare cameraLocation = <500, 1470, 100>;
    #declare cameraLookAt   = <152, 1510, 0>;
    #declare cameraAngle = 50;
    #declare projection = 0;
  #break
  #case(strcmp(ITEM,"Segment4")) // L2SPF segment 4
    #declare cameraLocation = <500, 1814, 100>;
    #declare cameraLookAt   = <152, 1800, 0>;
    #declare cameraAngle = 50;
    #declare projection = 0;
  #break
  #case(strcmp(ITEM,"Segment5")) // L2SPF segment 5
    #declare cameraLocation = <300, 2040, 200>;
    #declare cameraLookAt   = <90, 2030, 0>;
    #declare cameraAngle = 50;
    #declare projection = 0;
  #break
  #case(strcmp(ITEM,"Segment6")) // L2SPF segment 6
    #declare cameraLocation = <300, 2300, 200>;
    #declare cameraLookAt   = <90, 2305, 0>;
    #declare cameraAngle = 50;
    #declare projection = 0;
  #break
  #case(strcmp(ITEM,"Segment11")) // L2SPF segment 11
    #declare cameraLocation = <-120, 4220, 40>;
    #declare cameraLookAt   = <-360, 4030, 0>;
    #declare cameraAngle = 70;
    #declare projection = 0;
  #break
  #case(strcmp(ITEM,"Segment15")) // TDC segment 15
    #declare cameraLocation = <-433, 4980, 40>;
    #declare cameraLookAt   = <-485, 5030, 10>;
    #declare cameraAngle = 70;
    #declare projection = 0;
  #break
  #case(strcmp(ITEM,"Segment16")) // TDC segment 16
    #declare cameraLocation = <-400, 5320, 40>;
    #declare cameraLookAt   = <-485, 5320, 0>;
    #declare projection = 1;
  #break
  #case(strcmp(ITEM,"Segment16QuadA")) // TDC segment 16 - QuadA
    #declare cameraLocation = <-430, 4890, 40>;
    #declare cameraLookAt   = <-485, 4867, 0>;
    #declare cameraAngle = 50;
  #break
  #case(strcmp(ITEM,"Segment17")) // TDC segment 17
    #declare cameraLocation = <-430, 5450, 40>;
    #declare cameraLookAt   = <-485, 5520, 0>;
    #declare projection = 0;
    #declare cameraAngle = 70;
  #break
  #case(strcmp(ITEM,"Segment18")) // TDC segment 18
    #declare cameraLocation = <-400, 6300, 40>;
    #declare cameraLookAt   = <-485, 6350, 0>;
    #declare projection = 1;
  #break
  #case(strcmp(ITEM,"Segment19")) // TDC segment 19
    #declare cameraLocation = <-460, 6495, 20>;
    #declare cameraLookAt   = <-485, 6500, 0>;
    #declare projection = 1;
  #break
  #case(strcmp(ITEM,"Segment20")) // TDC segment 20
    #declare cameraLocation = <-655, 6960, 40>;
    #declare cameraLookAt   = <-485, 6750, 0>;
    #declare projection = 0;
    #declare cameraAngle = 50;
  #break
  #case(strcmp(ITEM,"Segment21")) // TDC segment 21
    #declare cameraLocation = <-655, 7000, 40>;
    #declare cameraLookAt   = <-485, 6920, 0>;
    #declare projection = 0;
    #declare cameraAngle = 50;
  #break
  #case(strcmp(ITEM,"Segment23")) // TDC segment 23
    #declare cameraLocation = <-350, 7290, 50>;
    #declare cameraLookAt   = <-485, 7355, 0>;
    #declare projection = 0;
    #declare cameraAngle = 50;
  #break
  #case(strcmp(ITEM,"Segment24")) // TDC segment 24
    #declare cameraLocation = <-300, 7950, 50>;
    #declare cameraLookAt   = <-485, 7750, 0>;
    #declare projection = 0;
    #declare cameraAngle = 60;
  #break
  #case(strcmp(ITEM,"Segment30")) // SPF segment 30
    #declare cameraLocation = <-400, 4520, 30>;
    #declare cameraLookAt   = <-485, 4520, 0>;
    #declare projection = 1;
  #break
  #case(strcmp(ITEM,"Segment31")) // SPF segment 31
    #declare cameraLocation = <-400, 4550, 30>;
    #declare cameraLookAt   = <-485, 4550, 0>;
    #declare projection = 1;
  #break
  #case(strcmp(ITEM,"Segment32")) // SPF segment 32
    #declare cameraLocation = <-350, 4755, 30>;
    #declare cameraLookAt   = <-485, 4600, 0>;
    #declare projection = 0;
    #declare cameraAngle = 70;
  #break
  #case(strcmp(ITEM,"Segment33")) // SPF segment 33
    #declare cameraLocation = <-290, 4570, 70>;
    #declare cameraLookAt   = <-485, 4580, 0>;
    #declare projection = 1;
//    #declare cameraAngle = 70;
  #break
  #case(strcmp(ITEM,"Segment35")) // SPF segment 35
    #declare cameraLocation = <-600, 6725, 70>;
    #declare cameraLookAt   = <-858, 6725, 0>;
    #declare projection = 0;
    #declare cameraAngle = 70;
  #break
  #case(strcmp(ITEM,"Segment36")) // SPF segment 36
    #declare cameraLocation = <-760, 6900, 30>;
    #declare cameraLookAt   = <-858, 6900, 0>;
    #declare projection = 1;
  #break
  #case(strcmp(ITEM,"Segment37")) // SPF segment 37
    #declare cameraLocation = <-760, 7200, 30>;
    #declare cameraLookAt   = <-858, 7250, 0>;
    #declare projection = 0;
    #declare cameraAngle = 60;
  #break
  #case(strcmp(ITEM,"Segment38")) // SPF segment 38
    #declare cameraLocation = <-760, 7600, 30>;
    #declare cameraLookAt   = <-858, 7600, 0>;
    #declare projection = 1;
  #break
  #case(strcmp(ITEM,"Segment39")) // SPF segment 39
    #declare cameraLocation = <-760, 7300, 60>;
    #declare cameraLookAt   = <-858, 7350, 0>;
    #declare projection = 0;
    #declare cameraAngle = 60;
  #break
  #case(strcmp(ITEM,"Segment41")) // SPF segment 41
    #declare cameraLocation = <-815, 8445, 60>;
    #declare cameraLookAt   = <-860, 8485, 0>;
    #declare projection = 0;
    #declare cameraAngle = 60;
  #break
  #case(strcmp(ITEM,"Segment43")) // SPF segment 43
    #declare cameraLocation = <-815, 9150, 40>;
    #declare cameraLookAt   = <-860, 9110, 0>;
    #declare projection = 0;
    #declare cameraAngle = 90;
  #break
  #case(strcmp(ITEM,"Segment45")) // SPF segment 45
    #declare cameraLocation = <-750, 9350, 50>;
    #declare cameraLookAt   = <-860, 9530, -120>;
    #declare projection = 0;
    #declare cameraAngle = 40;
  #break
  #case(strcmp(ITEM,"Segment46")) // SPF segment 46
    #declare cameraLocation = <-700, 6750, 70>;
    #declare cameraLookAt   = <-860, 6760, 0>;
    #declare projection = 0;
    #declare cameraAngle = 90;
  #break
  #case(strcmp(ITEM,"Segment47")) // SPF segment 47
    #declare cameraLocation = <-750, 6800, 100>;
    #declare cameraLookAt   = <-860, 6750, 0>;
    #declare projection = 0;
    #declare cameraAngle = 70;
  #break
  #case(strcmp(ITEM,"Segment48")) // SPF segment 48
    #declare cameraLocation = <-600, 6600, 100>;
    #declare cameraLookAt   = <-860, 6700, 0>;
    #declare projection = 0;
    #declare cameraAngle = 40;
  #break
  #case(strcmp(ITEM,"Segment49")) // SPF segment 49
    #declare cameraLocation = <-700, 10000, 30>;
    #declare cameraLookAt   = <-860, 10070, 0>;
    #declare projection = 0;
    #declare cameraAngle = 90;
  #break
  #case(strcmp(ITEM,"Segment1000")) // beamline view
    #declare cameraLocation = <200, 160, 40>;
    #declare cameraLookAt   = <125, 160, 0>;
    #declare cameraAngle = 170;
    #declare projection = 1;
  #break
  #case(strcmp(ITEM,"Segment1001")) // beamline back view
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

#include "a.inc"
