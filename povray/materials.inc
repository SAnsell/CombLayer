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

////////////////////// LEAD
// http://www.oyonale.com/modeles.php?lang=en&page=45

#declare N_Lead=normal{
    bump_map{jpeg "povray/stone09.jpg"}
    bump_size -5
    rotate x*90
//    scale 200
}

#declare matLead=texture{
    pigment{
        gradient x
        pigment_map{
            [0.9 wrinkles turbulence 1 lambda 3
	      color_map{
		[0 rgb <0.9,0.93,0.91>*0.7][1 rgb <0.64,0.62,0.59>*0.5]
	      }
	    ]
//	  [0.9 color rgb <0.85,0.7,0.1>] // vertical yellow line
	}
        scale 100
    }
    normal{N_Lead}
    finish{
        ambient 0
        diffuse 1
    }
//    rotate -y*20
}

///////////////////// RUSTY METAL
// http://www.oyonale.com/modeles.php?lang=en&page=45

#declare T_Shiny=texture{
    pigment {color rgb 0.1}
    finish{ambient 0 diffuse 1 metallic brilliance 4 specular 1 roughness 0.02 reflection {0.8,1}}
}
#declare T_Rusty=texture{
    pigment {bozo turbulence 1 color_map{[0.3 DarkWood*0.5][0.5 DarkBrown*0.5]}}
    normal{bozo turbulence 1 lambda 4 bump_size 1 scale 1/50}
    finish{ambient 0 diffuse 1 specular 0.01 roughness 0.1}
}

#declare matStainless304=texture{
    bozo turbulence 1 lambda 4
    texture_map{
        [0.3 T_Shiny]
        [0.7 T_Rusty]
//        [0.5 pigment{Red}]
//        [0.5 pigment{Blue}]
    }
    scale 50
}
////////////////////////
