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


// SkanskaConcrete
#declare mat234 = texture { Concrete1 };
