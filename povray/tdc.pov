// Usage:
// povray povray/tdc.pov <<< '"Segment15"'

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
#declare projection = 1;
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
  #case(strcmp(ITEM,"Segment7")) // L2SPF segment 7
    #declare cameraLocation = <170, 2560, 100>;
    #declare cameraLookAt   = <-17, 2540, 0>;
    #declare cameraAngle = 70;
    #declare projection = 0;
  #break
  #case(strcmp(ITEM,"Segment8")) // L2SPF segment 8
    #declare cameraLocation = <300, 2770, 200>;
    #declare cameraLookAt   = <0, 2830, 0>;
    #declare cameraAngle = 50;
    #declare projection = 0;
  #break
  #case(strcmp(ITEM,"Segment9")) // L2SPF segment 9
    #declare cameraLocation = <0, 3140, 100>;
    #declare cameraLookAt   = <-152, 3115, 0>;
    #declare cameraAngle = 50;
    #declare projection = 0;
  #break
  #case(strcmp(ITEM,"Segment10")) // L2SPF segment 10
    #declare cameraLocation = <0, 4400, 200>;
    #declare cameraLookAt   = <-150, 4000, 70>;
    #declare cameraAngle = 40;
    #declare projection = 0;
  #break
  #case(strcmp(ITEM,"Segment11")) // L2SPF segment 11
    #declare cameraLocation = <-120, 4220, 40>;
    #declare cameraLookAt   = <-360, 4030, 0>;
    #declare cameraAngle = 70;
    #declare projection = 0;
  #break
  #case(strcmp(ITEM,"Segment12")) // L2SPF segment 12
    #declare cameraLocation = <-300, 4350, 200>;
    #declare cameraLookAt   = <-420, 4310, 0>;
    #declare cameraAngle = 70;
    #declare projection = 0;
  #break
  #case(strcmp(ITEM,"Segment13")) // L2SPF segment 13 [last]
    #declare cameraLocation = <-310, 4650, 100>;
    #declare cameraLookAt   = <-455, 4595, 0>;
    #declare cameraAngle = 70;
    #declare projection = 0;
  #break
  #case(strcmp(ITEM,"Segment14")) // TDC segment 14
    #declare cameraLocation = <-340, 4850, 200>;
    #declare cameraLookAt   = <-480, 4840, 0>;
    #declare cameraAngle = 80;
    #declare projection = 0;
  #break
  #case(strcmp(ITEM,"Segment15")) // TDC segment 15
    #declare cameraLocation = <-433, 4980, 40>;
    #declare cameraLookAt   = <-485, 5030, 10>;
    #declare cameraAngle = 70;
    #declare projection = 0;
  #break
  #case(strcmp(ITEM,"Segment16")) // TDC segment 16
    #declare cameraLocation = <-200, 5200, 100>;
    #declare cameraLookAt   = <-485, 5330, 0>;
    #declare cameraAngle = 50;
    #declare projection = 0;
  #break
  #case(strcmp(ITEM,"Segment16QuadA")) // TDC segment 16 - QuadA
    #declare cameraLocation = <-430, 4890, 40>;
    #declare cameraLookAt   = <-485, 4867, 0>;
    #declare cameraAngle = 50;
  #break
  #case(strcmp(ITEM,"Segment17")) // TDC segment 17
    #declare cameraLocation = <-80, 5890, 100>;
    #declare cameraLookAt   = <-485, 5880, 0>;
    #declare projection = 0;
    #declare cameraAngle = 90;
  #break
  #case(strcmp(ITEM,"Segment18")) // TDC segment 18
    #declare cameraLocation = <-350, 6300, 40>;
    #declare cameraLookAt   = <-485, 6350, 0>;
    #declare projection = 0;
    #declare cameraAngle = 90;
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
    #declare cameraLocation = <-355, 6850, 40>;
    #declare cameraLookAt   = <-485, 6900, 0>;
    #declare projection = 0;
    #declare cameraAngle = 50;
  #break
  #case(strcmp(ITEM,"Segment22")) // TDC segment 22
    #declare cameraLocation = <-655, 7350, 40>;
    #declare cameraLookAt   = <-485, 7190, 0>;
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
  #case(strcmp(ITEM,"Segment25")) // TDC segment 25
    #declare cameraLocation = <-300, 7900, 100>;
    #declare cameraLookAt   = <-485, 8000, 0>;
    #declare projection = 0;
    #declare cameraAngle = 60;
  #break
  #case(strcmp(ITEM,"Segment26")) // TDC segment 26
    #declare cameraLocation = <0, 8400, 0>;
    #declare cameraLookAt   = <-485, 8400, 0>;
    #declare projection = 0;
    #declare cameraAngle = 70;
  #break
  #case(strcmp(ITEM,"Segment27")) // TDC segment 27
    #declare cameraLocation = <0, 8780, 100>;
    #declare cameraLookAt   = <-485, 8750, 0>;
    #declare projection = 0;
    #declare cameraAngle = 70;
  #break
  #case(strcmp(ITEM,"Segment28")) // TDC segment 28
    #declare cameraLocation = <0, 9280, 100>;
    #declare cameraLookAt   = <-485, 9270, 0>;
    #declare projection = 0;
    #declare cameraAngle = 70;
  #break
  #case(strcmp(ITEM,"Segment29")) // TDC segment 29
    #declare cameraLocation = <0, 9940, 0>;
    #declare cameraLookAt   = <-485, 9740, 0>;
    #declare projection = 0;
    #declare cameraAngle = 50;
  #break
  #case(strcmp(ITEM,"Segment30")) // SPF segment 30
    #declare cameraLocation = <-400, 4520, 30>;
    #declare cameraLookAt   = <-485, 4520, 0>;
    #declare projection = 1;
  #break
  #case(strcmp(ITEM,"Segment31")) // SPF segment 31
    #declare cameraLocation = <-440, 5730, 40>;
    #declare cameraLookAt   = <-730, 5640, 0>;
    #declare projection = 0;
    #declare cameraAngle = 80;
  #break
  #case(strcmp(ITEM,"Segment32")) // SPF segment 32
    #declare cameraLocation = <-614, 6200, 70>;
    #declare cameraLookAt   = <-800, 6000, 0>;
    #declare projection = 0;
    #declare cameraAngle = 50;
  #break
  #case(strcmp(ITEM,"Segment33")) // SPF segment 33
    #declare cameraLocation = <-570, 6250, 70>;
    #declare cameraLookAt   = <-830, 6210, 0>;
    #declare projection = 0;
    #declare cameraAngle = 60;
  #break
  #case(strcmp(ITEM,"Segment34")) // SPF segment 34
    #declare cameraLocation = <-510, 6500, 170>;
    #declare cameraLookAt   = <-835, 6490, 0>;
    #declare projection = 0;
    #declare cameraAngle = 50;
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
    #declare cameraLocation = <-760, 7890, 60>;
    #declare cameraLookAt   = <-858, 7880, 0>;
    #declare projection = 0;
    #declare cameraAngle = 60;
  #break
  #case(strcmp(ITEM,"Segment40")) // SPF segment 40
    #declare cameraLocation = <-600, 8700, 100>;
    #declare cameraLookAt   = <-855, 8300, 0>;
    #declare projection = 0;
    #declare cameraAngle = 30;
  #break
  #case(strcmp(ITEM,"Segment41")) // SPF segment 41
    #declare cameraLocation = <-815, 8445, 60>;
    #declare cameraLookAt   = <-860, 8485, 0>;
    #declare projection = 0;
    #declare cameraAngle = 60;
  #break
  #case(strcmp(ITEM,"Segment42")) // SPF segment 42
    #declare cameraLocation = <-230, 8800, 200>;
    #declare cameraLookAt   = <-855, 8790, 0>;
    #declare projection = 0;
    #declare cameraAngle = 50;
  #break
  #case(strcmp(ITEM,"Segment43")) // SPF segment 43
    #declare cameraLocation = <-500, 9150, 50>;
    #declare cameraLookAt   = <-733, 9163, 0>;
    #declare projection = 0;
    #declare cameraAngle = 60;
  #break
  #case(strcmp(ITEM,"Segment44")) // SPF segment 44
    #declare cameraLocation = <-700, 9600, 100>;
    #declare cameraLookAt   = <-860, 9300, -40>;
    #declare projection = 0;
    #declare cameraAngle = 60;
  #break
  #case(strcmp(ITEM,"Segment45")) // SPF segment 45
    #declare cameraLocation = <-750, 9350, 50>;
    #declare cameraLookAt   = <-860, 9530, -120>;
    #declare projection = 0;
    #declare cameraAngle = 40;
  #break
  #case(strcmp(ITEM,"Segment46")) // SPF segment 46
    #declare cameraLocation = <-600, 9470, 150>;
    #declare cameraLookAt   = <-850, 9480, 0>;
    #declare projection = 0;
    #declare cameraAngle = 70;
  #break
  #case(strcmp(ITEM,"Segment47")) // SPF segment 47
    #declare cameraLocation = <-650, 9650, 100>;
    #declare cameraLookAt   = <-860, 9690, 0>;
    #declare projection = 0;
    #declare cameraAngle = 60;
  #break
  #case(strcmp(ITEM,"Segment48")) // SPF segment 48
    #declare cameraLocation = <-720, 9800, 100>;
    #declare cameraLookAt   = <-860, 9880, -10>;
    #declare projection = 0;
    #declare cameraAngle = 50;
  #break
  #case(strcmp(ITEM,"Segment49")) // SPF segment 49
    #declare cameraLocation = <-700, 9790, 30>;
    #declare cameraLookAt   = <-730, 9930, 0>;
    #declare projection = 0;
    #declare cameraAngle = 90;
  #break
  #case(strcmp(ITEM,"All")) // Whole beamline view
    #declare cameraLocation = <-500, 5200, 1000>;
    #declare cameraLookAt   = <-600, 5100, 0>;
    #declare projection = 1;
  #break
  #case(strcmp(ITEM,"TeraBack")) // Terahertz penetration [back view]
    #declare cameraLocation = <-138, 4200, 10>;
    #declare cameraLookAt   = <-138, 3410, 0>;
    #declare projection = 0;
    #declare cameraAngle = 70;
  #break
  #case(strcmp(ITEM,"SPFMaze")) // SPF access maze
    #declare cameraLocation = <-2800, 10400, 1000>;
    #declare cameraLookAt   = <-2000, 10000, 300>;
    #declare cameraAngle = 60;
    #declare projection = 0;
  #break
  #case(strcmp(ITEM,"Anders40")) // Picture for Anders
    #declare cameraLocation = <-600, 8900, 100>;
    #declare cameraLookAt   = <-855, 8300, 0>;
    #declare projection = 0;
    #declare cameraAngle = 30;
  #break
  #case(strcmp(ITEM,"Anders12")) // Picture for Anders @ split b/w SPF and TDC
    #declare cameraLocation = <-450, 4000, 130>;
    #declare cameraLookAt   = <-440, 4600, -50>;
    #declare cameraAngle = 30;
    #declare projection = 0;
  #break
  #case(strcmp(ITEM,"AndersAll")) // Picture for Anders @ whole beam line
    #declare cameraLocation = <-430, 6210, 100>;
    #declare cameraLookAt   = <-650, 6800, -250>;
    #declare cameraAngle = 50;
    #declare projection = 0;
  #break
  #case(strcmp(ITEM,"D1Duct")) // D1-D4 ducts in FKG
    #declare cameraLocation = <550, 6640, 100>;
    #declare cameraLookAt   = <65, 7450, 150>;
    #declare cameraAngle = 60;
    #declare projection = 0;
  #break
  #case(strcmp(ITEM,"BTG")) // BTG wall
    #declare cameraLocation = <430, 10429, 100>;
    #declare cameraLookAt   = <240, 10100, 150>;
    #declare cameraAngle = 60;
    #declare projection = 0;
  #break
  #case(strcmp(ITEM,"BDRoom")) // Main beam dump room (view from SPF hall)
    #declare cameraLocation = <-80, 9500, 50>;
    #declare cameraLookAt   = <-740, 9870, -300>;
    #declare cameraAngle = 90;
    #declare projection = 0;
  #break
  #case(strcmp(ITEM,"MidTFrontLShield")) //
    #declare cameraLocation = <450, 2800, 30>;
    #declare cameraLookAt   = <230, 3200, 0>;
    #declare cameraAngle = 60;
    #declare projection = 0;
  #break
  #case(strcmp(ITEM,"L2SPF11ShieldA"))
    #declare cameraLocation = <-190, 4030, 20>;
    #declare cameraLookAt   = <-250, 4070, 0>;
    #declare projection = 0;
    #declare cameraAngle = 70;
  #break
  #case(strcmp(ITEM,"L2SPF12ShieldA"))
    #declare cameraLocation = <-220, 4240, 20>;
    #declare cameraLookAt   = <-300, 4300, 0>;
    #declare projection = 0;
    #declare cameraAngle = 70;
  #break
  #case(strcmp(ITEM,"SPF31Quad"))
    #declare cameraLocation = <-600, 5514, 12>;
    #declare cameraLookAt   = <-570, 5495, 0>;
    #declare projection = 0;
    #declare cameraAngle = 60;
  #break
  #case(strcmp(ITEM,"SPF32DipoleA"))
    #declare cameraLocation = <-690, 5920, 20>;
    #declare cameraLookAt   = <-655, 5883, 0>;
    #declare projection = 0;
    #declare cameraAngle = 65;
  #break
  #case(strcmp(ITEM,"SPF33ShieldA")) // local shielding at SPF33 - img_5389.mp4
    #declare cameraLocation = <-750, 6200, 20>;
    #declare cameraLookAt   = <-690, 6100, 0>;
    #declare projection = 0;
    #declare cameraAngle = 50;
  #break
  #case(strcmp(ITEM,"SPF46ShieldA")) // local shielding at SPF46 - img_5375.jpg
    #declare cameraLocation = <-850, 9450, 20>;
    #declare cameraLookAt   = <-730, 9385, -20>;
    #declare projection = 0;
    #declare cameraAngle = 65;
  #break
  #case(strcmp(ITEM,"SPF47ShieldB")) // img_5384.jpg
    #declare cameraLocation = <-860, 9850, 40>;
    #declare cameraLookAt   = <-760, 9760, 0>;
    #declare projection = 0;
    #declare cameraAngle = 40;
  #break
  #case(strcmp(ITEM,"SPF48ShieldA")) // local shielding at SPF48 - img_5378.mp4
    #declare cameraLocation = <-850, 9830, 20>;
    #declare cameraLookAt   = <-730, 9900, -10>;
    #declare projection = 0;
    #declare cameraAngle = 50;
  #break
  #case(strcmp(ITEM,"SPF48ShieldC")) // img_5457.jpg
    #declare cameraLocation = <-600, 9900, 70>;
    #declare cameraLookAt   = <-735, 9830, 0>;
    #declare projection = 0;
    #declare cameraAngle = 70;
  #break
  #case(strcmp(ITEM,"SPF49ShieldA")) // local shielding at SPF49 - img_5378.mp4
    #declare cameraLocation = <-900, 9850, 20>;
    #declare cameraLookAt   = <-730, 10025, 0>;
    #declare projection = 0;
    #declare cameraAngle = 50;
  #break
    #case(strcmp(ITEM,"TDC16CMagH"))
    #declare cameraLocation = <-335, 5280, 20>;
    #declare cameraLookAt   = <-363, 5287, 7>;
    #declare projection = 0;
    #declare cameraAngle = 70;
  #break
    #case(strcmp(ITEM,"SPF33ShieldB"))
    #declare cameraLocation = <-633, 6235, 20>;
    #declare cameraLookAt   = <-705, 6320, 0>;
    #declare projection = 0;
    #declare cameraAngle = 60;
  #break
    #case(strcmp(ITEM,"TDC26ShieldA"))
    #declare cameraLocation = <-243, 7850, 20>;
    #declare cameraLookAt   = <-352, 8115, 0>;
    #declare projection = 0;
    #declare cameraAngle = 40;
  #break
    #case(strcmp(ITEM,"L2SPF2YagScreenInBeam"))
    #declare cameraLocation = <230, 1370, 70>;
    #declare cameraLookAt   = <280, 1345, 0>;
    #declare projection = 0;
    #declare cameraAngle = 40;
  #break
    #case(strcmp(ITEM,"L2SPF2YagScreen"))
    #declare cameraLocation = <230, 1370, 70>;
    #declare cameraLookAt   = <280, 1345, 0>;
    #declare projection = 0;
    #declare cameraAngle = 40;
  #break
    #case(strcmp(ITEM,"L2SPF2YagUnit"))
    #declare cameraLocation = <230, 1370, 70>;
    #declare cameraLookAt   = <280, 1345, 0>;
    #declare projection = 0;
    #declare cameraAngle = 40;
  #break
    #case(strcmp(ITEM,"L2SPF2ShieldA"))
    #declare cameraLocation = <140, 1270, 20>;
    #declare cameraLookAt   = <240, 1350, 0>;
    #declare projection = 0;
    #declare cameraAngle = 30;
  #break
    #case(strcmp(ITEM,"L2SPF3ShieldA"))
    #declare cameraLocation = <370, 1500, 50>;
    #declare cameraLookAt   = <240, 1500, 0>;
    #declare projection = 0;
    #declare cameraAngle = 80;
  #break
    #case(strcmp(ITEM,"L2SPF4ShieldA"))
//    #declare cameraLocation = <160, 1650, 20>;
    #declare cameraLocation = <130, 1850, 20>;
    #declare cameraLookAt   = <207, 1810, 0>;
    #declare projection = 0;
    #declare cameraAngle = 40;
  #break
    #case(strcmp(ITEM,"L2SPF6ShieldA"))
    #declare cameraLocation = <180, 2300, 50>;
    #declare cameraLookAt   = <150, 2315, 0>;
    #declare projection = 0;
    #declare cameraAngle = 50;
  #break
    #case(strcmp(ITEM,"L2SPF6EBeam"))
    #declare cameraLocation = <260, 2310, 40>;
    #declare cameraLookAt   = <132, 2385, -5>;
    #declare projection = 0;
    #declare cameraAngle = 50;
  #break
    #case(strcmp(ITEM,"L2SPF10ShieldA"))
    #declare cameraLocation = <120, 3170, 30>;
    #declare cameraLookAt   = <-55, 3185, 0>;
    #declare projection = 0;
    #declare cameraAngle = 20;
  #break
    #case(strcmp(ITEM,"TDC1")) // TDC beamline view backwards from the BSP01 storage area
    #declare cameraLocation = <200, 25000, 300>;
    #declare cameraLookAt   = <-170, 4000, -50>;
    #declare projection = 0;
    #declare cameraAngle = 2.7;
  #break
    #case(strcmp(ITEM,"TDC2")) // TDC beamline view forward from the klystron gallery in BC2
    #declare cameraLocation = <0, -12000, 500>;
    #declare cameraLookAt   = <10, 4000, -50>;
    #declare projection = 0;
    #declare cameraAngle = 2.7;
  #break
    #case(strcmp(ITEM,"SpectrometerDipole"))
    #declare cameraLocation = <200, 7100, 100>;
    #declare cameraLookAt   = <-300, 8150, 0>;
    #declare projection = 0;
    #declare cameraAngle = 30;
  #break
    #case(strcmp(ITEM,"L2SPF"))
    #declare cameraLocation = <500, 1700, 100>;
    #declare cameraLookAt   = <300, 1700, 0>;
    #declare projection = 1;
  #break
  #else // whole beamline view (like All)
    #declare cameraLocation = <-500, 5200, 1000>;
    #declare cameraLookAt   = <-600, 5100, 0>;
    #declare projection = 1;
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
