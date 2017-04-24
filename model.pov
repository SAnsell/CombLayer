#version 3.7;
#include  "colors.inc"
#include "metals.inc"

global_settings {
assumed_gamma 1.0
}

camera {
     location  < 250, -160, 0 >
     look_at <0, 0, 0>
     sky z
     angle 90
}

// light source

#for (lightY, -5000, 0, 1000)
light_source {
     < 0, lightY, 190 >
     color White
     fade_distance 10.75
     fade_power 2
}
#end

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
