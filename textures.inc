#declare Concrete =
   texture { pigment { granite turbulence 1.5 color_map {
    [0  .25 color White color Gray75] [.25  .5 color White color Gray75]
    [.5 .75 color White color Gray75] [.75 1.1 color White color Gray75]}}
    finish { ambient 0.2 diffuse 0.3 crand 0.03 reflection 0 } normal {
    dents .5 scale .5 }}

#declare Concrete1 =	
   texture { pigment { granite turbulence 1.5 color_map {
    [0  .25 color White color Gray95] [.25  .5 color White color White]
    [.5 .75 color White color White] [.75 1.1 color White color Gray85]}}
    finish { ambient 0.2 diffuse 0.3 crand 0.003 reflection 0 } normal {
    dents .5 scale .5 }}

// Steel
#declare mat3 = texture {
	 pigment{color rgb<0.0,0.802342191,0.596864313> transmit 0 }
	 finish {
	 	ambient 0.1
		diffuse 0.9
		phong 1
	 	metallic
		}
	 };


// SkanskaConcrete
#declare mat234 = texture { Concrete1 };
// SS304L
//#declare mat236 = texture { pigment { rgb <0.80, 0.45, 0.35> }
//	                    finish {reflection {1.0} metallic }
//			    };
