#version 3.7;
#include "colors.inc"
#include "metals.inc"
#include "stones.inc"
#include "woods.inc"
#include "textures.inc"
#include "shapes3.inc"

#declare omnimaxOK = 1;
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

#declare beamlineStart = -332;
#declare beamlineEnd   = 2770;

#declare cameraLocation = <beamlineStart-20+clock, 8450, 10>;
#declare cameraLookAt   = <beamlineStart+clock,    8400, 0>;
#if (cameraLocation.x-20<beamlineStart)
  #declare cameraLookAt   = <beamlineStart, 8400, 0>;
#end
#if (cameraLocation.x>beamlineEnd)
  #declare cameraLookAt   = <beamlineEnd, 8400, 0>;
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
  #if (omnimaxOK=1)
    omnimax
  #end
  location cameraLocation
  look_at  cameraLookAt
  angle 90
  sky z
  right -4/3*x
}

// camera {
//   location  < -200, 1170-clock*10, 50 >
//   look_at <0, 1175-clock*10, 0>
//   right <-1.33, 0, 0>
//   sky z
//   angle 70
// }


#include "b.inc"
