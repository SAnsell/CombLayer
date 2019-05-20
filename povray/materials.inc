// These are user-defined material definitions for POV-Ray which overwrite
// automatically generated colours.
// Feel free to edit this file according to your taste.

#declare matAluminium    = texture { pigment{color rgb <0.89, 0.91, 0.91> }};
#declare matB4C          = texture { pigment{color rgb <0.1, 0.1, 0.1> }};
#declare matConcrete     = texture {T_Wood8};

// http://news.povray.org/povray.general/thread/%3C35D92C3C.AD869B36%40pacbell.net%3E/
#declare matHDConcrete =
   texture { pigment { granite turbulence 1.5 color_map {
    [0  .25 color White color Gray75] [.25  .5 color White color Gray75]
    [.5 .75 color White color Gray75] [.75 1.1 color White color Gray75]}}
    finish { ambient 0.2 diffuse 0.3 crand 0.03 reflection 0 } normal {
    dents .5 scale .5 }}

#declare matSkanskaConcrete =
   texture { pigment { granite turbulence 1.5 color_map {
    [0  .25 color White color Gray95] [.25  .5 color White color White]
    [.5 .75 color White color White] [.75 1.1 color White color Gray85]}}
    finish { ambient 0.2 diffuse 0.3 crand 0.003 reflection 0 } normal {
    dents .5 scale .5 }}

#declare matLead         = texture { pigment{color rgb <0.61, 0.59, 0.54> }};
#declare matStainless304 = texture { Silver_Metal };
