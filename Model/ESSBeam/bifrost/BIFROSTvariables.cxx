/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:    ESSBeam/bifrost/BIFROSTvariables.cxx
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. 
 *
 ****************************************************************************/
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <cmath>
#include <complex>
#include <list>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <iterator>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "support.h"
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "FocusGenerator.h"
#include "ShieldGenerator.h"
#include "ChopperGenerator.h"
#include "PitGenerator.h"
#include "PipeGenerator.h"
#include "RectPipeGenerator.h"
#include "JawGenerator.h"
#include "BladeGenerator.h"
#include "essVariables.h"
#include "variableSetup.h"
/*BIFROST optical system in TCS(model rotated 270 degrees respective to as constructed):
Feeder: (-101.911, -11.697, -137)
Position of start of elliptic section in TCS: (-1362.217, 1487.977, 137.000)
Angle between major axis and x-axis in TCS: 132.140 deg
Minor axis (b) horizontal plane (diameter equivalent): 69.634
Major axis (a) horizontal plane (diameter equivalent): 8268.485
Minor axis (b) Vertical direction (diameter equivalent): 48.862
Major axis (a) Vertical direction (diameter equivalent): 9243.211
Distance from start of guide section to center of horizontal ellipse: 664.511
Distance from start of guide section to center of vertical ellipse: 3249.748
Distance from start of guide section to end of guide section: 4396.000

Values usefull for double checking:
Position of end of Ellipse in TCS: (-4311.689, 4747.644, 137.000)
Horizontal width at section start: 68.728
Vertical Height at section start: 34.742
Horizontal width at section end: 29.978
Vertical Height at section end: 47.336


Curving Section:
Starting point in TCS: (-4358.185, 4799.031, 137.000)
Ending Point in TCS: (-16511.270, 18071.328, 137.000)
Width (mm): 29.5320
Height(mm): 47.5140
Radius of Curvature (mm): 1518532.237


Expanding section (ellipse):
Position of start of elliptic section in TCS: (-16511.270, 18071.328, 137.000)
Angle between major axis and x-axis in TCS: 132.819 deg
Minor axis (b) horizontal plane (diameter equivalent): 60.000
Major axis (a) horizontal plane (diameter equivalent): 57277.085
Minor axis (b) Vertical direction (diameter equivalent): 90.000
Major axis (a) Vertical direction (diameter equivalent): 58705.325
Distance from start of guide section to center of horizontal ellipse: 24928.80066
Distance from start of guide section to center of vertical ellipse: 24928.800
Distance from start of guide section to end of guide section: 24928.800

Values usefull for double checking:
Position of end of Ellipse in TCS: (-33455.112, 36356.849, 137.000)
Horizontal width at section start: 29.532
Vertical Height at section start: 47.514
Horizontal width at section end: 59.996
Vertical Height at section end: 90.000


Straight section:
Starting point in TCS: (-33455.112, 36356.849, 137.000)
Ending Point in TCS:  (-94626.724, 102372.255, 137.000)
Width (mm): 60.000
Height(mm): 90.000


Focusing section (ellipse):
Position of start of elliptic section in TCS: (-94626.724, 102372.255, 137.000)
Angle between major axis and x-axis in TCS: 132.819 deg
Minor axis (b) horizontal plane (diameter equivalent): 60.000
Major axis (a) horizontal plane (diameter equivalent): 48729.121
Minor axis (b) Vertical direction (diameter equivalent): 90.000
Major axis (a) Vertical direction (diameter equivalent): 46068.954
Distance from start of guide section to center of horizontal ellipse: 0.000
Distance from start of guide section to center of vertical ellipse: 0.000
Distance from start of guide section to end of guide section: 22114.200

Values usefull for double checking:
Position of end of Ellipse in TCS: (-109657.267, 118592.968, 137.000)
Horizontal width at section start: 59.996
Vertical Height at section start: 90.000
Horizontal width at section end: 25.184
Vertical Height at section end: 25.186

Sample Position in TCS: (-110051.484, 119018.400, 137.000)

 */
namespace setVariable
{
  
void
BIFROSTvariables(FuncDataBase& Control)
  /*!
    Create all the beamline variables for vor
    \param Control :: DataBase
  */
{
  ELog::RegMethod RegA("BIFROSTvariables[F]","BIFROSTvariables");

  setVariable::ChopperGenerator CGen;
  setVariable::FocusGenerator FGen;
  setVariable::ShieldGenerator SGen;
  setVariable::PitGenerator PGen;
  setVariable::PipeGenerator PipeGen;
  setVariable::RectPipeGenerator RPipeGen;
  setVariable::BladeGenerator BGen;

  int iLayerFlag=1;
  Control.addVariable("LayerFlag",iLayerFlag);
  /* //Original:
  PipeGen.setPipe(8.0,0.5);
  PipeGen.setWindow(-2.0,0.5);
  PipeGen.setFlange(-4.0,1.0);
  */

  //simplified version
  //  PipeGen.setPipe(14.0,0.5);
  //  PipeGen.setWindow(-1.0,0.5);
  // PipeGen.setFlange(-2.0,1.0);

  /* //unsuccesfull attempt to split guide bay
std::string baseKey="G1BLineTop4";
  int iseg=6;
 Control.addVariable("G1BLineTop4NSegment",2*iseg);
 for (int i=1; i<=iseg; i++){
   Control.addVariable(baseKey+"Width"+std::to_string(i),20.0);
      Control.addVariable(baseKey+"Depth"+std::to_string(i),10.0);
      Control.addVariable(baseKey+"Height"+std::to_string(i),12.0);
      Control.addVariable(baseKey+"Length"+std::to_string(i),170.0/iseg);
 };

 for (int i=iseg+1; i<=2*iseg; i++){
   Control.addVariable(baseKey+"Width"+std::to_string(i),28.0);
      Control.addVariable(baseKey+"Depth"+std::to_string(i),22.0);
      Control.addVariable(baseKey+"Height"+std::to_string(i),22.0);
      Control.addVariable(baseKey+"Length"+std::to_string(i),170.0/iseg);
 };
  */
  
  SGen.addWall(1,20.0,"CastIron");
  SGen.addRoof(1,20.0,"CastIron");
  SGen.addFloor(1,20.0,"CastIron");
  SGen.addFloorMat(5,"LimestoneConcrete");
  SGen.addRoofMat(5,"LimestoneConcrete");
  SGen.addWallMat(5,"LimestoneConcrete");

  // extent of beamline
  Control.addVariable("bifrostStopPoint",0);
  Control.addVariable("bifrostAxisXYAngle",-0.56);   // rotation in XY plane
  Control.addVariable("bifrostAxisZAngle",0.0);   // rotation out of XY plane
  Control.addVariable("bifrostAxisZStep",0.0);   // +/- height
  Control.addVariable("bifrostAxisXStep",-1.28);   // +/- horiz. position
  FGen.setLayer(3,0.3,"Void");
  FGen.setLayer(2,0.3,"Void");
  FGen.setLayer(1,0.8,"Copper"); // 8mm copper substrate in monolith plug

  // Length and offsets ensure proper width at start and end points.
  FGen.setYOffset(1.75);
  //  FGen.generateTaper(Control,"bifrostFA",439.6,6.86 ,3.00,3.4672,4.73);
  FGen.generateTaper(Control,"bifrostFA",439.6,6.8728 ,2.9978,3.4672,4.73);


    FGen.generateTaperGeneral(Control,"heimdalFTA",334.819, 7.096,2.584,
  			    3.154,2.584,
  			    1.724,2.507,
  			    1.724,2.507);
    double xstart=0;
    double xend=0;
    double lstart=0,rstart=0;
    double lend=0,rend=0;
    double tstart = 0;
    double tend = 0;

    for (int i=0; i< 20; i++){
      lstart = 6.8728/2.; // 7.096;
      lend =  lstart - (lstart-2.9978/2.)*350./439.6; //6.53 - 3.704;
      rstart = lstart; // 3.154;
      rend=lend; //3.704;
      tstart=3.4672/2.; //1.724;
      tend= tstart - (tstart - 4.73/2.)*350/439.6; // 2.343;
      FGen.setLayer(1,1.2,"Copper"); // 12mm copper substrate in monolith plug
      if ((i>2)&&(i<6)||(i>11)&&(i<15))   FGen.setLayer(1,2.5,"Copper"); // 25 in some places

	FGen.generateTaperGeneral(Control,"bifrostFSplit"+std::to_string(i),
				  350./20.,  //439.6/20.,
			    lstart+(lend-lstart)*i/20.0, lstart+(lend-lstart)*(i+1)/20,
  			    rstart+(rend-rstart)*i/20.0, rstart+(rend-rstart)*(i+1)/20,
  			    tstart+(tend-tstart)*i/20.0, tstart+(tend-tstart)*(i+1)/20,
			    tstart+(tend-tstart)*i/20.0, tstart+(tend-tstart)*(i+1)/20);
      Control.addVariable("bifrostFSplit"+std::to_string(i)+"BeamYStep",0.0);
      Control.addVariable("bifrostFSplit"+std::to_string(i)+"YStep",0.0);
      FGen.clearYOffset();}

  FGen.setLayer(1,0.8,"Copper"); // 8mm copper substrate in monolith plug

  // Length and offsets ensure proper width at start and end points.
  FGen.setYOffset(1.75);


    /*
  
  FGen.generateTaper(Control,"bifrostFSplit0",15,6.8728 ,2.9978,3.4672,4.73);
  FGen.generateTaper(Control,"bifrostFSplit1",15,6.8728 ,2.9978,3.4672,4.73);
  
  FGen.generateTaper(Control,"bifrostFSplit2",15,6.8728 ,2.9978,3.4672,4.73);

  FGen.generateTaper(Control,"bifrostFSplit3",15,6.8728 ,2.9978,3.4672,4.73);

  FGen.generateTaper(Control,"bifrostFSplit4",15,6.8728 ,2.9978,3.4672,4.73);

  FGen.generateTaper(Control,"bifrostFSplit5",15,6.8728 ,2.9978,3.4672,4.73);

  FGen.generateTaper(Control,"bifrostFSplit6",15,6.8728 ,2.9978,3.4672,4.73);

  FGen.generateTaper(Control,"bifrostFSplit7",15,6.8728 ,2.9978,3.4672,4.73);

  FGen.generateTaper(Control,"bifrostFSplit8",15,6.8728 ,2.9978,3.4672,4.73);

  FGen.generateTaper(Control,"bifrostFSplit9",15,6.8728 ,2.9978,3.4672,4.73);

  FGen.generateTaper(Control,"bifrostFSplit10",15,6.8728 ,2.9978,3.4672,4.73);
FGen.generateTaper(Control,"bifrostFSplit11",15,6.8728 ,2.9978,3.4672,4.73);
FGen.generateTaper(Control,"bifrostFSplit12",15,6.8728 ,2.9978,3.4672,4.73);
FGen.generateTaper(Control,"bifrostFSplit13",15,6.8728 ,2.9978,3.4672,4.73);
FGen.generateTaper(Control,"bifrostFSplit14",15,6.8728 ,2.9978,3.4672,4.73);
FGen.generateTaper(Control,"bifrostFSplit15",15,6.8728 ,2.9978,3.4672,4.73);
FGen.generateTaper(Control,"bifrostFSplit16",15,6.8728 ,2.9978,3.4672,4.73);
FGen.generateTaper(Control,"bifrostFSplit17",15,6.8728 ,2.9978,3.4672,4.73);
FGen.generateTaper(Control,"bifrostFSplit18",15,6.8728 ,2.9978,3.4672,4.73);
FGen.generateTaper(Control,"bifrostFSplit19",15,6.8728 ,2.9978,3.4672,4.73);
    */
  FGen.generateTaper(Control,"bifrostSplit0",15,6.8728 ,2.9978,3.4672,4.73);
  FGen.generateTaper(Control,"bifrostSplit1",15,6.8728 ,2.9978,3.4672,4.73);
  
  FGen.generateTaper(Control,"bifrostSplit2",15,6.8728 ,2.9978,3.4672,4.73);

  FGen.generateTaper(Control,"bifrostSplit3",15,6.8728 ,2.9978,3.4672,4.73);

  FGen.generateTaper(Control,"bifrostSplit4",15,6.8728 ,2.9978,3.4672,4.73);

  FGen.generateTaper(Control,"bifrostSplit5",15,6.8728 ,2.9978,3.4672,4.73);

  FGen.generateTaper(Control,"bifrostSplit6",15,6.8728 ,2.9978,3.4672,4.73);

  FGen.generateTaper(Control,"bifrostSplit7",15,6.8728 ,2.9978,3.4672,4.73);

  FGen.generateTaper(Control,"bifrostSplit8",15,6.8728 ,2.9978,3.4672,4.73);

  FGen.generateTaper(Control,"bifrostSplit9",15,6.8728 ,2.9978,3.4672,4.73);

  FGen.generateTaper(Control,"bifrostSplit10",15,6.8728 ,2.9978,3.4672,4.73);
FGen.generateTaper(Control,"bifrostSplit11",15,6.8728 ,2.9978,3.4672,4.73);
FGen.generateTaper(Control,"bifrostSplit12",15,6.8728 ,2.9978,3.4672,4.73);
FGen.generateTaper(Control,"bifrostSplit13",15,6.8728 ,2.9978,3.4672,4.73);
FGen.generateTaper(Control,"bifrostSplit14",15,6.8728 ,2.9978,3.4672,4.73);
FGen.generateTaper(Control,"bifrostSplit15",15,6.8728 ,2.9978,3.4672,4.73);
FGen.generateTaper(Control,"bifrostSplit16",15,6.8728 ,2.9978,3.4672,4.73);
FGen.generateTaper(Control,"bifrostSplit17",15,6.8728 ,2.9978,3.4672,4.73);
FGen.generateTaper(Control,"bifrostSplit18",15,6.8728 ,2.9978,3.4672,4.73);
FGen.generateTaper(Control,"bifrostSplit19",15,6.8728 ,2.9978,3.4672,4.73);
    

  FGen.setLayer(2,0.1,"Void");
  FGen.setLayer(3,0.0,"Void");
 
  // Pipe in gamma shield. Numerical arguments: offset, length
  //  PipeGen.generatePipe(Control,"bifrostPipeB",8.0,46.0);

    RPipeGen.setPipe(13.0,22.0,0.5);
   /*
  RPipeGen.setWindow(0.0,0.0,0.0);
   */
    RPipeGen.setFlange(14.0,23.0,1.0);
  
   //same as for HEIMDAL
    RPipeGen.generatePipe(Control,"bifrostPipeB",6.5,46.0);
    Control.addVariable("bifrostPipeBWindowActive",0.0);

  

  PipeGen.setWindow(0.0,0.0);
  PipeGen.setFlange(5.0,0.5); //inner flange diameter, flange width
   PipeGen.setPipe(4.5,0.5); //diameter thickness
   PipeGen.generatePipe(Control,"bifrostPipeC",0.5,30.5);//offset,length
  // Control.addVariable("bifrostPipeCWindowActive",0.0);



   
   FGen.setLayer(1,0.8,"Copper"); //8mm Cu substrate in light shutter, pathlength biasing
  
  // FGen.clearYOffset();
  FGen.generateTaper(Control,"bifrostFB", 439.6, 6.86, 3.00, 3.4672, 4.73);
  //  FGen.generateTaper(Control,"bifrostFB",44.0, 5.0,4.0, 5.0,4.0);
 
   FGen.setLayer(1,0.8,"Aluminium"); //8mm Al substrate beyond light shutter

   FGen.generateTaper(Control,"bifrostFC", 439.6, 6.86, 3.00, 3.4672, 4.73);
  
  Control.addVariable("bifrostAppAInnerWidth",0.0);
  Control.addVariable("bifrostAppAInnerHeight",0.0);
  Control.addVariable("bifrostAppAWidth",12.0);
  Control.addVariable("bifrostAppAHeight",12.0);
  Control.addVariable("bifrostAppAThick",7.0);
  Control.addVariable("bifrostAppAYStep",3.5);//offset
  Control.addVariable("bifrostAppAConstruct",0); // set to 1 to construct
  Control.addVariable("bifrostAppADefMat","Copper");

  // VACBOX A : 6.50m target centre
  //  Length 100.7 + Width [87.0] + Height [39.0] void Depth/2 + front
  CGen.setMainRadius(38.122);
  CGen.setFrame(82,82);
  // Chopper will be fixed with its wall to the VPipeC, hense offset is 1/2 of length
  CGen.generateChopper(Control,"bifrostChopperA", 11.0,22.0,20);
   // CGen.generateChopper(Control,"bifrostChopperA", 24.0,12.0,6.55);
  //numbers: offset / chopper case size along the beam / chopper inner space thick

 Control.addVariable("bifrostChopperAFrontFlangeNBolts",0);
 Control.addVariable("bifrostChopperABackFlangeNBolts",0);

 Control.addVariable("biforstChopperAIPortANBolt",0);
 Control.addVariable("bifrostChopperAIPortBNBolt",0);

  
  // // T0 chopper if needed
  //  CGen.generateChopper(Control,"bifrostChopperA", 20.0, 36.0 ,32.0);
  // BGen.setMaterials("Inconnel","Tungsten");
  // BGen.setThick({20.0});
  // BGen.addPhase({95},{60.0});
  // BGen.generateBlades(Control,"bifrostDBlade",10.0,25.0,35.0);



  
  BGen.setThick({0.2,0.2}); //Chopper blades thickness
  BGen.addPhase({95,275},{60.0,60.0});
  BGen.addPhase({95,275},{60.0,60.0});
  BGen.generateBlades(Control,"bifrostDBlade",0.5,20.0,35.0);
    //numbers: displacement of chopper blades wrt the box center, inner, outer radius
    
  // VACUUM PIPE: SDisk to T0 (A)
  PipeGen.setPipe(6.0,0.5); //numbers: radius, wall thickness
  // PipeGen.setWindow(0.0,0.0);
  PipeGen.setFlange(6.5,0.5); //inner radius, flange thickness
  Control.addVariable("bifrostPipeBAWindowActive",0.0);
  Control.addVariable("bifrostPipeBAFlangeLength",-1.0);
  //  Control.addVariable("bifrostPipeBAYStep",140.0); // will be set WRT Lightshutter
  //    Control.addVariable("bifrostPipeBAYStep",6.0); // will be set w.r.t. ChopperA  


  PipeGen.generatePipe(Control,"bifrostPipeBA",0.0,208.0); //numbers: offset, length
  Control.addVariable("bifrostPipeBANDivision",1);
  //  Control.addVariable("bifrostPipeBAXYAngle",0.1);

  //   FGen.clearYOffset();
  FGen.setYOffset(6.93); //Distance between end of Focus and start of Bender
  //  FGen.generateTaper(Control,"bifrostFC",396.0, 5.0 ,13.0, 5.0,13.0);   
  //  Control.addVariable("bifrostFCBeamYStep",1.10); 

  const double bendAngle(180.0);
  const double bendRadius(151853.2237);    // ~1.5km
   FGen.generateBender(Control,"bifrostBA",215.0,2.9534,2.9534,4.7514,4.7514,
                      bendRadius,bendAngle);



   for (int Nstr=0; Nstr<59;Nstr++){   
   FGen.generateBender(Control,"bifrostPhantom"+std::to_string(Nstr),
		       30+Nstr*30,2.9534,2.9534,4.7514,4.7514,
                      bendRadius,bendAngle);
   /*
   Control.addVariable("bifrostPhantom"+std::to_string(Nstr)+"Length",30);
     std::string keyName="bifrostPhantom";
     std::string NStr = std::to_string(Nstr);
     Control.addVariable(keyName+NStr+"AHeight",4.7514);
     Control.addVariable(keyName+NStr+"BHeight",4.7514);
     Control.addVariable(keyName+NStr+"AWidth",2.9534);
	  Control.addVariable(keyName+NStr+"BWidth",2.9534);
	  // angular rotation of bend direciton from +Z
	  Control.addVariable(keyName+NStr+"AngDir",180);
  	  Control.addVariable(keyName+NStr+"Radius",151853.2237);
   */
   }


   
  //Chopper is placed 2.15 m from the start of the curving section, gap 4cmB
  CGen.setMainRadius(38.122);
  CGen.setFrame(82,82);
  CGen.generateChopper(Control,"bifrostChopperB",3.0,6.0,4.0);

 Control.addVariable("bifrostChopperBFrontFlangeNBolts",0);
 Control.addVariable("bifrostChopperBBackFlangeNBolts",0);

 Control.addVariable("biforstChopperBIPortANBolt",0);
 Control.addVariable("bifrostChopperBIPortBNBolt",0);


  // Singe Blade chopper FOC
  BGen.setThick({0.2});
  BGen.addPhase({95},{60.0});
  BGen.generateBlades(Control,"bifrostFOC1Blade",0.0,20.0,35.0);

  // VACUUM PIPE: from ChoperB to 6m holding point
  PipeGen.setPipe(6.0,0.5);
  /*  PipeGen.setWindow(-2.0,0.5);*/
  PipeGen.setFlange(6.5,0.5);
  
  PipeGen.generatePipe(Control,"bifrostPipeBB",1.0,320.0);
  Control.addVariable("bifrostPipeBBXYAngle",0.15);
  Control.addVariable("bifrostPipeBBNDivision",1);

  //  FGen.clearYOffset();
  //  FGen.generateRectangle(Control,"bifrostFD",496.0, 13.0,13.0);   
  //Bender continues, chopper is placed 8.59 m after start of curved section
  FGen.generateBender(Control,"bifrostBB",859.0,2.9534,2.9534,4.7514,4.7514,
                      bendRadius,bendAngle);

  // VACUUM PIPE: from ChoperB to 6m holding point
  PipeGen.setPipe(6.0,0.5);
  /*  PipeGen.setWindow(-2.0,0.5);*/
  PipeGen.setFlange(6.5,0.5);
  
  //  PipeGen.generatePipe(Control,"bifrostPipeE",2.0,400.0);
    PipeGen.generatePipe(Control,"bifrostPipeBC",3.0,320.0);
     Control.addVariable("bifrostPipeBCXYAngle",0.15);

    //  FGen.clearYOffset(); //should not be cleared!!!!
  //  FGen.generateRectangle(Control,"bifrostFE",396.0, 13.0,13.0);   
  FGen.generateBender(Control,"bifrostBC",859.0,2.9534,2.9534,4.7514,4.7514,
                      bendRadius,bendAngle);

  CGen.setMainRadius(38.122);
  CGen.setFrame(82,82);
  //Chopper gap 4cm
  CGen.generateChopper(Control,"bifrostChopperC",2.0,4.0,2.0);

  
 Control.addVariable("bifrostChopperCFrontFlangeNBolts",0);
 Control.addVariable("bifrostChopperCBackFlangeNBolts",0);

 Control.addVariable("biforstChopperCIPortANBolt",0);
 Control.addVariable("bifrostChopperCIPortBNBolt",0);


  // Singe Blade chopper FOC
  BGen.setThick({0.2});
  BGen.addPhase({95},{60.0});
  BGen.generateBlades(Control,"bifrostFOC2Blade",0.0,20.0,35.0);

  // VACUUM PIPE: from ChoperC with 4m
  PipeGen.setPipe(6.0,0.5);
  /* PipeGen.setWindow(-2.0,0.5);
   */
  PipeGen.setFlange(6.5,0.5);
  
  PipeGen.generatePipe(Control,"bifrostPipeBD",0.0,450.0);
  Control.addVariable("bifrostPipeBDXYAngle",0.1);

  //  FGen.clearYOffset();
  //Bender continues
  FGen.generateBender(Control,"bifrostBD",1600.0,2.9534,2.9534,4.7514,4.7514,
                      bendRadius,bendAngle);
  // FGen.generateTaper(Control,"bifrostFF",477.5, 13.0,4.0, 13.0,4.0);   


// VACUUM PIPE: from ChoperC with 4m
  PipeGen.setPipe(6.0,0.5);
  /*  PipeGen.setWindow(-2.0,0.5);*/
  PipeGen.setFlange(6.5,0.5);
  PipeGen.generatePipe(Control,"bifrostPipeBE",3.0,450.0);
   Control.addVariable("bifrostPipeBEXYAngle",0.1);
   
  // FGen.clearYOffset();
  //Bender continues
   // FGen.generateBender(Control,"bifrostBE",1799.45,2.9534,2.9534,4.7514,4.7514,
   //                   bendRadius,bendAngle);
    FGen.generateBender(Control,"bifrostBE",1799.45,2.9534,2.9534,4.7514,4.7514,
                      bendRadius,bendAngle);
  // FGen.generateTaper(Control,"bifrostFF",477.5, 13.0,4.0, 13.0,4.0);   



  
  Control.addVariable("bifrostAppBInnerWidth",3.1);
  Control.addVariable("bifrostAppBInnerHeight",5.0);
  Control.addVariable("bifrostAppBWidth",12.0);
  Control.addVariable("bifrostAppBHeight",12.0);
  Control.addVariable("bifrostAppBThick",10.0);
  Control.addVariable("bifrostAppBYStep",8.0);
  Control.addVariable("bifrostAppBDefMat","Copper");

  
  // BEAM INSERT:
  Control.addVariable("bifrostBInsertHeight",20.0);
  Control.addVariable("bifrostBInsertWidth",28.0);
  Control.addVariable("bifrostBInsertTopWall",1.0);
  Control.addVariable("bifrostBInsertLowWall",1.0);
  Control.addVariable("bifrostBInsertLeftWall",1.0);
  Control.addVariable("bifrostBInsertRightWall",1.0);
  Control.addVariable("bifrostBInsertWallMat","Stainless304");       

  double InsertLength0=15.0, InsertLength1=16, InsertLength2=14.5;
  double InsertLengthTotal=InsertLength0+InsertLength1*10+InsertLength2*10;
  // NEW BEAM INSERT:
  // Control.addVariable("heimdalCInsertYStep",5.0);   
  { int i=0;
    InsertLength1=175;
   Control.addVariable("bifrostCInsertYStep",2.0);
    Control.addVariable("bifrostCInsertHeight"+std::to_string(i),33.6);
  Control.addVariable("bifrostCInsertWidth"+std::to_string(i),31.5); //31.4 from CAD
  Control.addVariable("bifrostCInsertLength"+std::to_string(i),InsertLength1);
  Control.addVariable("bifrostCInsertMat"+std::to_string(i),"SteelS355");
  }

  {int i=1;
    InsertLength2=145;
  Control.addVariable("bifrostCInsertHeight"+std::to_string(i),38.5);
  Control.addVariable("bifrostCInsertWidth"+std::to_string(i),40.0); //31.4 from CAD
  Control.addVariable("bifrostCInsertLength"+std::to_string(i),InsertLength2);
  Control.addVariable("bifrostCInsertMat"+std::to_string(i),"SteelS355");
  }
  //  Control.addVariable("bifrostCInsertNBox",21);
    Control.addVariable("bifrostCInsertNBox",2);
  Control.addVariable("bifrostCInsertHeight0",33.6);
  Control.addVariable("bifrostCInsertWidth0",31.5); //31.4 from CAD
  /*
  for (int i=1; i<11; i++){  
    Control.addVariable("bifrostCInsertHeight"+std::to_string(i),33.6);
  Control.addVariable("bifrostCInsertWidth"+std::to_string(i),31.5); //31.4 from CAD
  Control.addVariable("bifrostCInsertLength"+std::to_string(i),InsertLength1);
  Control.addVariable("bifrostCInsertMat"+std::to_string(i),"ChipIRSteel");
  }

  for (int i=11; i<21; i++){  
    Control.addVariable("bifrostCInsertHeight"+std::to_string(i),38.5);
  Control.addVariable("bifrostCInsertWidth"+std::to_string(i),40.0); //31.4 from CAD
  Control.addVariable("bifrostCInsertLength"+std::to_string(i),InsertLength2);
  Control.addVariable("bifrostCInsertMat"+std::to_string(i),"ChipIRSteel");
  }
  */
  
  //  Control.addVariable("bifrostCInsertHeight2",38.5);
  // Control.addVariable("bifrostCInsertWidth2",40.0); //39.7 from CAD
  
  //  Control.addVariable("bifrostCInsertLength0",InsertLength0);
  //  Control.addVariable("bifrostCInsertLength2",InsertLength2);

  Control.addVariable("bifrostCInsertMat0","SteelS355");
  Control.addVariable("bifrostCInsertMat1","SteelS355");
  Control.addVariable("bifrostCInsertMat2","SteelS355");
  
  Control.addVariable("bifrostCInsertNWall",1);
  Control.addVariable("bifrostCInsertWallThick",0.3);
  Control.addVariable("bifrostCInsertWallMat","Void");
  Control.addVariable("bifrostCInsertWallThick1",1.0);
  Control.addVariable("bifrostCInsertWallMat1","ChipIRSteel");

  Control.addVariable("bifrostCInsertWallThick2",1.0);
  Control.addVariable("bifrostCInsertWallMat2","Void");
  
    Control.addVariable("bifrostCInsertXYAngle",-0.819);
  Control.addVariable("bifrostCInsertXStep",-1.5);



  InsertLength0=15.0+160; InsertLength1=145;
  
  // NEW BEAM INSERT:
  // Control.addVariable("heimdalCInsertYStep",5.0);   
 
  Control.addVariable("bifrostCPhantomInsertYStep",2.0);
  Control.addVariable("bifrostCPhantomInsertNBox",2);
  Control.addVariable("bifrostCPhantomInsertHeight0",33.6);
  Control.addVariable("bifrostCPhantomInsertWidth0",31.5); //31.4 from CAD
  
    Control.addVariable("bifrostCPhantomInsertHeight0",33.6);
  Control.addVariable("bifrostCPhantomInsertWidth0",31.5); //31.4 from CAD
  Control.addVariable("bifrostCPhantomInsertLength0",InsertLength0);
  Control.addVariable("bifrostCPhantomInsertMat0","ChipIRSteel");
 

  
    Control.addVariable("bifrostCPhantomInsertHeight1",38.5);
  Control.addVariable("bifrostCPhantomInsertWidth1",40.0); //31.4 from CAD
  Control.addVariable("bifrostCPhantomInsertLength1",InsertLength1);
  Control.addVariable("bifrostCPhantomInsertMat1","ChipIRSteel");
 
 
  //  Control.addVariable("bifrostCPhantomInsertHeight2",38.5);
  // Control.addVariable("bifrostCPhantomInsertWidth2",40.0); //39.7 from CAD
  
  //  Control.addVariable("bifrostCPhantomInsertLength2",InsertLength2);

  Control.addVariable("bifrostCPhantomInsertMat0","Void");
  Control.addVariable("bifrostCPhantomInsertMat1","ChipIRSteel");
  Control.addVariable("bifrostCPhantomInsertMat2","ChipIRSteel");
  
  Control.addVariable("bifrostCPhantomInsertNWall",1);
  Control.addVariable("bifrostCPhantomInsertWallThick",0.3);
  Control.addVariable("bifrostCPhantomInsertWallMat","Void");
  Control.addVariable("bifrostCPhantomInsertWallThick1",1.0);
  Control.addVariable("bifrostCPhantomInsertWallMat1","ChipIRSteel");

  Control.addVariable("bifrostCPhantomInsertWallThick2",1.0);
  Control.addVariable("bifrostCPhantomInsertWallMat2","Void");
  
    Control.addVariable("bifrostCPhantomInsertXYAngle",-0.819);
  Control.addVariable("bifrostCPhantomInsertXStep",-1.5);


  
  // VACUUM PIPE: in bunker wall
  PipeGen.setPipe(6.0,0.5);
  //  PipeGen.setWindow(-2.0,0.5);
  PipeGen.setFlange(6.5,0.5);
  PipeGen.generatePipe(Control,"bifrostPipeWall",1.0,348.0);


     FGen.setYOffset(0);

 //TaperedPipe (vacuum housing in the bunker wall feedthrough
 // FGen.setLayer(1,6.0,"Stainless304");
  FGen.setLayer(1,0.2,"Void");
  FGen.setLayer(2,0.2,"Void");
  FGen.generateTaper(Control,"bifrostTPipeWall",180.0,18.5,18.5, 20.5,20.5);  
  Control.addVariable("bifrostTPipeWallLayerMat0","Stainless304");
  // Control.addVariable("bifrostTPipeWallYStep",1.0);  
  //  FGen.setLayer(1,10.0,"Stainless304");
  FGen.setLayer(1,0.2,"Void");
  FGen.generateTaper(Control,"bifrostTPipeWall1",58.0+60,27.5,27.5, 35.9,35.9);  
  Control.addVariable("bifrostTPipeWall1LayerMat0","Stainless304");

  //    FGen.setLayer(1,6.5,"Stainless304");
  //  FGen.setLayer(2,0.2,"Void");
  FGen.generateTaper(Control,"bifrostTPipeWall2",30,27.5,27.5, 35.9,35.9);  
  Control.addVariable("bifrostTPipeWall2LayerMat0","Stainless304");

  FGen.setLayer(1,4.0,"Stainless304");
  FGen.setLayer(2,0.3,"Void");
  FGen.generateTaper(Control,"bifrostTPipeWall3",40.0,10.5,10.5, 12.5,12.5);  

  
  //Definitions of defocusing sections
    double  length3 = 24.9288,
  //      double  length3 = 24.9288+0.14,
    Linx3 = 3.70973, Loutx3 = 28.6385, majoraxis_x = 28.6385, smallaxis_x3=0.03,
    Liny3 = 4.42383,Louty3 = 29.3526, majoraxis_y = 29.3527, smallaxis_y3=0.045;
  
  // VACUUM PIPE: leaving bunker
  
 

  

  // Guide at 9.2m (part of a 4->8cm in 20m)
 double majoroffset_x = (length3+Linx3+Loutx3)/2 - Loutx3 - length3;
 double hIn =
   2*sqrt(1-majoroffset_x*majoroffset_x/majoraxis_x/majoraxis_x)*smallaxis_x3;
  double majoroffset_y=(length3+Liny3+Louty3)/2 - Louty3 - length3;
  double vIn =
    2*sqrt(1-majoroffset_y*majoroffset_y/majoraxis_y/majoraxis_y)*smallaxis_y3;

  majoroffset_x = (length3+Linx3+Loutx3)/2 - Loutx3 - length3+0.01*InsertLengthTotal;
  double hOut=
    2*sqrt(1-majoroffset_x*majoroffset_x/majoraxis_x/majoraxis_x)*smallaxis_x3;
   majoroffset_y=(length3+Liny3+Louty3)/2 - Louty3 - length3 + 0.01*InsertLengthTotal;
  double vOut =
    2*sqrt(1-majoroffset_y*majoroffset_y/majoraxis_y/majoraxis_y)*smallaxis_y3;

  
  //Defocusing section starts in the bunker wall feedthrough
  // Guide in wall
  FGen.setLayer(1,1.0,"Copper");
  FGen.setLayer(2,0.3,"Void");
  FGen.setLayer(3,0.1,"Void");
  // FGen.generateTaper(Control,"bifrostFWall",2492.88,2.9534,6.0, 4.7514,9.0);
  FGen.generateTaper(Control,"bifrostFWall",300-2, 100.0*hIn,100.0*hOut, 100.0*vIn,100.0*vOut);
  FGen.setLayer(2,1.9,"Void");
  FGen.setLayer(3,0.1,"Void");
FGen.generateTaper(Control,"bifrostFWall1",300, 100.0*hIn,100.0*hOut, 100.0*vIn,100.0*vOut);
  FGen.setLayer(1,2.5,"Copper");
  FGen.setLayer(2,0.4,"Void");
  // FGen.setLayer(3,0.1,"Void");
  FGen.generateTaper(Control,"bifrostFWall2",InsertLengthTotal, 100.0*hIn,100.0*hOut, 100.0*vIn,100.0*vOut);   
 
  FGen.setLayer(2,0.0,"Void");
  FGen.setLayer(3,0.0,"Void");
 

  /*Parameters of the defocusing section starting right before the bunker wall*/
	/*  double  length3 = 24.9288,
    Linx3 = 3.70973, Loutx3 = 28.6385, majoraxis_x = 28.6385, smallaxis_x3=0.03,
    Liny3 = 4.42383,Louty3 = 29.3526, majoraxis_y = 29.3527, smallaxis_y3=0.045;
	*/
  // VACUUM PIPE: leaving bunker

  /*
  PipeGen.setWindow(-2.0,0.5);
  */
  //  PipeGen.generatePipe(Control,"bifrostPipeOutA",4.0,600);

  


  PipeGen.setMat("Aluminium");
  PipeGen.setWindow(-2.0,0.2); //2.5 mm window = 2*0.5 windows + 2*0.5 beam monitor
       PipeGen.setPipe(7.0,0.5);
  /*
  PipeGen.setWindow(-2.0,0.5);
  */ PipeGen.setFlange(7.8,1.0);
       //  PipeGen.generatePipe(Control,"bifrostPipeOutA",2.0,600);
         PipeGen.generatePipe(Control,"bifrostPipeOutA",2.0,100);
  Control.addVariable("bifrostPipeOutAYStep",4.0);
  Control.addVariable("bifrostPipeOutACladdingThick",0.3);
  //  Control.addVariable("bifrostPipeOutACladdingMat","Mirrobor");
    Control.addVariable("bifrostPipeOutACladdingMat","Air");
  Control.addVariable("bifrostPipeOutAWindowFrontMat","Aluminium");
  Control.addVariable("bifrostPipeOutAWindowActive",1);

  
  // Guide at 9.2m (part of a 4->8cm in 20m)
    majoroffset_x = (length3+Linx3+Loutx3)/2 - Loutx3 - length3 +3.6 + 0.02 +0.02;
    hIn =  2*sqrt(1-majoroffset_x*majoroffset_x/majoraxis_x/majoraxis_x)*smallaxis_x3;
    majoroffset_y=(length3+Liny3+Louty3)/2 - Louty3 - length3 +3.6 +0.02+0.02;
    vIn = 2*sqrt(1-majoroffset_y*majoroffset_y/majoraxis_y/majoraxis_y)*smallaxis_y3;
   majoroffset_x = (length3+Linx3+Loutx3)/2 - Loutx3 - length3 +3.6+0.02+0.02+5.96;
    hOut= 2*sqrt(1-majoroffset_x*majoroffset_x/majoraxis_x/majoraxis_x)*smallaxis_x3;
   majoroffset_y=(length3+Liny3+Louty3)/2 - Louty3 - length3 +3.6+0.02 + 0.02 +5.96;
    vOut = 2*sqrt(1-majoroffset_y*majoroffset_y/majoraxis_y/majoraxis_y)*smallaxis_y3;

  FGen.setLayer(1,0.5,"BorkronNBK7");
  FGen.setLayer(2,0.1,"Void");

  //  FGen.clearYOffset();
  FGen.generateTaper(Control,"bifrostFOutA",598.0,100.0*hIn,100.0*hOut, 100.0*vIn,100.0*vOut);   

  Control.addVariable("bifrostFOutAYStep",5.0);
  // Second vacuum pipe out of bunker [before chopper pit]
  PipeGen.setPipe(7.0,0.5);
  //  PipeGen.setWindow(-2.0,0.5);
  // PipeGen.setFlange(-4.0,1.0);
   PipeGen.setFlange(7.5,0.5);
   // PipeGen.generatePipe(Control,"bifrostPipeOutB",2.0,1095.8);
     //  PipeGen.generatePipe(Control,"bifrostPipeOutB",2.0,600);
  PipeGen.generatePipe(Control,
		       "bifrostPipeOutB",2.0,100*(length3-3.59)-150.0);
  Control.addVariable("bifrostPipeOutBWindowActive",0);

   // Next section of defocusing guide. Pipe length 600 cm + pipe offset 2cm + guide offset 2cm
   majoroffset_x = (length3+Linx3+Loutx3)/2 - Loutx3 - length3 +3.66+6.0+0.02+0.02; 
   hIn =  2*sqrt(1-majoroffset_x*majoroffset_x/majoraxis_x/majoraxis_x)*smallaxis_x3;
   majoroffset_y=(length3+Liny3+Louty3)/2 - Louty3 - length3 +3.66+6.04;
   vIn = 2*sqrt(1-majoroffset_y*majoroffset_y/majoraxis_y/majoraxis_y)*smallaxis_y3;
   majoroffset_x = (length3+Linx3+Loutx3)/2 - Loutx3 - length3 +3.66+5.96+6.04;
   hOut =  2*sqrt(1-majoroffset_x*majoroffset_x/majoraxis_x/majoraxis_x)*smallaxis_x3;
   majoroffset_y=(length3+Liny3+Louty3)/2 - Louty3 - length3 +3.66+5.96+6.04;
   vOut = 2*sqrt(1-majoroffset_y*majoroffset_y/majoraxis_y/majoraxis_y)*smallaxis_y3;

   //  FGen.clearYOffset();
  FGen.generateTaper(Control,"bifrostFOutB", 598.0,100.0*hIn,100.0*hOut, 100.0*vIn,100.0*vOut);
  Control.addVariable("bifrostFOutBYStep",0.5);
  /*
  // Another 6 m pipe
  PipeGen.setPipe(7.0,0.5);
  PipeGen.setWindow(-2.0,0.5);
  PipeGen.setFlange(-4.0,1.0);
  PipeGen.generatePipe(Control,"bifrostPipeOutC",4.0,600.0);

   majoroffset_x = (length3+Linx3+Loutx3)/2 - Loutx3 - length3 +3.75+6.02+6.04;
   hIn =  2*sqrt(1-majoroffset_x*majoroffset_x/majoraxis_x/majoraxis_x)*smallaxis_x3;
   majoroffset_y=(length3+Liny3+Louty3)/2 - Louty3 - length3 +3.75+6.02+6.04;
   vIn = 2*sqrt(1-majoroffset_y*majoroffset_y/majoraxis_y/majoraxis_y)*smallaxis_y3;
   majoroffset_x = (length3+Linx3+Loutx3)/2 - Loutx3 - length3 +3.75+5.96+6.04+6.02;
   hOut =  2*sqrt(1-majoroffset_x*majoroffset_x/majoraxis_x/majoraxis_x)*smallaxis_x3;
   majoroffset_y=(length3+Liny3+Louty3)/2 - Louty3 - length3 +3.75+5.96+6.04 + 6.02;
   vOut = 2*sqrt(1-majoroffset_y*majoroffset_y/majoraxis_y/majoraxis_y)*smallaxis_y3;

  // Guide at 9.2m (part of a 4->8cm in 20m)
  FGen.clearYOffset();
  FGen.generateTaper(Control,"bifrostFOutC",596.0,100.0*hIn,100.0*hOut, 100.0*vIn,100.0*vOut);   
  */
  
  // Another vacuum pipe out of bunker [before chopper pit]
  PipeGen.setPipe(7.0,0.5);
  //0.75mm window
    PipeGen.setWindow(-2.0,0.05);
  // PipeGen.setFlange(-4.0,1.0);
   PipeGen.setFlange(7.8,1.0);
   //  PipeGen.generatePipe(Control,"bifrostPipeOutC",2.0,100*(length3 -(3.5+6.02+6.02 + 0.02))-3.);
     PipeGen.generatePipe(Control,"bifrostPipeOutC",2.0,50.);
  Control.addVariable("bifrostPipeOutCWindowBackMat","Aluminium");
  Control.addVariable("bifrostPipeOutCWindowActive",2);
  Control.addVariable("bifrostPipeOutCCladdingThick",0.3);
  //  Control.addVariable("bifrostPipeOutCCladdingMat","Mirrobor");
    Control.addVariable("bifrostPipeOutCCladdingMat","Air");


  
   majoroffset_x = (length3+Linx3+Loutx3)/2 - Loutx3 -length3 +3.75+6.04 + 6.02;
   hIn =  2*sqrt(1-majoroffset_x*majoroffset_x/majoraxis_x/majoraxis_x)*smallaxis_x3;
   majoroffset_y=(length3+Liny3+Louty3)/2 - Louty3 -length3 +3.75+6.04+6.02;
   vIn = 2*sqrt(1-majoroffset_y*majoroffset_y/majoraxis_y/majoraxis_y)*smallaxis_y3;
   majoroffset_x = (length3+Linx3+Loutx3)/2 - Loutx3 ;
   hOut =  2*sqrt(1-majoroffset_x*majoroffset_x/majoraxis_x/majoraxis_x)*smallaxis_x3;
   majoroffset_y=(length3+Liny3+Louty3)/2 - Louty3 ;
   vOut = 2*sqrt(1-majoroffset_y*majoroffset_y/majoraxis_y/majoraxis_y)*smallaxis_y3;

  // Guide at 9.2m (part of a 4->8cm in 20m)
   // FGen.clearYOffset();
  FGen.generateTaper(Control,"bifrostFOutC",100*(length3 -(3.5+6.02 + 6.02 ))-0.15,100.0*hIn,100.0*hOut, 100.0*vIn,100.0*vOut);   
  Control.addVariable("bifrostFOutCYStep",0.5);

  //  PipeGen.setMat("Stainless304");



  PipeGen.setMat("Aluminium");
       PipeGen.setWindow(-2.0,0.05);
       PipeGen.setPipe(7.0,0.5);
  /*
  PipeGen.setWindow(-2.0,0.5);
  */ PipeGen.setFlange(7.8,1.0);
       //  PipeGen.generatePipe(Control,"bifrostPipeR0",2.0,600);
         PipeGen.generatePipe(Control,"bifrostPipeR0",2.0,100);
  Control.addVariable("bifrostPipeR0YStep",4.0);
  Control.addVariable("bifrostPipeR0CladdingThick",0.3);
  //  Control.addVariable("bifrostPipeR0CladdingMat","Mirrobor");
    Control.addVariable("bifrostPipeR0CladdingMat","Air");
  Control.addVariable("bifrostPipeR0WindowFrontMat","Aluminium");
  Control.addVariable("bifrostPipeR0WindowActive",1);
  Control.addVariable("bifrostPipeR0YStep",11.2);

  /*
  PipeGen.setWindow(0.0,0.0);
    PipeGen.setFlange(5.0,0.5); //inner flange diameter, flange width
     PipeGen.setPipe(4.5,0.5); //diameter thickness
    PipeGen.generatePipe(Control,"bifrostPipeR0",12,30.5);//offset,length
  */
  PipeGen.setPipe(7.0,0.5);
  //  PipeGen.setWindow(-2.0,0.5);
  // PipeGen.setFlange(-4.0,1.0);
   PipeGen.setFlange(7.5,0.5);
   // PipeGen.generatePipe(Control,"bifrostPipeOutB",2.0,1095.8);
     //  PipeGen.generatePipe(Control,"bifrostPipeOutB",2.0,600);
  PipeGen.generatePipe(Control,
		       "bifrostPipeR1",0.0,2000);
  Control.addVariable("bifrostPipeR1WindowActive",0);

  
  for(int i=0; i<5; i++){
  const std::string strNum(StrFunc::makeString(i));
  //      PipeGen.generatePipe(Control,"bifrostPipeR"+strNum,0.0,2000.0);
 FGen.generateTaper(Control,"bifrostFOutR"+strNum,
		    568,100.0*hOut,100.0*hOut, 100.0*vOut,100.0*vOut);   
Control.addVariable("bifrostFOutR"+strNum+"YStep",0.2);
 // Control.addVariable("bifrostFOutCYStep",0.5);
      
  // FGen.generateRectangle(Control,"bifrostFOutR"+strNum,1996.0,100.0*hOut,100.0*vOut);   
  }
Control.addVariable("bifrostFOutR0YStep",12.0);


  
  // Cut in the bunker wall
  FGen.setLayer(1,1.0,"Void");
  FGen.setLayer(2,0.0,"Void");
  FGen.setLayer(3,0.0,"Void");
  FGen.generateTaperGeneral(Control,"bifrostWallCut",30.0,
			    33,33,
  			    33,33,
			       131.0,131.0,
			       33,33);
  Control.addVariable("bifrostWallCutYStep",320);//offset along beam
  			   

  // Shield: leaving bunker
  //Gap between bunker wall and lateral shielding
  SGen.clearLayers();
  SGen.addWallMat(1,"Void");
  SGen.addRoofMat(1,"Void");
  SGen.addFloorMat(1,"Void");
  //manually select length (1st arg of shield generator) to choose gap size 
  SGen.generateShield(Control,"bifrostShieldGap",2.5, 90.0, 90.0, 160.0, 1,2);
  Control.addVariable("bifrostShieldGapVoidLeft",40.0);
    Control.addVariable("bifrostShieldGapVoidRight",40.0);
      Control.addVariable("bifrostShieldGapVoidHeight",40.0);
        Control.addVariable("bifrostShieldGapVoidDepth",40.0);
      Control.addVariable("DefaultTileThick",-1);
   
	//linear shielding

  /*

  SGen.clearLayers();
  SGen.addWallMat(1,"ChipIRSteel");
  SGen.addRoofMat(1,"ChipIRSteel");
  SGen.addFloorMat(1,"Void");
 
  //manually select length (1st arg of shield generator) to choose gap size 
  SGen.generateShield(Control,"bifrostInnerShieldGap1",40.0, 51.95, 41.95, 21.95, 1,4);
  Control.addVariable("bifrostInnerShieldGap1VoidLeft",32.0);
    Control.addVariable("bifrostInnerShieldGap1VoidRight",32.0);
      Control.addVariable("bifrostInnerShieldGap1VoidHeight",22.0);
        Control.addVariable("bifrostInnerShieldGap1VoidDepth",21.5);
       Control.addVariable("bifrostInnerShieldGap1Spacing",0.5);
	//	Control.addVariable("bifrostShieldGap1YStep",-28.0);
		Control.addVariable("bifrostInnerShieldGap1YStep",-29.5);

  SGen.generateShield(Control,"bifrostInnerShieldGap2",70.0, 51.95, 41.95,21.95, 1,4);
  Control.addVariable("bifrostInnerShieldGap2VoidLeft",32.0);
    Control.addVariable("bifrostInnerShieldGap2VoidRight",32.0);
      Control.addVariable("bifrostInnerShieldGap2VoidHeight",22.0);
        Control.addVariable("bifrostInnerShieldGap2VoidDepth",21.5);
    Control.addVariable("bifrostInnerShieldGap2Spacing",0.5);

*/

      
      //                Control.addVariable("bifrostD03FirstSupportSideShield",1);
           Control.addVariable("bifrostD03FirstSupportYStep",3);
           Control.addVariable("bifrostD03FirstSupportZStep",-113.7);
           Control.addVariable("bifrostD03FirstSupportLength1Edge",120);
           Control.addVariable("bifrostD03FirstSupportLength1Mid",135);
           Control.addVariable("bifrostD03FirstSupportLength1Low",60);
           Control.addVariable("bifrostD03FirstSupportBuildTube",0);
	             Control.addVariable("bifrostD03FirstSupportDefMatF","MagnadenseHC");
		     //            Control.addVariable("bifrostD03FirstSupportDefMatF","LimestoneConcrete");
           Control.addVariable("bifrostD03FirstSupportDefMatB","MagnadenseHC");

	   Control.addVariable("D03Support0DefMatF","MagnadenseHC");
	   Control.addVariable("D03Support8DefMatB","MagnadenseHC");

 	   Control.addVariable("bifrostD03ShutterSupport1DefMatF","MagnadenseHC");
 	   Control.addVariable("bifrostD03ShutterSupport1DefMatB","MagnadenseHC");
   Control.addVariable("bifrostD03ShutterSupport2DefMatF","MagnadenseHC");
   Control.addVariable("bifrostD03ShutterSupport2DefMatB","MagnadenseHC");
   Control.addVariable("bifrostD03LastSupportDefMatF","MagnadenseHC");

           //Control.addVariable("bifrostD03FirstSupportDefMatB","MagnadenseHC");

	   //	   Control.addVariable("bifrostD03ShutterSupport1Width1B",200);
	   //	   Control.addVariable("bifrostD03ShutterSupport2Width1F",200);
	   // in reality W1F=194.5 and W3F=94.5
	   //	   Control.addVariable("bifrostD03BaseShutter1Width1Back",194.);
	   //	   Control.addVariable("bifrostD03BaseShutter0Width1Back",194.);
	   Control.addVariable("bifrostD03BaseShutter0Width3Back",94.);
  	   Control.addVariable("bifrostD03BaseShutter0Width3BackSteel",114.);

	   Control.addVariable("bifrostD03BaseShutter0Width3Front",90);
	   Control.addVariable("bifrostD03BaseShutter0Length2Edge",47.5);
	   Control.addVariable("bifrostD03BaseShutter0Width2",92);

	   Control.addVariable("bifrostD03BaseShutter0Length1Edge",168.5);
	   Control.addVariable("bifrostD03BaseShutter0Length1Mid",179);
	   //205 cm
	   Control.addVariable("bifrostD03BaseShutter1Length1Edge",157.5);
	   Control.addVariable("bifrostD03BaseShutter1Length1Mid",168);
	   Control.addVariable("bifrostD03BaseShutter1Width2",92);
  	   Control.addVariable("bifrostD03BaseShutter1Width3BackSteel",114.);
	   Control.addVariable("bifrostD03BaseShutter1Width3Back",94.);
	   Control.addVariable("bifrostD03BaseShutter1Length2Edge",47.5);
	   // 325 cm length
	   Control.addVariable("bifrostD03BaseShutter2Length1Edge",135);
	   Control.addVariable("bifrostD03BaseShutter2Length1Mid",145);
	   //  Control.addVariable("bifrostD03BaseShutter2Width2",92);
	   // 	   Control.addVariable("bifrostD03BaseShutter2Width3BackSteel",114.);
	   //	   Control.addVariable("bifrostD03BaseShutter2Width3Back",94.);
	   Control.addVariable("bifrostD03BaseShutter2Length2Edge",120+70);


	   Control.addVariable("bifrostD03Base3Length2Edge",177.5);
	   Control.addVariable("bifrostD03Base3Length2Mid",162.5);
	   
           //125 cm length
	   Control.addVariable("bifrostD03BaseShutter3Length1Edge",5);
	   Control.addVariable("bifrostD03BaseShutter3Length1Mid",15);
	   //  Control.addVariable("bifrostD03BaseShutter2Width2",92);
	   // 	   Control.addVariable("bifrostD03BaseShutter2Width3BackSteel",114.);
	   //	   Control.addVariable("bifrostD03BaseShutter2Width3Back",94.);
	   Control.addVariable("bifrostD03BaseShutter3Length2Edge",120);

	   
	   Control.addVariable("bifrostD03Base3Length2Edge",177.5);
	   Control.addVariable("bifrostD03Base3Length2Mid",162.5);
	   
	   

	   Control.addVariable("bifrostD03BaseSteelShutter0Width1Front",70.);
	   Control.addVariable("bifrostD03BaseSteelShutter0Width1Back",70.);
	   Control.addVariable("bifrostD03BaseSteelShutter0Width3Front",60.);
	   Control.addVariable("bifrostD03BaseSteelShutter0Width3Back",60.);
	   Control.addVariable("bifrostD03BaseSteelShutter0Length2Edge",20);
	   Control.addVariable("bifrostD03BaseSteelShutter0Length1Edge",56);
	   Control.addVariable("bifrostD03BaseSteelShutter0Length1Mid",56.);
	   Control.addVariable("bifrostD03BaseSteelShutter0Gap",0);
	   //	   Control.addVariable("bifrostD03BaseSteelStart0GapV",0);
	   Control.addVariable("bifrostD03BaseSteelShutter0NLayers",2);
	   Control.addVariable("bifrostD03BaseSteelShutter0HeightUp",90.6);
           Control.addVariable("bifrostD03BaseSteelShutter0DefMatF","SteelS355");
           Control.addVariable("bifrostD03BaseSteelShutter0DefMatB","SteelS355");
	   Control.addVariable("bifrostD03BaseSteelShutter0InnerB4CLayerThick",1.5);
	   Control.addVariable("bifrostD03BaseSteelShutter0B4CTileMat","B4CConcrete");



	   
	   Control.addVariable("bifrostD03BaseSteelShutter3Width1Front",90.);
	   Control.addVariable("bifrostD03BaseSteelShutter3Width1Back",90.);
	   Control.addVariable("bifrostD03BaseSteelShutter3Width3Front",70.);
	   Control.addVariable("bifrostD03BaseSteelShutter3Width3Back",70.);
	   Control.addVariable("bifrostD03BaseSteelShutter3Length2Edge",78);
	   Control.addVariable("bifrostD03BaseSteelShutter3Length1Edge",78);
	   Control.addVariable("bifrostD03BaseSteelShutter3Length1Mid",78.);
	   Control.addVariable("bifrostD03BaseSteelShutter3Gap",0);
	   //	   Control.addVariable("bifrostD03BaseSteelStart0GapV",0);
	   Control.addVariable("bifrostD03BaseSteelShutter3NLayers",3);
	   Control.addVariable("bifrostD03BaseSteelShutter3HeightUp",100.);
           Control.addVariable("bifrostD03BaseSteelShutter3DefMatF","SteelS355");
           Control.addVariable("bifrostD03BaseSteelShutter3DefMatB","SteelS355");



	   Control.addVariable("bifrostD03BaseSteelShutter4Width1Front",90.);
	   Control.addVariable("bifrostD03BaseSteelShutter4Width1Back",90.);
	   Control.addVariable("bifrostD03BaseSteelShutter4Width3Front",70.);
	   Control.addVariable("bifrostD03BaseSteelShutter4Width3Back",70.);
	   Control.addVariable("bifrostD03BaseSteelShutter4Length2Edge",105);
	   Control.addVariable("bifrostD03BaseSteelShutter4Length1Edge",56);
	   Control.addVariable("bifrostD03BaseSteelShutter4Length1Mid",56.);
	   Control.addVariable("bifrostD03BaseSteelShutter4Gap",0);
	   //	   Control.addVariable("bifrostD03BaseSteelStart0GapV",0);
	   Control.addVariable("bifrostD03BaseSteelShutter4NLayers",3);
	   Control.addVariable("bifrostD03BaseSteelShutter4HeightUp",100.);
           Control.addVariable("bifrostD03BaseSteelShutter4DefMatF","SteelS355");
           Control.addVariable("bifrostD03BaseSteelShutter4DefMatB","SteelS355");

	   
	 //  Control.addVariable("bifrostD03BaseSteelShutter3InnerB4CLayerThick",1.5);
	 //  Control.addVariable("bifrostD03BaseSteelShutter3B4CTileMat","B4CConcrete");

	   Control.addVariable("bifrostD03BaseSteelShutter2Width1Front",70.);
	   Control.addVariable("bifrostD03BaseSteelShutter2Width1Back",70.);
	   Control.addVariable("bifrostD03BaseSteelShutter2Width3Front",60.);
	   Control.addVariable("bifrostD03BaseSteelShutter2Width3Back",60.);
	   Control.addVariable("bifrostD03BaseSteelShutter2Length2Edge",26.5);
	   Control.addVariable("bifrostD03BaseSteelShutter2Length1Edge",50);
	   Control.addVariable("bifrostD03BaseSteelShutter2Length1Mid",50.);
	   Control.addVariable("bifrostD03BaseSteelShutter2Gap",0);
	   //	   Control.addVariable("bifrostD03BaseSteelStart0GapV",0);
	   Control.addVariable("bifrostD03BaseSteelShutter2NLayers",2);
	   Control.addVariable("bifrostD03BaseSteelShutter2HeightUp",90);
           Control.addVariable("bifrostD03BaseSteelShutter2DefMatF","SteelS355");
           Control.addVariable("bifrostD03BaseSteelShutter2DefMatB","SteelS355");
	   Control.addVariable("bifrostD03BaseSteelShutter2InnerB4CLayerThick",1.5);
	   Control.addVariable("bifrostD03BaseSteelShutter2B4CTileMat","B4CConcrete");


	   
	   
	   Control.addVariable("bifrostD03BaseSteelShutter1Width1Front",94.);
	   Control.addVariable("bifrostD03BaseSteelShutter1Width1Back",94.);
	   Control.addVariable("bifrostD03BaseSteelShutter1InnerB4CLayerThick",1.5);
	   //	   Control.addVariable("bifrostD03BaseSteelShutter2Width2Front",94.);
	   //	   Control.addVariable("bifrostD03BaseSteelShutter2Width2Back",94.);
	   Control.addVariable("bifrostD03BaseSteelShutter1Width3Front",54.);
	   Control.addVariable("bifrostD03BaseSteelShutter1Width3Back",54.);
	   Control.addVariable("bifrostD03BaseSteelShutter1Length2Edge",20);
	   Control.addVariable("bifrostD03BaseSteelShutter1Length1Edge",43.5);
	   Control.addVariable("bifrostD03BaseSteelShutter1Length1Mid",52.);
	   Control.addVariable("bifrostD03BaseSteelShutter1Gap",0);
	   Control.addVariable("bifrostD03BaseSteelShutter1HeightUp",90.6);
           Control.addVariable("bifrostD03BaseSteelShutter1DefMatF","SteelS355");
           Control.addVariable("bifrostD03BaseSteelShutter1DefMatB","SteelS355");



	   /*	   Control.addVariable("bifrostD03BaseSteelShutter5Width1Front",114.);
	   Control.addVariable("bifrostD03BaseSteelShutter5Width1Back",114.);
	   Control.addVariable("bifrostD03BaseSteelShutter5InnerB4CLayerThick",0.0);
	   //	   Control.addVariable("bifrostD03BaseSteelShutter2Width2Front",94.);
	   //	   Control.addVariable("bifrostD03BaseSteelShutter2Width2Back",94.);
	   Control.addVariable("bifrostD03BaseSteelShutter5Width3Front",114.);
	   Control.addVariable("bifrostD03BaseSteelShutter5Width3Back",114.);
	   Control.addVariable("bifrostD03BaseSteelShutter5Length2Edge",20);
	   Control.addVariable("bifrostD03BaseSteelShutter5Length1Edge",43.5);
	   Control.addVariable("bifrostD03BaseSteelShutter5Length1Mid",52.);
	   Control.addVariable("bifrostD03BaseSteelShutter5Gap",0);
	   Control.addVariable("bifrostD03BaseSteelShutter5HeightUp",100.6);
           Control.addVariable("bifrostD03BaseSteelShutter5DefMatF","SteelS355");
           Control.addVariable("bifrostD03BaseSteelShutter5DefMatB","SteelS355");
	   */
	   
	   
	   //	   Control.addVariable("bifrostD03UpperShutterStartWidth1Front",194);
	   Control.addVariable("bifrostD03UpperShutterStartLength1Edge",15);
	   Control.addVariable("bifrostD03UpperShutterStartLength1Mid",30);
	   //   Control.addVariable("bifrostD03UpperShutterStartHeightUpFront",148);
	      Control.addVariable("bifrostD03UpperShutterStartHeightUpFront",123);
	   Control.addVariable("bifrostD03UpperShutterStartNSeg",2);
	   //	   Control.addVariable("bifrostD03UpperShutterStartWidth2",92);
	   //	   Control.addVariable("bifrostD03UpperShutterStartWidth3Front",94);
	   //std length 57.5 and 42.5, total is 1m.
           Control.addVariable("bifrostD03UpperShutterEndLength2Edge",72.5-15);
	   Control.addVariable("bifrostD03UpperShutterEndLength2Mid",72.5-30);
	   //	   Control.addVariable("bifrostD03UpperShutterEndLength1Edge",2700);
	   // Control.addVariable("bifrostD03UpperShutterEndLength1Mid",2700+15);

	   //	   Control.addVariable("bifrostD03UpperShutterEndHeightUpBack",148);
	   	   Control.addVariable("bifrostD03UpperShutterEndHeightUpBack",123);
	   Control.addVariable("bifrostD03UpperShutterEndNSeg",3);
	             Control.addVariable("bifrostD03UpperShutterEndXYAngle",180);
   	             Control.addVariable("bifrostD03UpperShutterEndYStep",100);
     //	             Control.addVariable("bifrostD03UpperShutterEndYStep",2700+72.5-15);

	  
	   Control.addVariable("bifrostD03UpperShutter0HeightLowBack",98);
	   Control.addVariable("bifrostD03UpperShutter0NSeg",2);
	   Control.addVariable("bifrostD03UpperShutter0HeightMid",95);	   
	   //   Control.addVariable("bifrostD03UpperShutter0HeightUpBack",148);
	   //  Control.addVariable("bifrostD03UpperShutter0HeightUpFront",148);
	    Control.addVariable("bifrostD03UpperShutter0HeightUpBack",123);
	    Control.addVariable("bifrostD03UpperShutter0HeightUpFront",123);
	   Control.addVariable("bifrostD03UpperShutter0Length1Edge",17);
	   Control.addVariable("bifrostD03UpperShutter0Length2Edge",46.5);
	   Control.addVariable("bifrostD03UpperShutter0Length1Mid",26.5);

	   //	   Control.addVariable("bifrostD03UpperShutterStartWidth3Front",78);
	   // Control.addVariable("bifrostD03UpperShutterEndWidth3Back",78);
	   
	   Control.addVariable("bifrostD03UpperShutter1Width2",100);
	   Control.addVariable("bifrostD03UpperShutter0Width2",100);
	   //Control.addVariable("bifrostD03UpperShutter1Width3Front",78);
	   // Control.addVariable("bifrostD03UpperShutter0Width3Front",78);
	   Control.addVariable("bifrostD03UpperShutter1HeightLowFront",51.5);
	   Control.addVariable("bifrostD03UpperShutter0HeightLowFront",51.5);
	   // Control.addVariable("bifrostD03UpperShutter0Width1Back",194);
	   // Control.addVariable("bifrostD03UpperShutter1Width1Back",194);
	    Control.addVariable("bifrostD03UpperShutter1Width3Back",112.5);
	   Control.addVariable("bifrostD03UpperShutter0Width3Back",112.5);

	   
	   Control.addVariable("bifrostD03UpperShutter1HeightLowBack",98);
	   Control.addVariable("bifrostD03UpperShutter1NSeg",2);
	   Control.addVariable("bifrostD03UpperShutter1HeightMid",95);
	   //	   Control.addVariable("bifrostD03UpperShutter1HeightUpFront",148);
	   //	   Control.addVariable("bifrostD03UpperShutter1HeightUpBack",148);
	   Control.addVariable("bifrostD03UpperShutter1HeightUpFront",123);
	   Control.addVariable("bifrostD03UpperShutter1HeightUpBack",123);
	   Control.addVariable("bifrostD03UpperShutter1Length2Edge",46.5);
	   Control.addVariable("bifrostD03UpperShutter1Length1Edge",17);
	   Control.addVariable("bifrostD03UpperShutter1Length1Mid",26.5);

	   
	   //	   Control.addVariable("bifrostD03UpperShutter0HeightUpBack",148);
	   Control.addVariable("bifrostD03SteelVoidLength",1000);
	   Control.addVariable("bifrostD03SteelVoidVoidRight",24);
	   Control.addVariable("bifrostD03SteelVoidVoidLeft",24);
	   Control.addVariable("bifrostD03SteelVoidVoidHeight",36.5);
	   Control.addVariable("bifrostD03SteelVoidVoidDepth",8.5);
	   Control.addVariable("bifrostD03SteelVoidDepth",8.5);
	   Control.addVariable("bifrostD03SteelVoidNWallLayersR",2);
	   Control.addVariable("bifrostD03SteelVoidNWallLayersL",2);
   	   Control.addVariable("bifrostD03SteelVoidInnerB4LayerThick",0);
	   Control.addVariable("bifrostD03SteelVoidNRoofLayers",2);

	   Control.addVariable("bifrostLeadUpperPlateLeft",27.5);
	   Control.addVariable("bifrostLeadUpperPlateNWallLayersR",2);
	   Control.addVariable("bifrostLeadUpperPlateNWallLayersL",2);
	   Control.addVariable("bifrostLeadUpperPlateNFloorLayers",1);
	   Control.addVariable("bifrostLeadUpperPlateNRoofLayers",2);
	   Control.addVariable("bifrostLeadUpperPlateSpacing",0);
	   Control.addVariable("bifrostLeadUpperPlateLength",60);
	   Control.addVariable("bifrostLeadUpperPlateRight",27.5);
	   Control.addVariable("bifrostLeadUpperPlateVoidLeft",20.5);
	   Control.addVariable("bifrostLeadUpperPlateVoidRight",20.5);
	   Control.addVariable("bifrostLeadUpperPlateHeight",67); //thickness is 67-60=7cm
	   Control.addVariable("bifrostLeadUpperPlateVoidHeight",63.5);
	   Control.addVariable("bifrostLeadUpperPlateDepth",-60);
	   Control.addVariable("bifrostLeadUpperPlateVoidDepth",-60);
	   //	   Control.addVariable("bifrostLeadUpperPlateDefMat","SteelS355");
	   //	   Control.addVariable("bifrostLeadUpperPlateVoidMat","SteelS355");
	   Control.addVariable("bifrostLeadUpperPlateDefMat","Lead51121");
	   Control.addVariable("bifrostLeadUpperPlateVoidMat","Lead51121");
	   Control.addVariable("bifrostLeadUpperPlateNLayers",2);




	   Control.addVariable("bifrostB4CLowerPlateLeft",36);
	   Control.addVariable("bifrostB4CLowerPlateNWallLayersR",2);
	   Control.addVariable("bifrostB4CLowerPlateNWallLayersL",2);
	   Control.addVariable("bifrostB4CLowerPlateNFloorLayers",1);
	   Control.addVariable("bifrostB4CLowerPlateNRoofLayers",2);
	   Control.addVariable("bifrostB4CLowerPlateSpacing",0);
	   //	   Control.addVariable("bifrostB4CLowerPlateZStep",9.9);
	   Control.addVariable("bifrostB4CLowerPlateLength",54);
	   Control.addVariable("bifrostB4CLowerPlateRight",36);
	   Control.addVariable("bifrostB4CLowerPlateVoidLeft",30);
	   Control.addVariable("bifrostB4CLowerPlateVoidRight",30);
	   Control.addVariable("bifrostB4CLowerPlateHeight",15.0);
	   Control.addVariable("bifrostB4CLowerPlateVoidHeight",7.5);
	   Control.addVariable("bifrostB4CLowerPlateDepth",0);
	   Control.addVariable("bifrostB4CLowerPlateVoidDepth",0);
	   //	   Control.addVariable("bifrostB4CLowerPlateDefMat","B4CAluminium");
	   //	   Control.addVariable("bifrostB4CLowerPlateDefMat","SteelS355");
	   Control.addVariable("bifrostB4CLowerPlateDefMat","MagnadenseHC");
	   Control.addVariable("bifrostB4CLowerPlateVoidMat","MagnadenseHC");
	   //	   Control.addVariable("bifrostB4CLowerPlateVoidMat","SteelS355");
	   //	   Control.addVariable("bifrostB4CLowerPlateRoofMat","LimestoneConcrete");
	   Control.addVariable("bifrostB4CLowerPlateNLayers",2);




	   
	   Control.addVariable("bifrostExtrusionPlateLeft",31);
	   Control.addVariable("bifrostExtrusionPlateNWallLayersR",2);
	   Control.addVariable("bifrostExtrusionPlateNWallLayersL",2);
	   Control.addVariable("bifrostExtrusionPlateNFloorLayers",1);
	   Control.addVariable("bifrostExtrusionPlateNRoofLayers",2);
	   Control.addVariable("bifrostExtrusionPlateSpacing",0);
	   //	   Control.addVariable("bifrostExtrusionPlateZStep",9.9);
	   Control.addVariable("bifrostExtrusionPlateLength",77);
	   Control.addVariable("bifrostExtrusionPlateRight",31);
	   Control.addVariable("bifrostExtrusionPlateVoidLeft",25);
	   Control.addVariable("bifrostExtrusionPlateVoidRight",25);
	   Control.addVariable("bifrostExtrusionPlateHeight",15.0);
	   Control.addVariable("bifrostExtrusionPlateVoidHeight",7.5);
	   Control.addVariable("bifrostExtrusionPlateDepth",0);
	   Control.addVariable("bifrostExtrusionPlateVoidDepth",0);
	   //	   Control.addVariable("bifrostExtrusionPlateDefMat","B4CAluminium");
	   //	   Control.addVariable("bifrostExtrusionPlateDefMat","SteelS355");
	   Control.addVariable("bifrostExtrusionPlateDefMat","MagnadenseHC");
	   Control.addVariable("bifrostExtrusionPlateVoidMat","MagnadenseHC");
	   //	   Control.addVariable("bifrostExtrusionPlateVoidMat","SteelS355");
	   //	   Control.addVariable("bifrostExtrusionPlateRoofMat","LimestoneConcrete");
	   Control.addVariable("bifrostExtrusionPlateNLayers",2);
	   Control.addVariable("bifrostExtrusionPlateYStep",55);


	   

	   /*
	   Control.addVariable("bifrostFirstSupportInsertionLeft",27.5);
	   Control.addVariable("bifrostFirstSupportInsertionSpacing",0);
	   Control.addVariable("bifrostFirstSupportInsertionLength",60);
	   Control.addVariable("bifrostFirstSupportInsertionRight",27.5);
	   Control.addVariable("bifrostFirstSupportInsertionVoidLeft",20.5);
	   Control.addVariable("bifrostFirstSupportInsertionVoidRight",20.5);
	   Control.addVariable("bifrostFirstSupportInsertionHeight",70);
	   Control.addVariable("bifrostFirstSupportInsertionVoidHeight",65);
	   Control.addVariable("bifrostFirstSupportInsertionDepth",-60);
	   Control.addVariable("bifrostFirstSupportInsertionVoidDepth",-60);
	   Control.addVariable("bifrostFirstSupportInsertionDefMat","SteelS355");
	   Control.addVariable("bifrostFirstSupportInsertionVoidMat","SteelS355");
	   Control.addVariable("bifrostFirstSupportInsertionNLayers",2);
	   */

	   

	   Control.addVariable("bifrostD03LeadFirstALeft",46.5);
	   Control.addVariable("bifrostD03LeadFirstALength",55);
	   //     Control.addVariable("bifrostD03LeadFirstANSeg",2);
	   Control.addVariable("bifrostD03LeadFirstARight",46.5);
	   Control.addVariable("bifrostD03LeadFirstAVoidLeft",34.5);
	   Control.addVariable("bifrostD03LeadFirstAVoidRight",34.5);
	   Control.addVariable("bifrostD03LeadFirstAHeight",60);
	   Control.addVariable("bifrostD03LeadFirstAVoidHeight",40);
	      Control.addVariable("bifrostD03LeadFirstAInnerB4CLayerThick",1.5);
	      Control.addVariable("bifrostD03LeadFirstANWallLayersR",4);
	      Control.addVariable("bifrostD03LeadFirstANWallLayersL",4);
      	      Control.addVariable("bifrostD03LeadFirstANRoofLayers",6);

	     Control.addVariable("bifrostD03LeadFirstAB4CTileMat","B4CConcrete");

	     
	     Control.addVariable("bifrostD03SteelFirstALeft",34.5);
	   Control.addVariable("bifrostD03SteelFirstALength",5+20+5);
	   //   Control.addVariable("bifrostD03SteelFirstNSeg",3);
	   Control.addVariable("bifrostD03SteelFirstARight",34.5);
	   Control.addVariable("bifrostD03SteelFirstAVoidLeft",24.5);
	   Control.addVariable("bifrostD03SteelFirstAVoidRight",24.5);
	   Control.addVariable("bifrostD03SteelFirstAHeight",46.5);
	   Control.addVariable("bifrostD03SteelFirstAVoidHeight",40);
	      Control.addVariable("bifrostD03SteelFirstAInnerB4CLayerThick",1.5);
	      Control.addVariable("bifrostD03SteelFirstANWallLayersR",3);
	      Control.addVariable("bifrostD03SteelFirstANWallLayersL",3);
      	      Control.addVariable("bifrostD03SteelFirstANRoofLayers",2);

	     Control.addVariable("bifrostD03SteelFirstAB4CTileMat","B4CConcrete");




	     Control.addVariable("bifrostD03B4CFirstALeft",34.5);
	   Control.addVariable("bifrostD03B4CFirstALength",1.5);
	   //   Control.addVariable("bifrostD03B4CFirstNSeg",3);
	   Control.addVariable("bifrostD03B4CFirstARight",34.5);
	   Control.addVariable("bifrostD03B4CFirstAVoidLeft",24.5);
	   Control.addVariable("bifrostD03B4CFirstAVoidRight",24.5);
	   Control.addVariable("bifrostD03B4CFirstAHeight",46.5);
	   Control.addVariable("bifrostD03B4CFirstAVoidHeight",40);
	      Control.addVariable("bifrostD03B4CFirstAInnerB4CLayerThick",1.5);
	      Control.addVariable("bifrostD03B4CFirstANWallLayersR",3);
	      Control.addVariable("bifrostD03B4CFirstANWallLayersL",3);
      	      Control.addVariable("bifrostD03B4CFirstANRoofLayers",2);

	     Control.addVariable("bifrostD03B4CFirstAB4CTileMat","B4CConcrete");
	     Control.addVariable("bifrostD03B4CFirstADefMat","B4CConcrete");




	     
	   Control.addVariable("bifrostD03SteelFirstBLeft",46.5);
	   Control.addVariable("bifrostD03SteelFirstBLength",75);
	   Control.addVariable("bifrostD03SteelFirstBNSeg",2);
	   Control.addVariable("bifrostD03SteelFirstBNWallLayersR",5);
	   Control.addVariable("bifrostD03SteelFirstBNWallLayersL",5);
	   Control.addVariable("bifrostD03SteelFirstBNRoofLayers",5);
	   Control.addVariable("bifrostD03SteelFirstBRight",46.5);
	   Control.addVariable("bifrostD03SteelFirstBVoidLeft",24.5);
	   Control.addVariable("bifrostD03SteelFirstBVoidRight",24.5);
	   Control.addVariable("bifrostD03SteelFirstBHeight",60);
	   Control.addVariable("bifrostD03SteelFirstBVoidHeight",40);
	    Control.addVariable("bifrostD03SteelFirstBInnerB4CLayerThick",1.5);
	     Control.addVariable("bifrostD03SteelFirstBB4CTileMat","B4CConcrete");
	    

	   	   Control.addVariable("bifrostD03SteelShutter0DefMat","SteelS355");
           Control.addVariable("bifrostD03SteelShutter0Length",20.0);
           Control.addVariable("bifrostD03SteelShutter0NSeg",2);
           Control.addVariable("bifrostD03SteelShutter0NWallLayersR",2);
           Control.addVariable("bifrostD03SteelShutter0NWallLayersL",2);
	   Control.addVariable("bifrostD03SteelShutter0NRoofLayers",2);
	   Control.addVariable("bifrostD03SteelShutter0SplitVoid",0);
	   //	   double ShutterZStep=45;
	   //Control.addVariable("bifrostD03SteelShutter0Height",108-ShutterZStep);
	   //D03SteelShutter4Height-1.2
           Control.addVariable("bifrostD03SteelShutter0Height",46.5+5);
           Control.addVariable("bifrostD03SteelShutter0VoidHeight",46.5);
           //Control.addVariable("bifrostD03SteelShutter0Left",45);
	   // Control.addVariable("bifrostD03SteelShutter0Right",45);
	   //    Control.addVariable("bifrostD03SteelShutter0ZStep",ShutterZStep);
	   //Width = Shutter3Left - Gap
           Control.addVariable("bifrostD03SteelShutter0Left",34+5); 
	   Control.addVariable("bifrostD03SteelShutter0Right",34+5);
	   Control.addVariable("bifrostD03SteelShutter0VoidLeft",34);
	   Control.addVariable("bifrostD03SteelShutter0VoidRight",34);
	   Control.addVariable("bifrostD03SteelShutter0Depth",-0.50);
	   Control.addVariable("bifrostD03SteelShutter0VoidDepth",-0.50);
	   Control.addVariable("bifrostD03SteelShutter0VoidMat","Air");
	    Control.addVariable("bifrostD03SteelShutter0NFloorLayers",1);




	   	   Control.addVariable("bifrostD03SteelShutter8DefMat","SteelS355");
           Control.addVariable("bifrostD03SteelShutter8Length",20.0);
           Control.addVariable("bifrostD03SteelShutter8NSeg",2);
           Control.addVariable("bifrostD03SteelShutter8NWallLayersR",2);
           Control.addVariable("bifrostD03SteelShutter8NWallLayersL",2);
	   Control.addVariable("bifrostD03SteelShutter8NRoofLayers",2);
	   Control.addVariable("bifrostD03SteelShutter8SplitVoid",0);
	   //	   double ShutterZStep=45;
	   //Control.addVariable("bifrostD03SteelShutter8Height",108-ShutterZStep);
	   //D03SteelShutter4Height-1.2
           Control.addVariable("bifrostD03SteelShutter8Height",46.5+5);
           Control.addVariable("bifrostD03SteelShutter8VoidHeight",46.5);
           //Control.addVariable("bifrostD03SteelShutter8Left",45);
	   // Control.addVariable("bifrostD03SteelShutter8Right",45);
	   //    Control.addVariable("bifrostD03SteelShutter8ZStep",ShutterZStep);
	   //Width = Shutter3Left - Gap
           Control.addVariable("bifrostD03SteelShutter8Left",34+5); 
	   Control.addVariable("bifrostD03SteelShutter8Right",34+5);
	   Control.addVariable("bifrostD03SteelShutter8VoidLeft",34);
	   Control.addVariable("bifrostD03SteelShutter8VoidRight",34);
	   Control.addVariable("bifrostD03SteelShutter8Depth",-0.50);
	   Control.addVariable("bifrostD03SteelShutter8VoidDepth",-0.50);
	   Control.addVariable("bifrostD03SteelShutter8VoidMat","Air");
	    Control.addVariable("bifrostD03SteelShutter8NFloorLayers",1);

	    

	   Control.addVariable("bifrostD03SteelShutter1DefMat","SteelS355");
           Control.addVariable("bifrostD03SteelShutter1Length",10.0);
           Control.addVariable("bifrostD03SteelShutter1NSeg",2);
           Control.addVariable("bifrostD03SteelShutter1NWallLayersR",3);
           Control.addVariable("bifrostD03SteelShutter1NWallLayersL",3);
	   Control.addVariable("bifrostD03SteelShutter1NRoofLayers",4);
	   Control.addVariable("bifrostD03SteelShutter1SplitVoid",0);
	   //	   double ShutterZStep=45;
	   //Control.addVariable("bifrostD03SteelShutter1Height",108-ShutterZStep);
	   //D03SteelShutter4Height-1.2
           Control.addVariable("bifrostD03SteelShutter1Height",98-1.2);
           Control.addVariable("bifrostD03SteelShutter1VoidHeight",46.5);
           //Control.addVariable("bifrostD03SteelShutter1Left",45);
	   // Control.addVariable("bifrostD03SteelShutter1Right",45);
	   //    Control.addVariable("bifrostD03SteelShutter1ZStep",ShutterZStep);
	   //Width = Shutter3Left - Gap
           Control.addVariable("bifrostD03SteelShutter1Left",46.25-1.2+5+5); 
	   Control.addVariable("bifrostD03SteelShutter1Right",46.25-1.2+5+5);
	   Control.addVariable("bifrostD03SteelShutter1VoidLeft",34);
	   Control.addVariable("bifrostD03SteelShutter1VoidRight",34);
	   Control.addVariable("bifrostD03SteelShutter1Depth",-0.50);
	   Control.addVariable("bifrostD03SteelShutter1VoidDepth",-0.50);
	   Control.addVariable("bifrostD03SteelShutter1VoidMat","SteelS355");
	    Control.addVariable("bifrostD03SteelShutter1NFloorLayers",2);
	    // Control.addVariable("bifrostD03SteelShutter1FloorMat","B4CConcrete");


	   Control.addVariable("bifrostD03SteelShutter7DefMat","SteelS355");
           Control.addVariable("bifrostD03SteelShutter7Length",10.0);
           Control.addVariable("bifrostD03SteelShutter7NSeg",2);
           Control.addVariable("bifrostD03SteelShutter7NWallLayersR",3);
           Control.addVariable("bifrostD03SteelShutter7NWallLayersL",3);
	   Control.addVariable("bifrostD03SteelShutter7NRoofLayers",4);
	   Control.addVariable("bifrostD03SteelShutter7SplitVoid",0);
	   //	   double ShutterZStep=45;
	   //Control.addVariable("bifrostD03SteelShutter7Height",108-ShutterZStep);
	   //D03SteelShutter4Height-1.2
           Control.addVariable("bifrostD03SteelShutter7Height",98-1.2);
           Control.addVariable("bifrostD03SteelShutter7VoidHeight",46.5);
           //Control.addVariable("bifrostD03SteelShutter7Left",45);
	   // Control.addVariable("bifrostD03SteelShutter7Right",45);
	   //    Control.addVariable("bifrostD03SteelShutter7ZStep",ShutterZStep);
	   //Width = Shutter3Left - Gap
           Control.addVariable("bifrostD03SteelShutter7Left",46.25-1.2+5+5); 
	   Control.addVariable("bifrostD03SteelShutter7Right",46.25-1.2+5+5);
	   Control.addVariable("bifrostD03SteelShutter7VoidLeft",34);
	   Control.addVariable("bifrostD03SteelShutter7VoidRight",34);
	   Control.addVariable("bifrostD03SteelShutter7Depth",-0.50);
	   Control.addVariable("bifrostD03SteelShutter7VoidDepth",-0.50);
	   Control.addVariable("bifrostD03SteelShutter7VoidMat","SteelS355");
	   Control.addVariable("bifrostD03SteelShutter7NFloorLayers",2);



	    
	   Control.addVariable("bifrostD03SteelShutter2DefMat","SteelS355");
           Control.addVariable("bifrostD03SteelShutter2Length",10.0);
           Control.addVariable("bifrostD03SteelShutter2NSeg",2);
           Control.addVariable("bifrostD03SteelShutter2NWallLayersR",3);
           Control.addVariable("bifrostD03SteelShutter2NWallLayersL",3);
	   Control.addVariable("bifrostD03SteelShutter2NRoofLayers",4);
	   Control.addVariable("bifrostD03SteelShutter2SplitVoid",1);
	   double ShutterZStep=45;
	   //Control.addVariable("bifrostD03SteelShutter2Height",108-ShutterZStep);
           Control.addVariable("bifrostD03SteelShutter2Height",75-ShutterZStep);
           Control.addVariable("bifrostD03SteelShutter2VoidHeight",75-ShutterZStep-35);
           //Control.addVariable("bifrostD03SteelShutter2Left",45);
	   // Control.addVariable("bifrostD03SteelShutter2Right",45);
           Control.addVariable("bifrostD03SteelShutter2ZStep",ShutterZStep);
           Control.addVariable("bifrostD03SteelShutter2Left",23.5);
	   Control.addVariable("bifrostD03SteelShutter2Right",23.5);
	   Control.addVariable("bifrostD03SteelShutter2VoidLeft",10);
	   Control.addVariable("bifrostD03SteelShutter2VoidRight",10);
	   Control.addVariable("bifrostD03SteelShutter2Depth",10);
	   // Control.addVariable("bifrostD03SteelShutter2VoidDepth",8.5);
	   Control.addVariable("bifrostD03SteelShutter2VoidMat","SteelS355");
	    Control.addVariable("bifrostD03SteelShutter2NFloorLayers",2);
           Control.addVariable("bifrostD03SteelShutter2FloorMat","B4CConcrete");

	  
	   
	   Control.addVariable("bifrostD03SteelShutter3DefMat","B4CConcrete");
	   Control.addVariable("bifrostD03SteelShutter3VoidMat","B4CConcrete");
   //Limestone concrete has similar density as B4C tiles, similar effect for gamma
           Control.addVariable("bifrostD03SteelShutter3Height",75-ShutterZStep);
           Control.addVariable("bifrostD03SteelShutter3VoidHeight",75-ShutterZStep-15);
           Control.addVariable("bifrostD03SteelShutter3Length",1.50);
           // Control.addVariable("bifrostD03SteelShutter3NSeg",3);
	   //       Control.addVariable("bifrostD03SteelShutter3Height",108);
	   Control.addVariable("bifrostD03SteelShutter3VoidDepth",10);
           Control.addVariable("bifrostD03SteelShutter3Left",23.5);
           Control.addVariable("bifrostD03SteelShutter3Right",23.5);
           Control.addVariable("bifrostD03SteelShutter3VoidLeft",14);
           Control.addVariable("bifrostD03SteelShutter3VoidRight",14);
	    Control.addVariable("bifrostD03SteelShutter3NLayerWallR",2);
	    Control.addVariable("bifrostD03SteelShutter3NLayerWallL",2);
	    Control.addVariable("bifrostD03SteelShutter3NLayerRoof",2);
	    // Control.addVariable("bifrostD03SteelShutter4ZStep",-ShutterZStep);

	   

	   Control.addVariable("bifrostD03SteelShutter4DefMat","SteelS355");
           Control.addVariable("bifrostD03SteelShutter4Length",73.5);
           Control.addVariable("bifrostD03SteelShutter4YStep",1);
           Control.addVariable("bifrostD03SteelShutter4NSeg",2);
           Control.addVariable("bifrostD03SteelShutter4Height",98);
           Control.addVariable("bifrostD03SteelShutter4Left",46.25);
           Control.addVariable("bifrostD03SteelShutter4Right",46.25);
           Control.addVariable("bifrostD03SteelShutter4VoidHeight",78);
           Control.addVariable("bifrostD03SteelShutter4VoidLeft",26.25);
           Control.addVariable("bifrostD03SteelShutter4VoidRight",26.25);
           Control.addVariable("bifrostD03SteelShutter4InnerB4CLayerThick",1.5);
           Control.addVariable("bifrostD03SteelShutter4B4CTileMat","B4CConcrete");


	   
	   Control.addVariable("bifrostD03SteelShutter6DefMat","SteelS355");
	   Control.addVariable("bifrostD03SteelShutter6Length",10.0);
           Control.addVariable("bifrostD03SteelShutter6NSeg",2);
           Control.addVariable("bifrostD03SteelShutter6VoidHeight",75-ShutterZStep-35);
           Control.addVariable("bifrostD03SteelShutter6Height",75-ShutterZStep);
           Control.addVariable("bifrostD03SteelShutter6ZStep",ShutterZStep);
           Control.addVariable("bifrostD03SteelShutter6Left",23.5);
           Control.addVariable("bifrostD03SteelShutter6Right",23.5);
	   Control.addVariable("bifrostD03SteelShutter6VoidLeft",10);
	   Control.addVariable("bifrostD03SteelShutter6VoidRight",10);
	   Control.addVariable("bifrostD03SteelShutter6Depth",10);
	   // Control.addVariable("bifrostD03SteelShutter2VoidDepth",8.5);
	   Control.addVariable("bifrostD03SteelShutter6VoidMat","SteelS355");
	    Control.addVariable("bifrostD03SteelShutter6NFloorLayers",2);
           Control.addVariable("bifrostD03SteelShutter6FloorMat","B4CConcrete");
           Control.addVariable("bifrostD03SteelShutter6SplitVoid",1);
	   Control.addVariable("bifrostE02SteelSystemB0ZStep",-ShutterZStep);
	    Control.addVariable("bifrostD03SteelShutter4NWallLayersR",5);
	    Control.addVariable("bifrostD03SteelShutter4NWallLayersL",5);
	    Control.addVariable("bifrostD03SteelShutter4NRoofLayers",5);
	    Control.addVariable("bifrostD03SteelShutter6NWallLayersR",5);
	    Control.addVariable("bifrostD03SteelShutter6NWallLayersL",5);
	    Control.addVariable("bifrostD03SteelShutter6NRoofLayers",5);


	   Control.addVariable("bifrostD03SteelShutter5DefMat","B4CConcrete");
	   Control.addVariable("bifrostD03SteelShutter5VoidMat","B4CConcrete");
   //Limestone concrete has similar density as B4C tiles, similar effect for gamma
           Control.addVariable("bifrostD03SteelShutter5Height",75-ShutterZStep);
           Control.addVariable("bifrostD03SteelShutter5VoidHeight",75-ShutterZStep-15);
           Control.addVariable("bifrostD03SteelShutter5Length",1.50);
           // Control.addVariable("bifrostD03SteelShutter5NSeg",3);
	   //       Control.addVariable("bifrostD03SteelShutter5Height",108);
	   Control.addVariable("bifrostD03SteelShutter5VoidDepth",10);
           Control.addVariable("bifrostD03SteelShutter5Left",23.5);
           Control.addVariable("bifrostD03SteelShutter5Right",23.5);
           Control.addVariable("bifrostD03SteelShutter5VoidLeft",14);
           Control.addVariable("bifrostD03SteelShutter5VoidRight",14);
	    Control.addVariable("bifrostD03SteelShutter5NLayerWallR",2);
	    Control.addVariable("bifrostD03SteelShutter5NLayerWallL",2);
	    Control.addVariable("bifrostD03SteelShutter5NLayerRoof",2);
	    // Control.addVariable("bifrostD03SteelShutter4ZStep",-ShutterZStep);

    //	   Control.addVariable("bifrostD03SteelShutterPhantomDefMat","Stainless304");
           Control.addVariable("bifrostD03SteelShutterPhantomLength",15.0);
           Control.addVariable("bifrostD03SteelShutterPhantomNSeg",1);
           Control.addVariable("bifrostD03SteelShutterPhantomHeight",98);
           Control.addVariable("bifrostD03SteelShutterPhantomLeft",51.25-5);
           Control.addVariable("bifrostD03SteelShutterPhantomRight",51.25-5);

	   Control.addVariable("bifrostD03SteelShutterExtraLength",15.0);
           Control.addVariable("bifrostD03SteelShutterExtraNSeg",1);
           Control.addVariable("bifrostD03SteelShutterExtraHeight",98);
           Control.addVariable("bifrostD03SteelShutterExtraDepth",-0.5);
           Control.addVariable("bifrostD03SteelShutterExtraSpacing",0.5);
           Control.addVariable("bifrostD03SteelShutterExtraSpacingMat","Air");
           Control.addVariable("bifrostD03SteelShutterExtraVoidDepth",-0.5);
           Control.addVariable("bifrostD03SteelShutterExtraVoidHeight",98-5);
           Control.addVariable("bifrostD03SteelShutterExtraLeft",51.25+5);
           Control.addVariable("bifrostD03SteelShutterExtraRight",51.25+5);
          Control.addVariable("bifrostD03SteelShutterExtraVoidLeft",51.25-5);
           Control.addVariable("bifrostD03SteelShutterExtraVoidRight",51.25-5);



	   Control.addVariable("bifrostD03SteelVoidShutterExtraLength",15.0);
           Control.addVariable("bifrostD03SteelVoidShutterExtraNSeg",1);
           Control.addVariable("bifrostD03SteelVoidShutterExtraHeight",98);
           Control.addVariable("bifrostD03SteelVoidShutterExtraDepth",-0.5);
	   //           Control.addVariable("bifrostD03SteelVoidShutterExtraSpacing",0.5);
	   //           Control.addVariable("bifrostD03SteelVoidShutterExtraSpacingMat","Air");
           Control.addVariable("bifrostD03SteelVoidShutterExtraDefMat","Air");
           Control.addVariable("bifrostD03SteelVoidShutterExtraVoidDepth",-0.5);
           Control.addVariable("bifrostD03SteelVoidShutterExtraVoidHeight",98-5);
           Control.addVariable("bifrostD03SteelVoidShutterExtraLeft",51.25+5);
           Control.addVariable("bifrostD03SteelVoidShutterExtraRight",51.25+5);
          Control.addVariable("bifrostD03SteelVoidShutterExtraVoidLeft",51.25-5);
           Control.addVariable("bifrostD03SteelVoidShutterExtraVoidRight",51.25-5);

	   
	   
           Control.addVariable("bifrostD03FirstBaseLength1Edge",184.);
           Control.addVariable("bifrostD03FirstBaseLength1Mid",198.5);
	   //	            Control.addVariable("bifrostD03FirstBaseWidth3Front",97);
	   	            Control.addVariable("bifrostD03FirstBaseWidth3Front",108.8);
           Control.addVariable("bifrostD03FirstBaseDefMatF","MagnadenseHC");
           Control.addVariable("bifrostD03FirstBaseDefMatB","MagnadenseHC");
	   // Control.addVariable("bifrostD03FirstBaseWidth3Front","MagnadenseHC");
           Control.addVariable("bifrostD03Base0DefMatB","MagnadenseHC");
           Control.addVariable("bifrostD03Base0DefMatF","MagnadenseHC");

           Control.addVariable("bifrostD03Base1DefMatB","MagnadenseHC");
           Control.addVariable("bifrostD03Base1DefMatF","MagnadenseHC");

	     Control.addVariable("bifrostD03Base2DefMatB","MagnadenseHC");
	     Control.addVariable("bifrostD03Base2DefMatF","MagnadenseHC");

	     //	   Control.addVariable("bifrostD03Base0DefMatB","MagnadenseHC");
	     Control.addVariable("bifrostD03Base3DefMatB","MagnadenseHC");
	     Control.addVariable("bifrostD03Base3DefMatF","MagnadenseHC");


	   Control.addVariable("bifrostD03BaseSteelStart0Width1Front",108.8);
	   Control.addVariable("bifrostD03BaseSteelStart0Width1Back",108.8);
	   //	   Control.addVariable("bifrostD03BaseSteelShutterWidth2Front",94.);
	   //	   Control.addVariable("bifrostD03BaseSteelShutterWidth2Back",94.);
	   Control.addVariable("bifrostD03BaseSteelStart0Width3Front",96.);
	   Control.addVariable("bifrostD03BaseSteelStart0Width3Back",96.);
	   Control.addVariable("bifrostD03BaseSteelStart0Length2Edge",40);
	   Control.addVariable("bifrostD03BaseSteelStart0Length1Edge",40);
	   Control.addVariable("bifrostD03BaseSteelStart0Length1Mid",40.);
	   Control.addVariable("bifrostD03BaseSteelStart0Gap",0);
	   //	   Control.addVariable("bifrostD03BaseSteelStart0GapV",0);
	   Control.addVariable("bifrostD03BaseSteelStart0NLayers",3);
	   Control.addVariable("bifrostD03BaseSteelStart0HeightUp",100);
           Control.addVariable("bifrostD03BaseSteelStart0DefMatF","SteelS355");
           Control.addVariable("bifrostD03BaseSteelStart0DefMatB","SteelS355");
	   //	   Control.addVariable("bifrostD03BaseSteelStart0InnerB4CLayerThick",1.5);
	   //	   Control.addVariable("bifrostD03BaseSteelStart0B4CTileMat","B4CConcrete");
	   //	   Control.addVariable("bifrostD03BaseSteelStart0B4CTileMatB","B4CConcrete");

	   Control.addVariable("bifrostD03BaseSteelStart1Width1Front",96.);
	   Control.addVariable("bifrostD03BaseSteelStart1Width1Back",96.);
	   //	   Control.addVariable("bifrostD03BaseSteelShutterWidth2Front",94.);
	   //	   Control.addVariable("bifrostD03BaseSteelShutterWidth2Back",94.);
	   Control.addVariable("bifrostD03BaseSteelStart1Width3Front",76.);
	   Control.addVariable("bifrostD03BaseSteelStart1Width3Back",69.);
	   Control.addVariable("bifrostD03BaseSteelStart1Length2Edge",120);
	   Control.addVariable("bifrostD03BaseSteelStart1Length1Edge",55);
	   Control.addVariable("bifrostD03BaseSteelStart1Length1Mid",55.);
	   Control.addVariable("bifrostD03BaseSteelStart1Gap",0);
	   //	   Control.addVariable("bifrostD03BaseSteelStart1GapV",0);
	   Control.addVariable("bifrostD03BaseSteelStart1NLayers",3);
	   Control.addVariable("bifrostD03BaseSteelStart1HeightUp",90);
           Control.addVariable("bifrostD03BaseSteelStart1DefMatF","Lead51121");
           Control.addVariable("bifrostD03BaseSteelStart1DefMatB","SteelS355");
	   Control.addVariable("bifrostD03BaseSteelStart1InnerB4CLayerThick",1.5);
	   Control.addVariable("bifrostD03BaseSteelStart1B4CTileMat","B4CConcrete");
	   Control.addVariable("bifrostD03BaseSteelStart1B4CTileMatB","B4CConcrete");


	   Control.addVariable("bifrostD03BaseSteelStart2Width1Front",70.);
	   Control.addVariable("bifrostD03BaseSteelStart2Width1Back",70.);
	   //	   Control.addVariable("bifrostD03BaseSteelShutterWidth2Front",94.);
	   //	   Control.addVariable("bifrostD03BaseSteelShutterWidth2Back",94.);
	   Control.addVariable("bifrostD03BaseSteelStart2Width3Front",60.);
	   Control.addVariable("bifrostD03BaseSteelStart2Width3Back",60.);
	   Control.addVariable("bifrostD03BaseSteelStart2Length2Edge",20);
	   Control.addVariable("bifrostD03BaseSteelStart2Length1Edge",180);
	   Control.addVariable("bifrostD03BaseSteelStart2Length1Mid",180.);
	   Control.addVariable("bifrostD03BaseSteelStart2Gap",0);
	   //	   Control.addVariable("bifrostD03BaseSteelStart0GapV",0);
	   Control.addVariable("bifrostD03BaseSteelStart2NLayers",1);
	   Control.addVariable("bifrostD03BaseSteelStart2HeightUp",90);
           Control.addVariable("bifrostD03BaseSteelStart2DefMatF","SteelS355");
           Control.addVariable("bifrostD03BaseSteelStart2DefMatB","SteelS355");




          
	    Control.addVariable("bifrostD03FirstUpperBlockLength1Edge",84.);
	     Control.addVariable("bifrostD03FirstUpperBlockNSeg",3);
	   Control.addVariable("bifrostD03FirstUpperBlockZStep",100.0);
	   Control.addVariable("bifrostD03FirstUpperBlockLength1Mid",99.5);
	            Control.addVariable("bifrostD03FirstUpperBlockWidth3Front",46.5*2);
	            Control.addVariable("bifrostD03FirstUpperBlockWidth3Back",93);
            Control.addVariable("bifrostD03FirstUpperBlockHeightLowFront",60.);
             Control.addVariable("bifrostD03FirstUpperBlockHeightLowBack",60);
           Control.addVariable("bifrostD03FirstUpperBlockDefMatF","MagnadenseHC");
           Control.addVariable("bifrostD03FirstUpperBlockDefMatB","MagnadenseHC");

	   //	Control.addVariable("bifrostD03SecondUpperBlockLength1Edge",84.5);
	   //     Control.addVariable("bifrostD03SecondUpperBlockLength1Mid",99.5);
      	        Control.addVariable("bifrostD03SecondUpperBlockWidth3Front",93);
		    //  Control.addVariable("bifrostD03SecondUpperBlockWidth3Back",83);
            Control.addVariable("bifrostD03SecondUpperBlockHeightLowFront",60);
	   Control.addVariable("bifrostD03SecondUpperBlockDefMatB","MagnadenseHC");
	   Control.addVariable("bifrostD03SecondUpperBlockDefMatF","MagnadenseHC");
	   Control.addVariable("bifrostD03Upper0DefMatF","MagnadenseHC");
	   Control.addVariable("bifrostD03Upper0DefMatB","MagnadenseHC");
	   Control.addVariable("bifrostD03Upper1DefMatF","MagnadenseHC");


	    Control.addVariable("bifrostD03Upper16DefMatF","MagnadenseHC");
  Control.addVariable("bifrostD03Upper17DefMatF","MagnadenseHC");
  Control.addVariable("bifrostD03Upper17DefMatB","MagnadenseHC");
  Control.addVariable("bifrostD03UpperShutterStartDefMatF","MagnadenseHC");
  Control.addVariable("bifrostD03UpperShutterStartDefMatB","MagnadenseHC");
  Control.addVariable("bifrostD03UpperShutterEndDefMatF","MagnadenseHC");
  Control.addVariable("bifrostD03UpperShutterEndDefMatB","MagnadenseHC");
  Control.addVariable("bifrostD03UpperShutter0DefMatF","MagnadenseHC");
  Control.addVariable("bifrostD03UpperShutter0DefMatB","MagnadenseHC");
  Control.addVariable("bifrostD03UpperShutter1DefMatF","MagnadenseHC");
  Control.addVariable("bifrostD03UpperShutter1DefMatB","MagnadenseHC");
   Control.addVariable("bifrostE02Upper0DefMatF","MagnadenseHC");
   //  Control.addVariable("bifrostD03UpperShutter0DefMatB","MagnadenseHC");
  Control.addVariable("bifrostD03BaseShutter0DefMatF","MagnadenseHC");
  Control.addVariable("bifrostD03BaseShutter0DefMatB","MagnadenseHC");
  Control.addVariable("bifrostD03BaseShutter1DefMatF","MagnadenseHC");
  Control.addVariable("bifrostD03BaseShutter1DefMatB","MagnadenseHC");

      
      //Copied from heimdal:
      
        SGen.clearLayers();
  SGen.addWallMat(1,"MagnadenseHC");
  SGen.addRoofMat(1,"MagnadenseHC");
  SGen.addFloorMat(1,"MagnadenseHC");
  
  //manually select length (1st arg of shield generator) to choose gap size 
  SGen.generateShield(Control,"bifrostShieldGap1",30, 150, 150, 155.0, 2,6);
  Control.addVariable("bifrostShieldGap1VoidLeft",75.25);
    Control.addVariable("bifrostShieldGap1VoidRight",75.25);
      Control.addVariable("bifrostShieldGap1VoidHeight",75.25);
  Control.addVariable("bifrostShieldGap1Left",117.25);
    Control.addVariable("bifrostShieldGap1Right",117.25);
      Control.addVariable("bifrostShieldGap1Height",117.25);
        Control.addVariable("bifrostShieldGap1VoidDepth",175);
        Control.addVariable("bifrostShieldGap1Depth",175);
        Control.addVariable("bifrostShieldGap1VoidMat","Air");
	Control.addVariable("bifrostShieldGap1NFloorLayers",1);
        Control.addVariable("bifrostShieldGap1Spacing",0.0);
		Control.addVariable("bifrostShieldGap1YStep",320.0);
	//		Control.addVariable("bifrostShieldGap1YStep",-29.5);
  
  SGen.generateShield(Control,"bifrostShieldGap2",200.0, 71.0, 71.0, 150.0, 1,7);
  Control.addVariable("bifrostShieldGap2VoidLeft",41.0);
    Control.addVariable("bifrostShieldGap2VoidRight",41.0);
      Control.addVariable("bifrostShieldGap2VoidHeight",41.0);
        Control.addVariable("bifrostShieldGap2VoidDepth",100.0);
	Control.addVariable("bifrostShieldGap2FinalLayerThick",0.5);
      	Control.addVariable("bifrostShieldGap2YStep",1.0);
  Control.addVariable("bifrostShieldGap2NRoofLayers",6);



  SGen.generateShield(Control,"bifrostInnerShieldGap1",20.0, 56.0, 56.0, 170.0, 1,6);
  Control.addVariable("bifrostInnerShieldGap1VoidLeft",41.0);
    Control.addVariable("bifrostInnerShieldGap1VoidRight",41.0);
      Control.addVariable("bifrostInnerShieldGap1VoidHeight",41.0);
        Control.addVariable("bifrostInnerShieldGap1VoidDepth",100.0);
       Control.addVariable("bifrostInnerShieldGap1Spacing",0.0);
       Control.addVariable("bifrostInnerShieldGap1NFloorLayers",3);
       Control.addVariable("bifrostInnerShieldGap1InnerB4CLayerThick",0.0);
       Control.addVariable("bifrostInnerShieldGap1OuterB4CLayerThick",0.0);
       //  Control.addVariable("bifrostInnerShieldGap1B4CTileMat","B4C");
      	Control.addVariable("bifrostInnerShieldGap1YStep",1.0);

	
  SGen.clearLayers();
  SGen.addWallMat(1,"ChipIRSteel");
  SGen.addRoofMat(1,"ChipIRSteel");
  SGen.addFloorMat(1,"ChipIRSteel");

  Control.addVariable("DefaultTileThick",0.0);
  
  SGen.generateShield(Control,"bifrostInnerShieldGap2",220.0, 40.95, 40.95, 20.0, 1,5);
  Control.addVariable("bifrostInnerShieldGap2VoidLeft",30.95);
    Control.addVariable("bifrostInnerShieldGap2VoidRight",30.95);
      Control.addVariable("bifrostInnerShieldGap2VoidHeight",30.95);
        Control.addVariable("bifrostInnerShieldGap2VoidDepth",20.0);
       Control.addVariable("bifrostInnerShieldGap2Spacing",0.0);
       Control.addVariable("bifrostInnerShieldGap2NFloorLayers",1);
       //      Control.addVariable("bifrostInnerShieldGap2InnerB4CLayerThick",0.5);
       //       Control.addVariable("bifrostInnerShieldGap2OuterB4CLayerThick",0.5);
       //      Control.addVariable("bifrostInnerShieldGap2B4CTileMat","B4C");





       

    
  SGen.generateShield(Control,"bifrostInnerShieldA",2500.0,30.95, 30.95,20.0, 1,4);
  Control.addVariable("bifrostInnerShieldAVoidLeft",21.0);
    Control.addVariable("bifrostInnerShieldAVoidRight",21.0);
      Control.addVariable("bifrostInnerShieldAVoidHeight",21.0);
        Control.addVariable("bifrostInnerShieldAVoidDepth",20.0);
        Control.addVariable("bifrostInnerShieldASpacing",0.0);
        Control.addVariable("bifrostInnerShieldANFloorLayers",1);

	
  SGen.generateShield(Control,"bifrostInnerShieldB",2500.0,30.95, 30.95,20.0, 1,4);
  Control.addVariable("bifrostInnerShieldBVoidLeft",21.0);
    Control.addVariable("bifrostInnerShieldBVoidRight",21.0);
      Control.addVariable("bifrostInnerShieldBVoidHeight",21.0);
        Control.addVariable("bifrostInnerShieldBVoidDepth",20.0);
        Control.addVariable("bifrostInnerShieldBSpacing",0.0);
        Control.addVariable("bifrostInnerShieldBNFloorLayers",1);





SGen.generateShield(Control,"bifrostShutterFrontB4C",0.1, 30.95, 30.95, 21.95, 1, 2);
 Control.addVariable("bifrostShutterFrontB4CVoidLeft",57.0);
   Control.addVariable("bifrostShutterFrontB4CVoidRight",57.0);
     Control.addVariable("bifrostShutterFrontB4CVoidHeight",57.0);
 Control.addVariable("bifrostShutterFrontB4CLeft",82.0);
   Control.addVariable("bifrostShutterFrontB4CRight",82.0);
     Control.addVariable("bifrostShutterFrontB4CHeight",82.0);
       Control.addVariable("bifrostShutterFrontB4CVoidDepth",21.95);
       Control.addVariable("bifrostShutterFrontB4CSpacing",0.0);
       Control.addVariable("bifrostShutterFrontB4CWallMat1","B4C");
       Control.addVariable("bifrostShutterFrontB4CRoofMat1","B4C");


      

 SGen.generateShield(Control,"bifrostShutterRearB4C",0.1, 152.0, 152.0, 21.95,1, 3);
 Control.addVariable("bifrostShutterRearB4CVoidLeft",32.0);
 Control.addVariable("bifrostShutterRearB4CVoidRight",32.0);
     Control.addVariable("bifrostShutterRearB4CVoidHeight",32.0);
 Control.addVariable("bifrostShutterRearB4CLeft",91.95);
   Control.addVariable("bifrostShutterRearB4CRight",91.95);
     Control.addVariable("bifrostShutterRearB4CHeight",91.95);
       Control.addVariable("bifrostShutterRearB4CVoidDepth",21.95);
       Control.addVariable("bifrostShutterRearB4CSpacing",0.0);
       Control.addVariable("bifrostShutterRearB4CVoidMat","Void");
       Control.addVariable("bifrostShutterRearB4CWallMat1","LimestoneConcrete");
       Control.addVariable("bifrostShutterRearB4CRoofMat1","LimestoneConcrete");
      Control.addVariable("bifrostShutterRearB4CWallMat2","B4C");
       Control.addVariable("bifrostShutterRearB4CRoofMat2","B4C");



	


SGen.generateShield(Control,"bifrostShutterInnerShield",150.0, 44.95, 71.95,99.95, 1,5);
  Control.addVariable("bifrostShutterInnerShieldVoidLeft",25.0);
    Control.addVariable("bifrostShutterInnerShieldVoidRight",25.0);
      Control.addVariable("bifrostShutterInnerShieldVoidHeight",52.0);
        Control.addVariable("bifrostShutterInnerShieldVoidDepth",99.9);
        Control.addVariable("bifrostShutterInnerShieldSpacing",0.5);
        Control.addVariable("bifrostShutterInnerShieldInnerB4CLayerThick",0.1);
        Control.addVariable("bifrostShutterInnerShieldB4CTileMat","B4C");

       
SGen.generateShield(Control,"bifrostShutterFrontHorseshoe",10.0, 24.95, 51.95, 20, 2, 2);
 Control.addVariable("bifrostShutterFrontHorseshoeVoidLeft",21.0);
   Control.addVariable("bifrostShutterFrontHorseshoeVoidRight",21.0);
     Control.addVariable("bifrostShutterFrontHorseshoeVoidHeight",21.0);
       Control.addVariable("bifrostShutterFrontHorseshoeVoidDepth",20);
       Control.addVariable("bifrostShutterFrontHorseshoeSpacing",0.0);
       Control.addVariable("bifrostShutterFrontHorseshoeNFloorLayers",1);

       
SGen.generateShield(Control,"bifrostShutterRearHorseshoe",5.0, 24.95, 51.95, 20, 1, 2);
 Control.addVariable("bifrostShutterRearHorseshoeVoidLeft",21.0);
   Control.addVariable("bifrostShutterRearHorseshoeVoidRight",21.0);
     Control.addVariable("bifrostShutterRearHorseshoeVoidHeight",21.0);
       Control.addVariable("bifrostShutterRearHorseshoeVoidDepth",20);
       Control.addVariable("bifrostShutterRearHorseshoeSpacing",0.0);
       Control.addVariable("bifrostShutterRearHorseshoeNFloorLayers",1);


	
	Control.addVariable("bifrostInnerShieldGap1NFloorLayers",1);
	  Control.addVariable("bifrostInnerShieldGap2NFloorLayers",2);
	  Control.addVariable("bifrostInnerShieldANFloorLayers",2);
	  Control.addVariable("bifrostShutterInnerShieldNFloorLayers",2);
	  Control.addVariable("bifrostShutterInnerShieldSpacing",0.5);

		
  SGen.clearLayers();
  SGen.addWallMat(1, "MagnadenseHC");
  SGen.addRoofMat(1, "MagnadenseHC");
  SGen.addFloorMat(1,"MagnadenseHC");

  
  /*
	  //manually select length (1st arg of shield generator) to choose gap size 
  SGen.generateShield(Control,"bifrostShieldGap1",30.0, 92.0, 92.0, 130.0, 1,6);
  Control.addVariable("bifrostShieldGap1VoidLeft",32.0);
    Control.addVariable("bifrostShieldGap1VoidRight",32.0);
      Control.addVariable("bifrostShieldGap1VoidHeight",32.0);
        Control.addVariable("bifrostShieldGap1VoidDepth",100.0);
	//	Control.addVariable("bifrostInnerShieldGap1YStep",-28.0);
		Control.addVariable("bifrostShieldGap1YStep",-29.5);
		Control.addVariable("bifrostShieldGap1NFloorLayers",3);


		


  SGen.generateShield(Control,"bifrostShieldGap2",70.0, 92.0, 92.0, 170.0, 1,6);
  Control.addVariable("bifrostShieldGap2VoidLeft",32.0);
    Control.addVariable("bifrostShieldGap2VoidRight",32.0);
      Control.addVariable("bifrostShieldGap2VoidHeight",32.0);
        Control.addVariable("bifrostShieldGap2VoidDepth",100.0);


  */

	
  SGen.clearLayers();
  SGen.addWallMat(1, "LimestoneConcrete");
  SGen.addRoofMat(1, "LimestoneConcrete");
  SGen.addFloorMat(1,"LimestoneConcrete");
	

  
  SGen.generateShield(Control,"bifrostShieldA",1880.0, 71.0, 71.0, 150.0, 1, 6);
  Control.addVariable("bifrostShieldAVoidLeft",31.0);
    Control.addVariable("bifrostShieldAVoidRight",31.0);
      Control.addVariable("bifrostShieldAVoidHeight",31.0);
        Control.addVariable("bifrostShieldAVoidDepth",100.0);

	
  
  SGen.generateShield(Control,"bifrostShieldB",1860.0, 71.0, 71.0, 150.0, 1, 6);
  Control.addVariable("bifrostShieldBVoidLeft",31.0);
    Control.addVariable("bifrostShieldBVoidRight",31.0);
      Control.addVariable("bifrostShieldBVoidHeight",31.0);
        Control.addVariable("bifrostShieldBVoidDepth",100.0);


  
SGen.generateShield(Control,"bifrostShutterFrontWall",15.0, 95.0, 122.0, 170.0, 2, 6);
 Control.addVariable("bifrostShutterFrontWallVoidLeft",31.0);
   Control.addVariable("bifrostShutterFrontWallVoidRight",31.0);
     Control.addVariable("bifrostShutterFrontWallVoidHeight",31.0);
       Control.addVariable("bifrostShutterFrontWallVoidDepth",100.0);


SGen.generateShield(Control,"bifrostShutterExpansion",90.0, 95.0, 122.0, 170.0, 1, 5);
 Control.addVariable("bifrostShutterExpansionVoidLeft",45.0);
   Control.addVariable("bifrostShutterExpansionVoidRight",45.0);
     Control.addVariable("bifrostShutterExpansionVoidHeight",72.0);
       Control.addVariable("bifrostShutterExpansionVoidDepth",100.0);
  
       
SGen.generateShield(Control,"bifrostShutterRearWall",15.0, 95.0, 122.0, 170.0, 2, 6);
 Control.addVariable("bifrostShutterRearWallVoidLeft",31.0);
   Control.addVariable("bifrostShutterRearWallVoidRight",31.0);
     Control.addVariable("bifrostShutterRearWallVoidHeight",31.0);
       Control.addVariable("bifrostShutterRearWallVoidDepth",100.0);

 

  //Shutter position is 49.423 in TCS

  Control.addVariable("bifrostShutterAFrameInnerWidth",19.0);
  Control.addVariable("bifrostShutterAFrameInnerHeight",40.0);
  Control.addVariable("bifrostShutterAFrameWidth",30.0);
  Control.addVariable("bifrostShutterAFrameHeight",80.0);
  Control.addVariable("bifrostShutterAFrameThick",3.0);
  Control.addVariable("bifrostShutterAFrameYStep",6.0);//offset
  Control.addVariable("bifrostShutterAFrameZStep",10.0);//offset
  // Control.addVariable("bifrostShutterAFrameConstruct",0); // set to 1 to construct
  Control.addVariable("bifrostShutterAFrameDefMat","Aluminium");
  Control.addVariable("bifrostShutterAFrameVoidMat","Air");

  Control.addVariable("bifrostShutterARailsVoidMat","Air");
  Control.addVariable("bifrostShutterARailsInnerWidth",12.0);
  Control.addVariable("bifrostShutterARailsInnerHeight",39.0);
  Control.addVariable("bifrostShutterARailsWidth",18.0);
  Control.addVariable("bifrostShutterARailsHeight",40.0);
  Control.addVariable("bifrostShutterARailsThick",3.0);
  Control.addVariable("bifrostShutterARailsYStep",6.00);//offset
  Control.addVariable("bifrostShutterARailsZStep",10.0);//offset
  // Control.addVariable("bifrostShutterARailsConstruct",0); // set to 1 to construct
  Control.addVariable("bifrostShutterARailsDefMat","Stainless304");


  
  Control.addVariable("bifrostShutterAB4CNSlices",2);
  Control.addVariable("bifrostShutterAB4CNLayers0",5);
  Control.addVariable("bifrostShutterAB4CNLayers1",8);
  Control.addVariable("bifrostShutterAB4CWidth",10);
  Control.addVariable("bifrostShutterAB4CHeight",15);
  Control.addVariable("bifrostShutterAB4CDepth",0);
  Control.addVariable("bifrostShutterAB4CLength0",1.0);
  Control.addVariable("bifrostShutterAB4CMat0","B4C");
  Control.addVariable("bifrostShutterAB4CLength1",8.0);
  Control.addVariable("bifrostShutterAB4CMat1","Lead51121");
  //  Control.addVariable("bifrostShutterAB4CZStep",-10); //down
   Control.addVariable("bifrostShutterAB4CZStep",5); //up

  Control.addVariable("bifrostShutterAPedestalDefMat","LimestoneConcrete");
  Control.addVariable("bifrostShutterAPedestalNSeg",5);
  Control.addVariable("bifrostShutterAPedestalLength",30);
  Control.addVariable("bifrostShutterAPedestalSplitVoid",1);
  Control.addVariable("bifrostShutterAPedestalVoidMat","LimestoneConcrete");
  Control.addVariable("bifrostShutterAPedestalHeight",80);
  Control.addVariable("bifrostShutterAPedestalVoidHeight",50);
  Control.addVariable("bifrostShutterAPedestalZStep",2);
  Control.addVariable("bifrostShutterAPedestalDepth",0);
  Control.addVariable("bifrostShutterAPedestalVoidDepth",0);
  Control.addVariable("bifrostShutterAPedestalVoidRight",15);
  Control.addVariable("bifrostShutterAPedestalVoidLeft",15);
  Control.addVariable("bifrostShutterAPedestalRight",25);
  Control.addVariable("bifrostShutterAPedestalLeft",25);
  //  Control.addVariable("bifrostShutterAPedestalDefMat","Stainless304");
  // Control.addVariable("bifrostShutterAPedestalDefMat","Stainless304");



  
       
	/*
  // Shield: leaving bunker
  SGen.clearLayers();
  SGen.addWallMat(1,"Concrete");
  SGen.addRoofMat(1,"Concrete");
  SGen.addFloorMat(1,"Concrete");
  SGen.generateShield(Control,"bifrostShieldA",5000.0, 140.0, 140.0, 140.0, 4, 4);
	*/
	/*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables 
    \param keyName :: head name for variable
    \param length :: overall length
    \param side :: full extent at sides, dist. from beam center to the outermost surface
    \param height :: Full height
    \param depth :: Full depth
    \param NSeg :: number of segments
    \param NLayer :: number of layers
  */



  // ARRAY SECTION
  

  PGen.setFeLayer(6.0);
  PGen.setConcLayer(10.0);
  //  PGen.generatePit(Control,"bifrostOutPitA",6505.0,25.0,160.0,210.0,40.0);
    PGen.generatePit(Control,"bifrostOutPitA",5130.0,25.0,160.0,210.0,40.0);

  Control.addVariable("bifrostOutACutFrontShape","Square");
  Control.addVariable("bifrostOutACutFrontRadius",5.0);
  
  Control.addVariable("bifrostOutACutBackShape","Square");
  Control.addVariable("bifrostOutACutBackRadius",5.0);

  // FIRST out of bunker chopper
  CGen.setMainRadius(38.122);
  CGen.setFrame(86.5,86.5);
  CGen.generateChopper(Control,"bifrostChopperOutA",22.0,12.0,6.55);

  // Singe Blade chopper FOC
  BGen.setThick({0.2});
  BGen.addPhase({95},{60.0});
  BGen.generateBlades(Control,"bifrostFOCOutABlade",0.0,20.0,35.0);

  // Shield: leaving chopper pit A
  SGen.generateShield(Control,"bifrostShieldC",7945.0,140.0,140.0,140.0,4,4);

  // ARRAY SECTION

  

  FGen.setLayer(1,0.5,"BorkronNBK7");
  FGen.setLayer(2,0.1,"Void");

  PipeGen.generatePipe(Control,"bifrostPipeS0",0,6140.0);

  for(size_t i=0;i<10;i++)
    {
      const std::string strNum(StrFunc::makeString(i));
      //   PipeGen.generatePipe(Control,"bifrostPipeS"+strNum,yStep,600.0);
      FGen.generateRectangle(Control,"bifrostFOutS"+strNum,610.0,100.0*hOut,100.0*vOut);
      Control.addVariable("bifrostFOutS"+strNum+"YStep",0.2);
    //yStep=4.0;
    }

double   yStep = 14.0; // space for chopper disc
    Control.addVariable("bifrostFOutS0YStep",yStep);
    Control.addVariable("bifrostPipeS0WindowActive",0);


  //Focusing section
  //  FGen.setLayer(1,0.5,"Copper");
  double  length1 = 22.1142,
    Linx1 = 24.3645,Loutx1 = 2.25034,majoraxis_x1 = 24.3646,smallaxis_x1=0.03,
    Liny1 = 23.0344,Louty1 = 0.920233,majoraxis_y1 = 23.0345,smallaxis_y1=0.045;
  //double gap(8.0);
  yStep=0;
  for(size_t i=0;i<6;i++)
    {
   majoroffset_x =
     (length1+Linx1+Loutx1)/2 - Loutx1 - length1 +0.02+0.01*i*(yStep+300); 
   hIn =  2*sqrt(1-majoroffset_x*majoroffset_x/majoraxis_x1/majoraxis_x1)*smallaxis_x1;
   majoroffset_y=(length1+Liny1+Louty1)/2 - Louty1 - length1 + 0.02 +0.01*i*(yStep+300);
   vIn = 2*sqrt(1-majoroffset_y*majoroffset_y/majoraxis_y1/majoraxis_y1)*smallaxis_y1;

   majoroffset_x =
     (length1+Linx1+Loutx1)/2 - Loutx1 - length1 - 0.02 +0.01*(i+1)*(yStep+300);
   hOut =  2*sqrt(1-majoroffset_x*majoroffset_x/majoraxis_x1/majoraxis_x1)*smallaxis_x1;
   majoroffset_y
     =(length1+Liny1+Louty1)/2 - Louty1 - length1 - 0.02 +0.01*(i+1)*(yStep+300);
   vOut = 2*sqrt(1-majoroffset_y*majoroffset_y/majoraxis_y1/majoraxis_y1)*smallaxis_y1;
      const std::string strNum(StrFunc::makeString(i));
      PipeGen.generatePipe(Control,"bifrostPipeE"+strNum,yStep,600.0);
      /* FGen.generateTaper(Control,"bifrostFOutE"+strNum,360,//396.0,
                         hIn*100.001,hOut*100,vIn*100.001,vOut*100.00);
      */
      
      if (i==0)
      FGen.generateRectangle(Control,"bifrostFOutE"+strNum,300,100.0*hOut,100.0*vOut);
    else      FGen.generateTaperGeneral(Control,"bifrostFOutE"+strNum,300,
			    hIn*50.001,hOut*50.00,
  			    hIn*50.001,hOut*50.00,
  			    vIn*50,vOut*50.,
  			    vIn*50,vOut*50.);
      //      std::cout << i <<" "<< hIn*100  <<" "<< hOut*100  <<" "<< vIn*100
      //	<<" "<< vOut*100 << std::endl;
	}

    Control.addVariable("bifrostE02PillarCoverL1IChicane",1);
    Control.addVariable("bifrostE02PillarCoverL7IChicane",1);
    Control.addVariable("bifrostE02PillarCoverL11IChicane",1);
    Control.addVariable("bifrostE02PillarCoverL15IChicane",1);
    Control.addVariable("bifrostE02PillarCoverL19IChicane",1);
   // Chopper pit variables, base
    Control.addVariable("bifrostE02PillarCoverL4ITubes",1);
    Control.addVariable("bifrostE02PillarCoverL4NLayers",5);
    Control.addVariable("bifrostE02PillarCoverR4NLayers",5);
    Control.addVariable("bifrostE02PillarCoverR4ITubes",-1);
    Control.addVariable("bifrostE02PillarCoverL4YStep",-40);
    Control.addVariable("bifrostE02PillarCoverR4YStep",-40);
    Control.addVariable("bifrostE02PillarCoverL4VoidLength",15+40-1.5);
    Control.addVariable("bifrostE02PillarCoverL4Length",95+40);
    Control.addVariable("bifrostE02PillarCoverL4Left",(380-5)*0.5);
    Control.addVariable("bifrostE02PillarCoverR4Left",(380-5)*0.5);
    Control.addVariable("bifrostE02PillarCoverL4VoidLeft",(211-5)*0.5);

    Control.addVariable("bifrostE02PillarCoverR4VoidLeft",(211-5)*0.5);
    Control.addVariable("bifrostE02PillarCoverL4Right",(380-5)*0.5);
    Control.addVariable("bifrostE02PillarCoverR4Right",(380-5)*0.5);
    Control.addVariable("bifrostE02PillarCoverL4VoidRight",(211-5)*0.5);
    Control.addVariable("bifrostE02PillarCoverR4VoidRight",(211-5)*0.5);
    Control.addVariable("bifrostE02PillarCoverL4Gap",0.1);
    Control.addVariable("bifrostE02PillarCoverR4Gap",0.1);
    Control.addVariable("bifrostE02PillarCoverR4VoidLength",15+40-1.5);
    Control.addVariable("bifrostE02PillarCoverL4VoidHeight",100);
    Control.addVariable("bifrostE02PillarCoverR4VoidHeight",100);
    Control.addVariable("bifrostE02PillarCoverL4Height",100);
    Control.addVariable("bifrostE02PillarCoverR4Height",100);
    Control.addVariable("bifrostE02PillarCoverR4Length",95+40);
	   
	   Control.addVariable("bifrostE02BaseSteelChopperYStep",92.);
	   Control.addVariable("bifrostE02BaseSteelChopperNLayers",3);
	   Control.addVariable("bifrostE02BaseSteelChopperWidth1Front",220.);
	   Control.addVariable("bifrostE02BaseSteelChopperWidth1Back",220.);
	   Control.addVariable("bifrostE02BaseSteelChopperInnerB4CLayerThick",1.5);
	   //	   Control.addVariable("bifrostD03BaseSteelShutter2Width2Front",94.);
	   //	   Control.addVariable("bifrostD03BaseSteelShutter2Width2Back",94.);
	   Control.addVariable("bifrostE02BaseSteelChopperWidth3Front",180.);
	   Control.addVariable("bifrostE02BaseSteelChopperWidth3Back",180.);
	   Control.addVariable("bifrostE02BaseSteelChopperLength2Edge",74);
	   Control.addVariable("bifrostE02BaseSteelChopperLength1Edge",80);
	   Control.addVariable("bifrostE02BaseSteelChopperLength1Mid",80.);
	   Control.addVariable("bifrostE02BaseSteelChopperGap",0);
	   Control.addVariable("bifrostE02BaseSteelChopperHeightUp",110);
           Control.addVariable("bifrostE02BaseSteelChopperDefMatF","SteelS355");
           Control.addVariable("bifrostE02BaseSteelChopperDefMatB","SteelS355");


	   //	   Control.addVariable("bifrostE02SteelSystemF22DefMat","B4CConcrete");
	   //	   Control.addVariable("bifrostE02SteelSystemF22VoidMat","B4CConcrete");
   //Limestone concrete has similar density as B4C tiles, similar effect for gamma
           Control.addVariable("bifrostE02SteelSystemF22Height",165);
           Control.addVariable("bifrostE02SteelSystemF22VoidHeight",145);
	   //       Control.addVariable("bifrostE02SteelSystemF22Length",1.50);
           // Control.addVariable("bifrostE02SteelSystemF22NSeg",3);
	   //       Control.addVariable("bifrostE02SteelSystemF22Height",108);
	    Control.addVariable("bifrostE02SteelSystemF22VoidDepth",-11);
	    Control.addVariable("bifrostE02SteelSystemF22Depth",-11);
           Control.addVariable("bifrostE02SteelSystemF22Left",110);
           Control.addVariable("bifrostE02SteelSystemF22Right",110);
           Control.addVariable("bifrostE02SteelSystemF22VoidLeft",90);
           Control.addVariable("bifrostE02SteelSystemF22VoidRight",90);
	    Control.addVariable("bifrostE02SteelSystemF22NWallLayersR",4);
	    Control.addVariable("bifrostE02SteelSystemF22NWallLayersL",4);
	    Control.addVariable("bifrostE02SteelSystemF22NRoofLayers",4);
	    Control.addVariable("bifrostE02SteelSystemF22NLayerFloor",1);
	    Control.addVariable("bifrostE02SteelSystemF22InnerB4CLayerThick",1.5);
	    Control.addVariable("bifrostE02SteelSystemF22B4CTileMat","B4CConcrete");
	    Control.addVariable("bifrostE02SteelSystemF22Spacing",1);
    Control.addVariable("bifrostE02SteelSystemF22SpacingMat","Air");
    

           Control.addVariable("bifrostE02SteelSystemB22Height",165);
           Control.addVariable("bifrostE02SteelSystemB22VoidHeight",145);
	   //       Control.addVariable("bifrostE02SteelSystemB22Length",1.50);
           // Control.addVariable("bifrostE02SteelSystemB22NSeg",3);
	   //       Control.addVariable("bifrostE02SteelSystemB22Height",108);
	    Control.addVariable("bifrostE02SteelSystemB22VoidDepth",-11);
	    Control.addVariable("bifrostE02SteelSystemB22Depth",-11);
           Control.addVariable("bifrostE02SteelSystemB22Left",110);
           Control.addVariable("bifrostE02SteelSystemB22Right",110);
           Control.addVariable("bifrostE02SteelSystemB22VoidLeft",90);
           Control.addVariable("bifrostE02SteelSystemB22VoidRight",90);
	    Control.addVariable("bifrostE02SteelSystemB22NWallLayersR",4);
	    Control.addVariable("bifrostE02SteelSystemB22NWallLayersL",4);
	    Control.addVariable("bifrostE02SteelSystemB22NRoofLayers",4);
	    Control.addVariable("bifrostE02SteelSystemB22NLayerFloor",1);
	    Control.addVariable("bifrostE02SteelSystemB22InnerB4CLayerThick",1.5);
	    Control.addVariable("bifrostE02SteelSystemB22B4CTileMat","B4CConcrete");
	    Control.addVariable("bifrostE02SteelSystemB22Spacing",1);
    Control.addVariable("bifrostE02SteelSystemB22SpacingMat","Air");

           Control.addVariable("bifrostE02SteelSystemF23Height",165);
           Control.addVariable("bifrostE02SteelSystemF23VoidHeight",145);
	   //       Control.addVariable("bifrostE02SteelSystemF23Length",1.50);
           // Control.addVariable("bifrostE02SteelSystemF23NSeg",3);
	   //       Control.addVariable("bifrostE02SteelSystemF23Height",108);
	   Control.addVariable("bifrostE02SteelSystemF23VoidDepth",-11);
	   Control.addVariable("bifrostE02SteelSystemF23Depth",-11);
           Control.addVariable("bifrostE02SteelSystemF23Left",110);
           Control.addVariable("bifrostE02SteelSystemF23Right",110);
           Control.addVariable("bifrostE02SteelSystemF23VoidLeft",90);
           Control.addVariable("bifrostE02SteelSystemF23VoidRight",90);
	    Control.addVariable("bifrostE02SteelSystemF23NWallLayersR",4);
	    Control.addVariable("bifrostE02SteelSystemF23NWallLayersL",4);
	    Control.addVariable("bifrostE02SteelSystemF23NRoofLayers",4);
	    Control.addVariable("bifrostE02SteelSystemF23NLayerFloor",1);
	    Control.addVariable("bifrostE02SteelSystemF23InnerB4CLayerThick",1.5);
	    Control.addVariable("bifrostE02SteelSystemF23B4CTileMat","B4CConcrete");
	    Control.addVariable("bifrostE02SteelSystemF23Spacing",1);
            Control.addVariable("bifrostE02SteelSystemF23SpacingMat","Air");

	    
           Control.addVariable("bifrostE02SteelSystemB23Height",165);
           Control.addVariable("bifrostE02SteelSystemB23VoidHeight",145);
	   //       Control.addVariable("bifrostE02SteelSystemB23Length",1.50);
           // Control.addVariable("bifrostE02SteelSystemB23NSeg",3);
	   //       Control.addVariable("bifrostE02SteelSystemB23Height",108);
	   Control.addVariable("bifrostE02SteelSystemB23VoidDepth",-11);
	   Control.addVariable("bifrostE02SteelSystemB23Depth",-11);
           Control.addVariable("bifrostE02SteelSystemB23Left",110);
           Control.addVariable("bifrostE02SteelSystemB23Right",110);
           Control.addVariable("bifrostE02SteelSystemB23VoidLeft",90);
           Control.addVariable("bifrostE02SteelSystemB23VoidRight",90);
	    Control.addVariable("bifrostE02SteelSystemB23NWallLayersR",4);
	    Control.addVariable("bifrostE02SteelSystemB23NWallLayersL",4);
	    Control.addVariable("bifrostE02SteelSystemB23NRoofLayers",4);
	    Control.addVariable("bifrostE02SteelSystemB23NLayerFloor",1);
	    Control.addVariable("bifrostE02SteelSystemB23InnerB4CLayerThick",1.5);
	    Control.addVariable("bifrostE02SteelSystemB23B4CTileMat","B4CConcrete");
	    Control.addVariable("bifrostE02SteelSystemB23Spacing",1);
            Control.addVariable("bifrostE02SteelSystemB23SpacingMat","Air");

    
	    Control.addVariable("bifrostE02SpacingSystem22Height",165);
           Control.addVariable("bifrostE02SpacingSystem22VoidHeight",145);
	   //       Control.addVariable("bifrostE02SpacingSystem23Length",1.50);
           // Control.addVariable("bifrostE02SpacingSystem23NSeg",3);
	   //       Control.addVariable("bifrostE02SpacingSystem23Height",108);
	   Control.addVariable("bifrostE02SpacingSystem22VoidDepth",-10);
           Control.addVariable("bifrostE02SpacingSystem22Left",110);
           Control.addVariable("bifrostE02SpacingSystem22Right",110);
           Control.addVariable("bifrostE02SpacingSystem22VoidLeft",90);
           Control.addVariable("bifrostE02SpacingSystem22VoidRight",90);
	   //  Control.addVariable("bifrostE02SpacingSystem22NLayerWallR",2);
	   //  Control.addVariable("bifrostE02SpacingSystem22NLayerWallL",2);
	    //	    Control.addVariable("bifrostE02SpacingSystem22NLayerRoof",2);

	    Control.addVariable("bifrostE02SpacingSystem23Height",165);
           Control.addVariable("bifrostE02SpacingSystem23VoidHeight",145);
	   //       Control.addVariable("bifrostE02SpacingSystem23Length",1.50);
           // Control.addVariable("bifrostE02SpacingSystem23NSeg",3);
	   //       Control.addVariable("bifrostE02SpacingSystem23Height",108);
	   Control.addVariable("bifrostE02SpacingSystem23VoidDepth",-10);
           Control.addVariable("bifrostE02SpacingSystem23Left",110);
           Control.addVariable("bifrostE02SpacingSystem23Right",110);
           Control.addVariable("bifrostE02SpacingSystem23VoidLeft",90);
           Control.addVariable("bifrostE02SpacingSystem23VoidRight",90);
	   //  Control.addVariable("bifrostE02SpacingSystem23NLayerWallR",2);
	   // Control.addVariable("bifrostE02SpacingSystem23NLayerWallL",2);
	   //  Control.addVariable("bifrostE02SpacingSystem23NLayerRoof",2);

	   
           Control.addVariable("bifrostE02SteelSystemChopperBackHeight",165);
           Control.addVariable("bifrostE02SteelSystemChopperBackVoidHeight",46.5);
	         Control.addVariable("bifrostE02SteelSystemChopperBackLength",20);
            Control.addVariable("bifrostE02SteelSystemChopperBackNSeg",3);
	   //       Control.addVariable("bifrostE02SteelSystemChopperBackHeight",108);
		  Control.addVariable("bifrostE02SteelSystemChopperBackVoidDepth",0);
           Control.addVariable("bifrostE02SteelSystemChopperBackLeft",110);
           Control.addVariable("bifrostE02SteelSystemChopperBackRight",110);
           Control.addVariable("bifrostE02SteelSystemChopperBackVoidLeft",34);
           Control.addVariable("bifrostE02SteelSystemChopperBackVoidRight",34);
	    Control.addVariable("bifrostE02SteelSystemChopperBackNLayerWallR",2);
	    Control.addVariable("bifrostE02SteelSystemChopperBackNLayerWallL",2);
	    Control.addVariable("bifrostE02SteelSystemChopperBackNLayerRoof",2);
	    // Control.addVariable("bifrostE02SteelSystemChopperBackSpacing",1);

	    

           Control.addVariable("bifrostE02SteelSystemChopperFrontHeight",165);
           Control.addVariable("bifrostE02SteelSystemChopperFrontVoidHeight",46.5);
	         Control.addVariable("bifrostE02SteelSystemChopperFrontLength",20);
            Control.addVariable("bifrostE02SteelSystemChopperFrontNSeg",3);
	   //       Control.addVariable("bifrostE02SteelSystemChopperFrontHeight",108);
		  Control.addVariable("bifrostE02SteelSystemChopperFrontVoidDepth",0);
           Control.addVariable("bifrostE02SteelSystemChopperFrontLeft",110);
           Control.addVariable("bifrostE02SteelSystemChopperFrontRight",110);
           Control.addVariable("bifrostE02SteelSystemChopperFrontVoidLeft",34);
           Control.addVariable("bifrostE02SteelSystemChopperFrontVoidRight",34);
	    Control.addVariable("bifrostE02SteelSystemChopperFrontNLayerWallR",2);
	    Control.addVariable("bifrostE02SteelSystemChopperFrontNLayerWallL",2);
	    Control.addVariable("bifrostE02SteelSystemChopperFrontNLayerRoof",2);
	    // Control.addVariable("bifrostE02SteelSystemChopperFrontSpacing",1);

           Control.addVariable("bifrostE02B4CSystemChopperBackHeight",145);
           Control.addVariable("bifrostE02B4CSystemChopperBackVoidHeight",46.5);
	         Control.addVariable("bifrostE02B4CSystemChopperBackLength",1.5);
            Control.addVariable("bifrostE02B4CSystemChopperBackNSeg",1);
	   //       Control.addVariable("bifrostE02B4CSystemChopperBackHeight",108);
		  Control.addVariable("bifrostE02B4CSystemChopperBackVoidDepth",0);
           Control.addVariable("bifrostE02B4CSystemChopperBackLeft",90);
           Control.addVariable("bifrostE02B4CSystemChopperBackRight",90);
           Control.addVariable("bifrostE02B4CSystemChopperBackVoidLeft",34);
           Control.addVariable("bifrostE02B4CSystemChopperBackVoidRight",34);
	    Control.addVariable("bifrostE02B4CSystemChopperBackNLayerWallR",2);
	    Control.addVariable("bifrostE02B4CSystemChopperBackNLayerWallL",2);
	    Control.addVariable("bifrostE02B4CSystemChopperBackNLayerRoof",2);
	    Control.addVariable("bifrostE02B4CSystemChopperBackDefMat","B4CConcrete");

	    

           Control.addVariable("bifrostE02B4CSystemChopperFrontHeight",145);
           Control.addVariable("bifrostE02B4CSystemChopperFrontVoidHeight",46.5);
	         Control.addVariable("bifrostE02B4CSystemChopperFrontLength",1.5);
            Control.addVariable("bifrostE02B4CSystemChopperFrontNSeg",1);
	   //       Control.addVariable("bifrostE02B4CSystemChopperFrontHeight",108);
		  Control.addVariable("bifrostE02B4CSystemChopperFrontVoidDepth",0);
           Control.addVariable("bifrostE02B4CSystemChopperFrontLeft",90);
           Control.addVariable("bifrostE02B4CSystemChopperFrontRight",90);
           Control.addVariable("bifrostE02B4CSystemChopperFrontVoidLeft",34);
           Control.addVariable("bifrostE02B4CSystemChopperFrontVoidRight",34);
	    Control.addVariable("bifrostE02B4CSystemChopperFrontNLayerWallR",2);
	    Control.addVariable("bifrostE02B4CSystemChopperFrontNLayerWallL",2);
	    Control.addVariable("bifrostE02B4CSystemChopperFrontNLayerRoof",2);
	     Control.addVariable("bifrostE02B4CSystemChopperFrontDefMat","B4CConcrete");

	    
  //  const double bendAngle(180.0);
  // const double bendRadiusBI(151853.2237);    // ~1.5km
    FGen.generateRectangle(Control,"PhantombifrostCChicane0",20,20,20);
    FGen.generateRectangle(Control,"bifrostCChicane0",20,20,20);
    FGen.generateBender(Control,"bifrostCChicane1",17.0,20,20,20,20,
                      17,180);
    FGen.generateBender(Control,"PhantombifrostCChicane1",15.0,5,5,5,5,
                      14,180);
    //    FGen.generateBender(Control,"bifrostCChicane2",40.0,20,20,20,20,
    //                30,0);
       Control.addVariable("PhantombifrostCChicane1ZStep",40);
       Control.addVariable("bifrostCChicane1ZStep",40);
       Control.addVariable("bifrostCChicane1XStep",145);
       Control.addVariable("PhantombifrostCChicane1XStep",150);
    FGen.generateRectangle(Control,"bifrostCChicane2",50,20,20);
       FGen.generateRectangle(Control,"PhantombifrostCChicane2",20,20,20);
    //  Control.addVariable("bifrostCChicane2PreXYAngle",60);
     FGen.generateBender(Control,"bifrostCChicane3",15.0,20,20,20,20,
                   15,0);
     FGen.generateBender(Control,"PhantombifrostCChicane3",15.0,20,20,20,20,
                   15,0);
     //            Control.addVariable("bifrostCChicane3ZStep",40);

    FGen.generateRectangle(Control,"bifrostCChicane4",15,20,20);


  // HUT:
  
  Control.addVariable("bifrostCaveYStep",50.0);
  Control.addVariable("bifrostCaveXStep",-55.0);
  Control.addVariable("bifrostCaveVoidFront",60.0);
  Control.addVariable("bifrostCaveVoidHeight",300.0);
  Control.addVariable("bifrostCaveVoidDepth",183.0);
  Control.addVariable("bifrostCaveVoidWidth",610.0);
  Control.addVariable("bifrostCaveVoidLength",700.0);

  Control.addVariable("bifrostCaveFeFront",50.0);
  Control.addVariable("bifrostCaveFeLeftWall",50.0);
  Control.addVariable("bifrostCaveFeRightWall",50.0);
  Control.addVariable("bifrostCaveFeRoof",50.0);
  Control.addVariable("bifrostCaveFeFloor",50.0);
  Control.addVariable("bifrostCaveFeBack",50.0);

  Control.addVariable("bifrostCaveConcFront",50.0);
  Control.addVariable("bifrostCaveConcLeftWall",50.0);
  Control.addVariable("bifrostCaveConcRightWall",50.0);
  Control.addVariable("bifrostCaveConcRoof",50.0);
  Control.addVariable("bifrostCaveConcFloor",50.0);
  Control.addVariable("bifrostCaveConcBack",50.0);

  Control.addVariable("bifrostCaveFeMat","LimestoneConcrete");
  Control.addVariable("bifrostCaveConcMat","LimestoneConcrete");

  // Beam port through front of cave
  Control.addVariable("bifrostCaveCutShape","Circle");
  Control.addVariable("bifrostCaveCutRadius",10.0);

  /**************/

   majoroffset_x = (length1+Linx1+Loutx1)/2 - Loutx1 - 3.872; 
   hIn =  2*sqrt(1-majoroffset_x*majoroffset_x/majoraxis_x1/majoraxis_x1)*smallaxis_x1;
   majoroffset_y=(length1+Liny1+Louty1)/2 - Louty1 - 3.872;
   vIn = 2*sqrt(1-majoroffset_y*majoroffset_y/majoraxis_y1/majoraxis_y1)*smallaxis_y1;
   majoroffset_x = (length1+Linx1+Loutx1)/2 - Loutx1-2.40 ;
   hOut =  2*sqrt(1-majoroffset_x*majoroffset_x/majoraxis_x1/majoraxis_x1)*smallaxis_x1;
   majoroffset_y=(length1+Liny1+Louty1)/2 - Louty1 -2.40 ;
   vOut = 2*sqrt(1-majoroffset_y*majoroffset_y/majoraxis_y1/majoraxis_y1)*smallaxis_y1;
    FGen.generateTaper(Control,"bifrostFCave1",147.2,
                         hIn*100,hOut*100,vIn*100,vOut*100);

  Control.addVariable("bifrostSlitsInnerWidth",6.0);
  Control.addVariable("bifrostSlitsInnerHeight",6.0);
  Control.addVariable("bifrostSlitsWidth",9.0);
  Control.addVariable("bifrostSlitsHeight",9.0);
  Control.addVariable("bifrostSlitsThick",0.5);
  Control.addVariable("bifrostSlitsYStep",1.0);//offset
  // Control.addVariable("bifrostSlitsConstruct",0); // set to 1 to construct
  Control.addVariable("bifrostSlitsDefMat","B4C");
  Control.addVariable("bifrostSlitsVoidMat","B4C");


  Control.addVariable("bifrostB4CChopperInnerWidth",6.0);
  Control.addVariable("bifrostB4CChopperInnerHeight",9.0);
  Control.addVariable("bifrostB4CChopperWidth",9.0);
  Control.addVariable("bifrostB4CChopperHeight",10.0);
  Control.addVariable("bifrostB4CChopperThick",0.5);
  Control.addVariable("bifrostB4CChopperYStep",1.0);//offset
  // Control.addVariable("bifrostB4CChopperConstruct",0); // set to 1 to construct
  Control.addVariable("bifrostB4CChopperDefMat","Aluminium");
  Control.addVariable("bifrostB4CChopperVoidMat","B4C");



  
  Control.addVariable("bifrostBeamMonitorChopperInnerWidth",6.0);
  Control.addVariable("bifrostBeamMonitorChopperInnerHeight",9.0);
  Control.addVariable("bifrostBeamMonitorChopperWidth",9.0);
  Control.addVariable("bifrostBeamMonitorChopperHeight",10.0);
  Control.addVariable("bifrostBeamMonitorChopperThick",0.2);
  Control.addVariable("bifrostBeamMonitorChopperYStep",4.0);//offset
  // Control.addVariable("bifrostBeamMonitorChopperConstruct",0); // set to 1 to construct
  Control.addVariable("bifrostBeamMonitorChopperDefMat","Aluminium");
  Control.addVariable("bifrostBeamMonitorChopperVoidMat","Aluminium");


   majoroffset_x = (length1+Linx1+Loutx1)/2 - Loutx1 - 2.38; 
   hIn =  2*sqrt(1-majoroffset_x*majoroffset_x/majoraxis_x1/majoraxis_x1)*smallaxis_x1;
   majoroffset_y=(length1+Liny1+Louty1)/2 - Louty1 - 2.38;
   vIn = 2*sqrt(1-majoroffset_y*majoroffset_y/majoraxis_y1/majoraxis_y1)*smallaxis_y1;
   majoroffset_x = (length1+Linx1+Loutx1)/2 - Loutx1 ;
   hOut =  2*sqrt(1-majoroffset_x*majoroffset_x/majoraxis_x1/majoraxis_x1)*smallaxis_x1;
   majoroffset_y=(length1+Liny1+Louty1)/2 - Louty1  ;
   vOut = 2*sqrt(1-majoroffset_y*majoroffset_y/majoraxis_y1/majoraxis_y1)*smallaxis_y1;
    FGen.generateTaper(Control,"bifrostFCave2",238.0,
                         hIn*100,hOut*100,vIn*100,vOut*100);
    Control.addVariable ("bifrostFCave2YStep",2.00);
    Control.addVariable ("bifrostFCave1YStep",0.00);

    

    //Sample environment -- need to find out how to take input from files in external project
    //offset with respect to the link point the magnet base will be attached to 
    Control.addVariable("SampleMagnetBaseYStep",58.0);
  Control.addVariable("SampleMagnetBaseXStep",0.0);

  Control.addVariable("SampleMagnetBaseInnerRadius",4.0); // Space in the middle for the sample stick
  Control.addVariable("SampleMagnetBaseOuterRadius",27.5);
  
  Control.addVariable("SampleMagnetBaseNLayers",4);
  
  Control.addVariable("SampleMagnetBaseLRadius0",6.5);
  Control.addVariable("SampleMagnetBaseLRadius1",13.0);
  Control.addVariable("SampleMagnetBaseLRadius2",19.50);
  Control.addVariable("SampleMagnetBaseLRadius3",25.5);
  Control.addVariable("SampleMagnetBaseLThick",0.5); // Default value for Al ring thickness
  Control.addVariable("SampleMagnetBaseLThick0",1.0);
  Control.addVariable("SampleMagnetBaseLThick1",0.8);
  Control.addVariable("SampleMagnetBaseLThick2",0.6);
  Control.addVariable("SampleMagnetBaseLThick3",0.2);
  Control.addVariable("SampleMagnetBaseLTemp",300.0);
  
  Control.addVariable("SampleMagnetBaseTopOffset",1.0);
  Control.addVariable("SampleMagnetBaseBaseOffset",1.0);
  Control.addVariable("SampleMagnetBaseCutTopAngle",88.0);
  Control.addVariable("SampleMagnetBaseCutBaseAngle",88.0);

  Control.addVariable("SampleMagnetBaseTopThick",10.0);
  Control.addVariable("SampleMagnetBaseBaseThick",10.0);

  Control.addVariable("SampleMagnetBaseApertureWidth",2.0); // width of opening in the rings for neutron beam passage
  Control.addVariable("SampleMagnetBaseApertureHeight",2.0);  // height of opening in the rings for neutron beam passage

  Control.addVariable("SampleMagnetBaseMat","Aluminium");
  Control.addVariable("SampleMagnetBaseSampleMat","Vanadium");
  Control.addVariable("SampleMagnetBaseVoidMat","Air");



  Control.addVariable("ASampleHolderThickness",0.5);
  Control.addVariable("ASampleHolderOuterRadius",3.5);
  Control.addVariable("ASampleHolderTop",60.0);
  Control.addVariable("ASampleHolderBottom",30.0);
  Control.addVariable("ASampleHolderTemp",300.0);
  
  Control.addVariable("ASampleSampleRadius",1.0);
  Control.addVariable("ASampleSampleHeight",2.0);
  Control.addVariable("ASampleSampleTemp",300.0);
 
  Control.addVariable("ASampleHolderMat","Aluminium");
  Control.addVariable("ASampleSampleMat","Vanadium");

  /****Cave variables *****/

  


  int nLayer=5;
  Control.addVariable("BrickShapeInLayers",nLayer);

  for (int iwall = 0 ; iwall<nLayer; iwall++){
  Control.addVariable("BrickShapeInLayerThick"+std::to_string(iwall),59.0/nLayer);
  }
  Control.addVariable("BrickShapeInMat","LimestoneConcrete");
  Control.addVariable("BrickShapeInSurroundMat","Air");



  Control.addVariable("BrickShapeOutWidth",300);
  Control.addVariable("BrickShapeOutHeight",200);
  Control.addVariable("BrickShapeOutBrickWidth",90);
  Control.addVariable("BrickShapeOutBrickHeight",60);
  Control.addVariable("BrickShapeOutBrickThick",60);
  Control.addVariable("BrickShapeOutGapWidthHorizontal",0.6);
  Control.addVariable("BrickShapeOutGapWidthDepth",1.0);
  Control.addVariable("BrickShapeOutGapWidthVertical",0.3);

  Control.addVariable("BrickShapeOutOffsetVertical",40.0);
  Control.addVariable("BrickShapeOutOffsetHorizontal",60.0);

  Control.addVariable("BrickShapeOutLayers",nLayer);

  for (int iwall = 0 ; iwall<nLayer; iwall++){
  Control.addVariable("BrickShapeOutLayerThick"+std::to_string(iwall),59.0/nLayer);
  }
  Control.addVariable("BrickShapeOutMat","Concrete");
  Control.addVariable("BrickShapeOutSurroundMat","Air");





  Control.addVariable("BIFROSTCaveLengthFront",325);
  Control.addVariable("BIFROSTCaveLengthBack",395);
  //  Control.addVariable("BIFROSTCaveHeight",146.3);
        Control.addVariable("BIFROSTCaveHeight",176.3);
  Control.addVariable("BIFROSTCaveDepth",313.7);
  Control.addVariable("BIFROSTCaveWidthRight",370);
  Control.addVariable("BIFROSTCaveWidthLeft",210);
  /*  Control.addVariable("BIFROSTCaveDoglegWidth",60);
  Control.addVariable("BIFROSTCaveDoglegDepth",25);
  Control.addVariable("BIFROSTCaveDoglegLength",80);
  */
    Control.addVariable("BIFROSTCaveZStep",13.7);
    Control.addVariable("BIFROSTCaveYStep",445);

 
   Control.addVariable("BIFROSTCaveThick",120);
  Control.addVariable("BIFROSTCaveWallThick",120);
  //   Control.addVariable("BIFROSTCaveRoofThick",120);
      Control.addVariable("BIFROSTCaveRoofThick",90);
  Control.addVariable("BIFROSTCaveFloorThick",100);
  Control.addVariable("BIFROSTCaveNRoofLayers",10);
  Control.addVariable("BIFROSTCaveNWallLayers",10);
  Control.addVariable("BIFROSTCaveNFloorLayers",10);
  Control.addVariable("BIFROSTCaveRoofExtraThick",0);

  
  


  Control.addVariable("BIFROSTCaveLiningLengthFront",323.9);
  Control.addVariable("BIFROSTCaveLiningLengthBack",393.9);
  // Control.addVariable("BIFROSTCaveLiningHeight",145.2);
     Control.addVariable("BIFROSTCaveLiningHeight",175.2);
  Control.addVariable("BIFROSTCaveLiningDepth",312.6);
  Control.addVariable("BIFROSTCaveLiningWidthRight",368.9);
  Control.addVariable("BIFROSTCaveLiningWidthLeft",208.9);
  /*  Control.addVariable("BIFROSTCaveLiningDoglegWidth",60);
  Control.addVariable("BIFROSTCaveLiningDoglegDepth",25);
  Control.addVariable("BIFROSTCaveLiningDoglegLength",80);
  */
  // Control.addVariable("BIFROSTCaveLiningZStep",313.7);

  // Control.addVariable("BIFROSTCaveLiningWallMat","B-Poly");
  Control.addVariable("BIFROSTCaveLiningWallMat","Air");
  Control.addVariable("BIFROSTCaveLiningInnerMat","Air");
 
  Control.addVariable("BIFROSTCaveLiningThick",1.1);
  Control.addVariable("BIFROSTCaveLiningWallThick",1.1);
  Control.addVariable("BIFROSTCaveLiningRoofThick",1.1);
  Control.addVariable("BIFROSTCaveLiningFloorThick",1.1);
  Control.addVariable("BIFROSTCaveLiningNRoofLayers",1);
  Control.addVariable("BIFROSTCaveLiningNWallLayers",1);
  Control.addVariable("BIFROSTCaveLiningNFloorLayers",1);
  Control.addVariable("BIFROSTCaveLiningRoofExtraThick",0);




  Control.addVariable("BIFROSTCaveCableLow1NSeg",12);
  Control.addVariable("BIFROSTCaveCableLow1NWallLayers",3);
  Control.addVariable("BIFROSTCaveCableLow1NRoofLayers",3);
  Control.addVariable("BIFROSTCaveCableLow1NFloorLayers",1);
  Control.addVariable("BIFROSTCaveCableLow1Length",146);
  Control.addVariable("BIFROSTCaveCableLow1Left",50);
  Control.addVariable("BIFROSTCaveCableLow1Right",50);
  Control.addVariable("BIFROSTCaveCableLow1VoidLeft",25);
  Control.addVariable("BIFROSTCaveCableLow1VoidRight",25);
  Control.addVariable("BIFROSTCaveCableLow1VoidDepth",0);
  Control.addVariable("BIFROSTCaveCableLow1Depth",0);
  Control.addVariable("BIFROSTCaveCableLow1VoidHeight",25);
  Control.addVariable("BIFROSTCaveCableLow1Height",50);
  Control.addVariable("BIFROSTCaveCableLow1Spacing",0.0);
  // Control.addVariable("BIFROSTCaveCableLow1SpacingMat","LimestoneConcrete");
  Control.addVariable("BIFROSTCaveCableLow1DefMat","LimestoneConcrete");
  Control.addVariable("BIFROSTCaveCableLow1VoidMat","Air");
  /*  Control.addVariable("BIFROSTCaveCableLow1XStep", 3.5);
  Control.addVariable("BIFROSTCaveCableLow1YStep", 88.0);
  */
   Control.addVariable("BIFROSTCaveCableLow1B4CMat", "Air");
   Control.addVariable("BIFROSTCaveCableLow1SplitVoid", 0);
   Control.addVariable("BIFROSTCaveCableLow1ZStep", -313.7);
   Control.addVariable("BIFROSTCaveCableLow1XStep", 315.0);

  SGen.generateShield(Control,"BIFROSTCaveCableLowBlock1",25.0, 240, 25.1, 0, 1,2);
   Control.addVariable("BIFROSTCaveCableLowBlock1Right",-50);
   Control.addVariable("BIFROSTCaveCableLowBlock1Left",100.0);
  Control.addVariable("BIFROSTCaveCableLowBlock1VoidLeft",75);
    Control.addVariable("BIFROSTCaveCableLowBlock1VoidRight",25);
      Control.addVariable("BIFROSTCaveCableLowBlock1VoidHeight",25);
      Control.addVariable("BIFROSTCaveCableLowBlock1Height",50);
        Control.addVariable("BIFROSTCaveCableLowBlock1VoidDepth",0);
       Control.addVariable("BIFROSTCaveCableLowBlock1Spacing",0.0);
       Control.addVariable("BIFROSTCaveCableLowBlock1SpacingMat","B4C");
       Control.addVariable("BIFROSTCaveCableLowBlock1InnerB4CLayerThick",0.0);
       Control.addVariable("BIFROSTCaveCableLowBlock1NWallLayers",3);
       Control.addVariable("BIFROSTCaveCableLowBlock1NRoofLayers",3);
       Control.addVariable("BIFROSTCaveCableLowBlock1NFloorLayers",1);
       //       Control.addVariable("BIFROSTCaveCableLowBlock1B4CTileMat","B-Poly");
       //       Control.addVariable("BIFROSTCaveCableLowBlock1DefMat","B-Poly");
       Control.addVariable("BIFROSTCaveCableLowBlock1B4CTileMat","Air");
       Control.addVariable("BIFROSTCaveCableLowBlock1DefMat","LimestoneConcrete");
  Control.addVariable("BIFROSTCaveCableLowBlock1VoidFront",50);
  Control.addVariable("BIFROSTCaveCableLowBlock1Front",75);
    Control.addVariable("BIFROSTCaveCableLowBlock1YStep",0.0);
    // Control.addVariable("BIFROSTCaveCableLowBlock1ZStep",-25.0);
    Control.addVariable("BIFROSTCaveCableLowBlock1XStep",50.0);
    Control.addVariable("BIFROSTCaveCableLowBlock1VoidMat","Air");


  SGen.generateShield(Control,"BIFROSTCaveCableLowBlock11",25.0, 240, 25.1, 0, 1,2);
   Control.addVariable("BIFROSTCaveCableLowBlock11Right",99);
   Control.addVariable("BIFROSTCaveCableLowBlock11Left",-100.0);
  Control.addVariable("BIFROSTCaveCableLowBlock11VoidLeft",75);
    Control.addVariable("BIFROSTCaveCableLowBlock11VoidRight",75);
      Control.addVariable("BIFROSTCaveCableLowBlock11VoidHeight",25);
      Control.addVariable("BIFROSTCaveCableLowBlock11Height",50);
        Control.addVariable("BIFROSTCaveCableLowBlock11VoidDepth",0);
       Control.addVariable("BIFROSTCaveCableLowBlock11Spacing",0.0);
       Control.addVariable("BIFROSTCaveCableLowBlock11SpacingMat","B4C");
       Control.addVariable("BIFROSTCaveCableLowBlock11InnerB4CLayerThick",0.0);
       Control.addVariable("BIFROSTCaveCableLowBlock11NWallLayers",3);
       Control.addVariable("BIFROSTCaveCableLowBlock11NRoofLayers",3);
       Control.addVariable("BIFROSTCaveCableLowBlock11NFloorLayers",1);
       //       Control.addVariable("BIFROSTCaveCableLowBlock11B4CTileMat","B-Poly");
       //       Control.addVariable("BIFROSTCaveCableLowBlock11DefMat","B-Poly");
       Control.addVariable("BIFROSTCaveCableLowBlock11B4CTileMat","Air");
       Control.addVariable("BIFROSTCaveCableLowBlock11DefMat","LimestoneConcrete");
  Control.addVariable("BIFROSTCaveCableLowBlock11VoidFront",50);
  Control.addVariable("BIFROSTCaveCableLowBlock11Front",75);
    Control.addVariable("BIFROSTCaveCableLowBlock11XYAngle",-90.0);
    // Control.addVariable("BIFROSTCaveCableLowBlock11ZStep",-25.0);
    Control.addVariable("BIFROSTCaveCableLowBlock11XStep",0.0);
    Control.addVariable("BIFROSTCaveCableLowBlock11YStep",75.0);
    Control.addVariable("BIFROSTCaveCableLowBlock11VoidMat","Air");



      SGen.generateShield(Control,"BIFROSTCaveCableLowBlock21",25.0, 240, 25.1, 0, 1,2);
   Control.addVariable("BIFROSTCaveCableLowBlock21Right",99);
   Control.addVariable("BIFROSTCaveCableLowBlock21Left",-100.0);
  Control.addVariable("BIFROSTCaveCableLowBlock21VoidLeft",75);
    Control.addVariable("BIFROSTCaveCableLowBlock21VoidRight",75);
      Control.addVariable("BIFROSTCaveCableLowBlock21VoidHeight",25);
      Control.addVariable("BIFROSTCaveCableLowBlock21Height",50);
        Control.addVariable("BIFROSTCaveCableLowBlock21VoidDepth",0);
       Control.addVariable("BIFROSTCaveCableLowBlock21Spacing",0.0);
       Control.addVariable("BIFROSTCaveCableLowBlock21SpacingMat","B4C");
       Control.addVariable("BIFROSTCaveCableLowBlock21InnerB4CLayerThick",0.0);
       Control.addVariable("BIFROSTCaveCableLowBlock21NWallLayers",3);
       Control.addVariable("BIFROSTCaveCableLowBlock21NRoofLayers",3);
       Control.addVariable("BIFROSTCaveCableLowBlock21NFloorLayers",1);
       //       Control.addVariable("BIFROSTCaveCableLowBlock21B4CTileMat","B-Poly");
       //       Control.addVariable("BIFROSTCaveCableLowBlock21DefMat","B-Poly");
       Control.addVariable("BIFROSTCaveCableLowBlock21B4CTileMat","Air");
       Control.addVariable("BIFROSTCaveCableLowBlock21DefMat","LimestoneConcrete");
  Control.addVariable("BIFROSTCaveCableLowBlock21VoidFront",50);
  Control.addVariable("BIFROSTCaveCableLowBlock21Front",75);
    Control.addVariable("BIFROSTCaveCableLowBlock21XYAngle",-90.0);
    // Control.addVariable("BIFROSTCaveCableLowBlock21ZStep",-25.0);
    Control.addVariable("BIFROSTCaveCableLowBlock21XStep",0.0);
    Control.addVariable("BIFROSTCaveCableLowBlock21YStep",75.0);
    Control.addVariable("BIFROSTCaveCableLowBlock21VoidMat","Air");



    
  SGen.generateShield(Control,"BIFROSTCaveCableLowBlock2",25.0, 240, 25.1, 0, 1,2);
   Control.addVariable("BIFROSTCaveCableLowBlock2Right",-50);
   Control.addVariable("BIFROSTCaveCableLowBlock2Left",100.0);
  Control.addVariable("BIFROSTCaveCableLowBlock2VoidLeft",75);
    Control.addVariable("BIFROSTCaveCableLowBlock2VoidRight",25);
      Control.addVariable("BIFROSTCaveCableLowBlock2VoidHeight",25);
      Control.addVariable("BIFROSTCaveCableLowBlock2Height",50);
        Control.addVariable("BIFROSTCaveCableLowBlock2VoidDepth",0);
       Control.addVariable("BIFROSTCaveCableLowBlock2Spacing",0.0);
       Control.addVariable("BIFROSTCaveCableLowBlock2SpacingMat","B4C");
       Control.addVariable("BIFROSTCaveCableLowBlock2InnerB4CLayerThick",1.0);
       Control.addVariable("BIFROSTCaveCableLowBlock2NWallLayers",3);
       Control.addVariable("BIFROSTCaveCableLowBlock2NRoofLayers",3);
       Control.addVariable("BIFROSTCaveCableLowBlock2NFloorLayers",1);
       //          Control.addVariable("BIFROSTCaveCableLowBlock2B4CTileMat","B4CConcrete");
                 Control.addVariable("BIFROSTCaveCableLowBlock2B4CTileMat","B-Poly");
       //       Control.addVariable("BIFROSTCaveCableLowBlock2DefMat","B-Poly");
	  //      Control.addVariable("BIFROSTCaveCableLowBlock2B4CTileMat","Air");
       Control.addVariable("BIFROSTCaveCableLowBlock2DefMat","LimestoneConcrete");
  Control.addVariable("BIFROSTCaveCableLowBlock2VoidFront",50);
  Control.addVariable("BIFROSTCaveCableLowBlock2Front",75);
    Control.addVariable("BIFROSTCaveCableLowBlock2YStep",0.0);
    // Control.addVariable("BIFROSTCaveCableLowBlock2ZStep",-25.0);
    Control.addVariable("BIFROSTCaveCableLowBlock2XStep",50.0);
    Control.addVariable("BIFROSTCaveCableLowBlock2VoidMat","Air");
    Control.addVariable("BIFROSTCaveCableLowBlock2FrontLining",0);


   

  Control.addVariable("BIFROSTCaveCableLow2NSeg",12);
  Control.addVariable("BIFROSTCaveCableLow2NWallLayers",3);
  Control.addVariable("BIFROSTCaveCableLow2NRoofLayers",3);
  Control.addVariable("BIFROSTCaveCableLow2NFloorLayers",1);
  Control.addVariable("BIFROSTCaveCableLow2Length",146);
  Control.addVariable("BIFROSTCaveCableLow2Left",50);
  Control.addVariable("BIFROSTCaveCableLow2Right",50);
  Control.addVariable("BIFROSTCaveCableLow2VoidLeft",25);
  Control.addVariable("BIFROSTCaveCableLow2VoidRight",25);
  Control.addVariable("BIFROSTCaveCableLow2VoidDepth",-1.0);
  Control.addVariable("BIFROSTCaveCableLow2Depth",-1.0);
  Control.addVariable("BIFROSTCaveCableLow2VoidHeight",25);
  Control.addVariable("BIFROSTCaveCableLow2Height",50);
  Control.addVariable("BIFROSTCaveCableLow2Spacing",1.0);
  Control.addVariable("BIFROSTCaveCableLow2SpacingMat","LimestoneConcrete");
  Control.addVariable("BIFROSTCaveCableLow2DefMat","LimestoneConcrete");
  //  Control.addVariable("BIFROSTCaveCableLow2B4CTileMat","Lead");
  Control.addVariable("BIFROSTCaveCableLow2InnerB4CLayerThick",1.0);
  Control.addVariable("BIFROSTCaveCableLow2VoidMat","Air");
  /*  Control.addVariable("BIFROSTCaveCableLow2XStep", 3.5);
  Control.addVariable("BIFROSTCaveCableLow2YStep", 88.0);
  */
   Control.addVariable("BIFROSTCaveCableLow2B4CTileMat", "B-Poly");
   Control.addVariable("BIFROSTCaveCableLow2SplitVoid", 0);
   Control.addVariable("BIFROSTCaveCableLow2ZStep", -313.7);
   Control.addVariable("BIFROSTCaveCableLow2XStep", 157);




  Control.addVariable("BIFROSTCaveCableLow3NSeg",10);
  Control.addVariable("BIFROSTCaveCableLow3NWallLayers",3);
  Control.addVariable("BIFROSTCaveCableLow3NRoofLayers",3);
  Control.addVariable("BIFROSTCaveCableLow3NFloorLayers",1);
  Control.addVariable("BIFROSTCaveCableLow3Length",121);
  Control.addVariable("BIFROSTCaveCableLow3Left",50);
  Control.addVariable("BIFROSTCaveCableLow3Right",50);
  Control.addVariable("BIFROSTCaveCableLow3VoidLeft",25);
  Control.addVariable("BIFROSTCaveCableLow3VoidRight",25);
  Control.addVariable("BIFROSTCaveCableLow3VoidDepth",-1);
  Control.addVariable("BIFROSTCaveCableLow3Depth",-1);
  Control.addVariable("BIFROSTCaveCableLow3VoidHeight",25);
  Control.addVariable("BIFROSTCaveCableLow3Height",50);
  Control.addVariable("BIFROSTCaveCableLow3Spacing",1.0);
  Control.addVariable("BIFROSTCaveCableLow3SpacingMat","LimestoneConcrete");
  Control.addVariable("BIFROSTCaveCableLow3DefMat","LimestoneConcrete");
  Control.addVariable("BIFROSTCaveCableLow3VoidMat","Air");
  /*  Control.addVariable("BIFROSTCaveCableLow3XStep", 3.5);
  Control.addVariable("BIFROSTCaveCableLow3YStep", 88.0);
  */
  Control.addVariable("BIFROSTCaveCableLow3InnerB4CLayerThick",1.0);
   Control.addVariable("BIFROSTCaveCableLow3B4CTileMat", "B-Poly");
   Control.addVariable("BIFROSTCaveCableLow3SplitVoid", 0);
   Control.addVariable("BIFROSTCaveCableLow3ZStep", -313.7);
   Control.addVariable("BIFROSTCaveCableLow3XStep", -158.5);


  Control.addVariable("BIFROSTCaveCableLowBlock3NSeg",10);
  Control.addVariable("BIFROSTCaveCableLowBlock3NWallLayers",3);
  Control.addVariable("BIFROSTCaveCableLowBlock3NRoofLayers",3);
  Control.addVariable("BIFROSTCaveCableLowBlock3NFloorLayers",1);
  Control.addVariable("BIFROSTCaveCableLowBlock3Length",100);
  Control.addVariable("BIFROSTCaveCableLowBlock3Left",35);
  Control.addVariable("BIFROSTCaveCableLowBlock3Right",35);
  Control.addVariable("BIFROSTCaveCableLowBlock3VoidLeft",25);
  Control.addVariable("BIFROSTCaveCableLowBlock3VoidRight",25);
  Control.addVariable("BIFROSTCaveCableLowBlock3VoidDepth",-1);
  Control.addVariable("BIFROSTCaveCableLowBlock3Depth",-1);
  Control.addVariable("BIFROSTCaveCableLowBlock3VoidHeight",25);
  Control.addVariable("BIFROSTCaveCableLowBlock3Height",35);
  Control.addVariable("BIFROSTCaveCableLowBlock3Spacing",1.0);
  Control.addVariable("BIFROSTCaveCableLowBlock3SpacingMat","Air");
  Control.addVariable("BIFROSTCaveCableLowBlock3DefMat","Lead");
  Control.addVariable("BIFROSTCaveCableLowBlock3VoidMat","Air");
  // Control.addVariable("BIFROSTCaveCableLowBlock3SplitVoid",1);
  /*  Control.addVariable("BIFROSTCaveCableLowBlock3XStep", 3.5);
  Control.addVariable("BIFROSTCaveCableLowBlock3YStep", 88.0);
  */
  Control.addVariable("BIFROSTCaveCableLowBlock3InnerB4CLayerThick",1.0);
   Control.addVariable("BIFROSTCaveCableLowBlock3B4CTileMat", "B-Poly");
   Control.addVariable("BIFROSTCaveCableLowBlock3SplitVoid", 1);
   Control.addVariable("BIFROSTCaveCableLowBlock3ZStep", 100);
   Control.addVariable("BIFROSTCaveCableLowBlock3YStep", 0);
   Control.addVariable("BIFROSTCaveCableLowBlock3ZAngle", -90);
   //  Control.addVariable("BIFROSTCaveCableLowBlock3XStep", -158.5);



   
  


  Control.addVariable("BIFROSTCavePlatformNSeg",3);
  Control.addVariable("BIFROSTCavePlatformNWallLayers",3);
  Control.addVariable("BIFROSTCavePlatformNRoofLayers",3);
  Control.addVariable("BIFROSTCavePlatformNFloorLayers",1);
  Control.addVariable("BIFROSTCavePlatformLength",10);
  Control.addVariable("BIFROSTCavePlatformLeft",70);
  Control.addVariable("BIFROSTCavePlatformRight",90);
  Control.addVariable("BIFROSTCavePlatformVoidLeft",60);
  Control.addVariable("BIFROSTCavePlatformVoidRight",80);
  Control.addVariable("BIFROSTCavePlatformVoidDepth",58.5);
  Control.addVariable("BIFROSTCavePlatformDepth",58.5);
  Control.addVariable("BIFROSTCavePlatformSpacing",0);
  Control.addVariable("BIFROSTCavePlatformHeight",106.5);
  Control.addVariable("BIFROSTCavePlatformVoidHeight",96.5);
  Control.addVariable("BIFROSTCavePlatformDefMat","Stainless304");
  Control.addVariable("BIFROSTCavePlatformVoidMat","Stainless304");
  /*  Control.addVariable("BIFROSTCavePlatformXStep", 3.5);
  Control.addVariable("BIFROSTCavePlatformYStep", 88.0);
  */
   Control.addVariable("BIFROSTCavePlatformSplitVoid", 1);
   Control.addVariable("BIFROSTCavePlatformZStep", 77.0-22.0);
   Control.addVariable("BIFROSTCavePlatformXStep", 150.0);
  // Control.addVariable("BIFROSTCavePlatformYStep", 88.0);
  Control.addVariable("BIFROSTCavePlatformZAngle", 90.0);
  Control.addVariable("BIFROSTCavePlatformXYAngle", -90.0);



  Control.addVariable("BIFROSTCavePlatformFenceNSeg",3);
  Control.addVariable("BIFROSTCavePlatformFenceNWallLayers",3);
  Control.addVariable("BIFROSTCavePlatformFenceNRoofLayers",3);
  Control.addVariable("BIFROSTCavePlatformFenceNFloorLayers",1);
  Control.addVariable("BIFROSTCavePlatformFenceLength",109);
  Control.addVariable("BIFROSTCavePlatformFenceLeft",71);
  Control.addVariable("BIFROSTCavePlatformFenceRight",91);
  Control.addVariable("BIFROSTCavePlatformFenceVoidLeft",66);
  Control.addVariable("BIFROSTCavePlatformFenceVoidRight",86);
  Control.addVariable("BIFROSTCavePlatformFenceVoidDepth",58.5);
  Control.addVariable("BIFROSTCavePlatformFenceDepth",58.5);
  Control.addVariable("BIFROSTCavePlatformFenceSpacing",0);
  Control.addVariable("BIFROSTCavePlatformFenceHeight",107.5);
  Control.addVariable("BIFROSTCavePlatformFenceVoidHeight",107.5);
  Control.addVariable("BIFROSTCavePlatformFenceDefMat","B-Poly");
  Control.addVariable("BIFROSTCavePlatformFenceVoidMat","Air");
  Control.addVariable("BIFROSTCavePlatformFenceRoofMat","Air");
  /*  Control.addVariable("BIFROSTCavePlatformFenceXStep", 3.5);
  Control.addVariable("BIFROSTCavePlatformFenceYStep", 88.0);
  */
   Control.addVariable("BIFROSTCavePlatformFenceSplitVoid", 0);
  // Control.addVariable("BIFROSTCavePlatformFenceZStep", 77.0-22.0);
  // Control.addVariable("BIFROSTCavePlatformFenceXStep", 150.0);
  // Control.addVariable("BIFROSTCavePlatformFenceYStep", 88.0);
  // Control.addVariable("BIFROSTCavePlatformFenceZAngle", 90.0);
  // Control.addVariable("BIFROSTCavePlatformFenceXYAngle", -90.0);




  
  Control.addVariable("BIFROSTCavePlatformB4CNSeg",1);
  Control.addVariable("BIFROSTCavePlatformB4CNWallLayers",2);
  Control.addVariable("BIFROSTCavePlatformB4CNRoofLayers",2);
  Control.addVariable("BIFROSTCavePlatformB4CNFloorLayers",1);
  Control.addVariable("BIFROSTCavePlatformB4CLength",12);
  Control.addVariable("BIFROSTCavePlatformB4CLeft",71);
  Control.addVariable("BIFROSTCavePlatformB4CRight",91);
  Control.addVariable("BIFROSTCavePlatformB4CVoidLeft",70);
  Control.addVariable("BIFROSTCavePlatformB4CVoidRight",90);
  Control.addVariable("BIFROSTCavePlatformB4CVoidDepth",58.5);
  Control.addVariable("BIFROSTCavePlatformB4CDepth",58.5);
  Control.addVariable("BIFROSTCavePlatformB4CSpacing",0);
  Control.addVariable("BIFROSTCavePlatformB4CHeight",107.5);
  Control.addVariable("BIFROSTCavePlatformB4CVoidHeight",106.5);
  Control.addVariable("BIFROSTCavePlatformB4CDefMat","B4CConcrete");
  Control.addVariable("BIFROSTCavePlatformB4CVoidMat","B4CConcrete");
  /*  Control.addVariable("BIFROSTCavePlatformB4CXStep", 3.5);
  Control.addVariable("BIFROSTCavePlatformB4CYStep", 88.0);
  */
   Control.addVariable("BIFROSTCavePlatformB4CSplitVoid", 1);
   Control.addVariable("BIFROSTCavePlatformB4CZStep", 77.0-23.0);
   Control.addVariable("BIFROSTCavePlatformB4CXStep", 150.0);
  // Control.addVariable("BIFROSTCavePlatformB4CYStep", 88.0);
  Control.addVariable("BIFROSTCavePlatformB4CZAngle", 90.0);
  Control.addVariable("BIFROSTCavePlatformB4CXYAngle", -90.0);


  
  /*
  Control.addVariable("BIFROSTCavePlatformB4CNSeg",3);
  Control.addVariable("BIFROSTCavePlatformB4CNWallLayers",3);
  Control.addVariable("BIFROSTCavePlatformB4CNRoofLayers",3);
  Control.addVariable("BIFROSTCavePlatformB4CNFloorLayers",1);
  Control.addVariable("BIFROSTCavePlatformB4CLength",1);
  Control.addVariable("BIFROSTCavePlatformB4CRight",70);
  Control.addVariable("BIFROSTCavePlatformB4CLeft",90);
  Control.addVariable("BIFROSTCavePlatformB4CVoidRight",60);
  Control.addVariable("BIFROSTCavePlatformB4CVoidLeft",80);
  Control.addVariable("BIFROSTCavePlatformB4CVoidDepth",58.5);
  Control.addVariable("BIFROSTCavePlatformB4CDepth",58.5);
  Control.addVariable("BIFROSTCavePlatformB4CSpacing",0);
  Control.addVariable("BIFROSTCavePlatformB4CHeight",106.5);
  Control.addVariable("BIFROSTCavePlatformB4CVoidHeight",96.5);
  Control.addVariable("BIFROSTCavePlatformB4CDefMat","B4CConcrete");
  Control.addVariable("BIFROSTCavePlatformB4CVoidMat","B4CConcrete");
  */
  /*  Control.addVariable("BIFROSTCavePlatformB4CXStep", 3.5);
  Control.addVariable("BIFROSTCavePlatformB4CYStep", 88.0);
  */
   Control.addVariable("BIFROSTCavePlatformB4CSplitVoid", 1);

  

  //Change LR ?

  
  //Sgen: length, 0.5*width, height, depth, seg along, seg walls  
  SGen.generateShield(Control,"BIFROSTCaveBeamstopDoughnut2",57.0, 140, 200, 10, 6,3);
  Control.addVariable("BIFROSTCaveBeamstopDoughnut2Left",70);
  Control.addVariable("BIFROSTCaveBeamstopDoughnut2Right",70);
  Control.addVariable("BIFROSTCaveBeamstopDoughnut2Height",70);
  Control.addVariable("BIFROSTCaveBeamstopDoughnut2Spacing",0);
  Control.addVariable("BIFROSTCaveBeamstopDoughnut2Depth",70);
  Control.addVariable("BIFROSTCaveBeamstopDoughnut2Front",60);
  Control.addVariable("BIFROSTCaveBeamstopDoughnut2VoidLeft",40);
  Control.addVariable("BIFROSTCaveBeamstopDoughnut2VoidRight",40);
  Control.addVariable("BIFROSTCaveBeamstopDoughnut2VoidHeight",40);
  Control.addVariable("BIFROSTCaveBeamstopDoughnut2VoidDepth",40);
  Control.addVariable("BIFROSTCaveBeamstopDoughnut2YStep",0);
  Control.addVariable("BIFROSTCaveBeamstopDoughnut2DefMat","LimestoneConcrete");
  Control.addVariable("BIFROSTCaveBeamstopDoughnut2VoidMat","Air");
  //  Control.addVariable("BIFROSTCaveBeamstopDoughnut2NFrontLayers",2);
  Control.addVariable("BIFROSTCaveBeamstopDoughnut2NWallLayers",3);
  Control.addVariable("BIFROSTCaveBeamstopDoughnut2NRoofLayers",3);
  Control.addVariable("BIFROSTCaveBeamstopDoughnut2NFloorLayers",3);
  Control.addVariable("DefaultTileThick",0.0);

  SGen.generateShield(Control,"BIFROSTCaveBeamstopDoughnut",43.0, 140, 200, 10, 5,5);
  Control.addVariable("BIFROSTCaveBeamstopDoughnutLeft",70);
  Control.addVariable("BIFROSTCaveBeamstopDoughnutRight",70);
  Control.addVariable("BIFROSTCaveBeamstopDoughnutHeight",70);
  Control.addVariable("BIFROSTCaveBeamstopDoughnutSpacing",0);
  Control.addVariable("BIFROSTCaveBeamstopDoughnutDepth",70);
  Control.addVariable("BIFROSTCaveBeamstopDoughnutVoidLeft",22);
  Control.addVariable("BIFROSTCaveBeamstopDoughnutVoidRight",22);
  Control.addVariable("BIFROSTCaveBeamstopDoughnutVoidHeight",22);
  Control.addVariable("BIFROSTCaveBeamstopDoughnutVoidDepth",22);
  Control.addVariable("BIFROSTCaveBeamstopDoughnutYStep",1);
  Control.addVariable("BIFROSTCaveBeamstopDoughnutDefMat","LimestoneConcrete");
  Control.addVariable("BIFROSTCaveBeamstopDoughnutVoidMat","Air");
  Control.addVariable("DefaultTileThick",0.0);





  
  SGen.generateShield(Control,"BIFROSTCaveSampleLabyrinth",280.0, 140, 160, 10, 1,8);
  Control.addVariable("BIFROSTCaveSampleLabyrinthLeft",280);
  Control.addVariable("BIFROSTCaveSampleLabyrinthRight",220);
  Control.addVariable("BIFROSTCaveSampleLabyrinthHeight",376.3);//80cm thick roof
  Control.addVariable("BIFROSTCaveSampleLabyrinthDepth",313.7);
  Control.addVariable("BIFROSTCaveSampleLabyrinthFront",200);
  Control.addVariable("BIFROSTCaveSampleLabyrinthXStep",150);
  Control.addVariable("BIFROSTCaveSampleLabyrinthVoidLeft",220);
  Control.addVariable("BIFROSTCaveSampleLabyrinthVoidRight",160);
  Control.addVariable("BIFROSTCaveSampleLabyrinthSpacing",0.0);
  Control.addVariable("BIFROSTCaveSampleLabyrinthVoidHeight",296.3);
  Control.addVariable("BIFROSTCaveSampleLabyrinthHeight",376.3);
  Control.addVariable("BIFROSTCaveSampleLabyrinthVoidDepth",313.7);
  Control.addVariable("BIFROSTCaveSampleLabyrinthVoidFront",140);
  Control.addVariable("BIFROSTCaveSampleLabyrinthDefMat","LimestoneConcrete");
  Control.addVariable("BIFROSTCaveSampleLabyrinthNWallLayers",6);
  Control.addVariable("BIFROSTCaveSampleLabyrinthNRoofLayers",8);
  Control.addVariable("BIFROSTCaveSampleLabyrinthNFloorLayers",2);
  Control.addVariable("BIFROSTCaveSampleLabyrinthVoidMat","Air");
        Control.addVariable("BIFROSTCaveSampleLabyrinthB4CTileMat","Air");
        Control.addVariable("BIFROSTCaveSampleLabyrinthInnerB4CLayerThick",0.0);
  Control.addVariable("DefaultTileThick",0.0);




  
  // SGen.generateShield(Control,"BIFROSTCaveSampleLabyrinthB4C",280.0, 140, 160, 10, 1,8);
  Control.addVariable("BIFROSTCaveSampleLabyrinthB4CLeft",280);
  Control.addVariable("BIFROSTCaveSampleLabyrinthB4CRight",190);
   Control.addVariable("BIFROSTCaveSampleLabyrinthB4CDepth",0);
  Control.addVariable("BIFROSTCaveSampleLabyrinthB4CFront",140);
  Control.addVariable("BIFROSTCaveSampleLabyrinthB4CXStep",210);
  Control.addVariable("BIFROSTCaveSampleLabyrinthB4CVoidLeft",279);
  Control.addVariable("BIFROSTCaveSampleLabyrinthB4CVoidRight",189);
  Control.addVariable("BIFROSTCaveSampleLabyrinthB4CSpacing",0.0);
  Control.addVariable("BIFROSTCaveSampleLabyrinthB4CVoidHeight",235.3);
  Control.addVariable("BIFROSTCaveSampleLabyrinthB4CHeight",236.3);
  Control.addVariable("BIFROSTCaveSampleLabyrinthB4CZStep",60);
  Control.addVariable("BIFROSTCaveSampleLabyrinthB4CVoidDepth",0);
  Control.addVariable("BIFROSTCaveSampleLabyrinthB4CVoidFront",139);
  Control.addVariable("BIFROSTCaveSampleLabyrinthB4CDefMat","B4CConcrete");
  Control.addVariable("BIFROSTCaveSampleLabyrinthB4CNWallLayers",2);
  Control.addVariable("BIFROSTCaveSampleLabyrinthB4CNRoofLayers",2);
  Control.addVariable("BIFROSTCaveSampleLabyrinthB4CNFloorLayers",1);
  Control.addVariable("BIFROSTCaveSampleLabyrinthB4CVoidMat","Air");
        Control.addVariable("BIFROSTCaveSampleLabyrinthB4CB4CTileMat","Air");
        Control.addVariable("BIFROSTCaveSampleLabyrinthB4CInnerB4CLayerThick",0.0);
  Control.addVariable("DefaultTileThick",0.0);





  
  

    SGen.generateShield(Control,"BIFROSTCaveSampleCorridor",300.0, 140, 160, 10, 1,8);
  Control.addVariable("BIFROSTCaveSampleCorridorLeft",200);
  Control.addVariable("BIFROSTCaveSampleCorridorRight",60);
  Control.addVariable("BIFROSTCaveSampleCorridorHeight",336.3);//40cm thick roof
  Control.addVariable("BIFROSTCaveSampleCorridorDepth",313.7);
  //  Control.addVariable("BIFROSTCaveSampleCorridorXStep",150);
  Control.addVariable("BIFROSTCaveSampleCorridorVoidLeft",140);
  Control.addVariable("BIFROSTCaveSampleCorridorVoidRight",0);
  Control.addVariable("BIFROSTCaveSampleCorridorSpacing",0.0);
  Control.addVariable("BIFROSTCaveSampleCorridorVoidHeight",296.3);
  Control.addVariable("BIFROSTCaveSampleCorridorHeight",336.3);
  Control.addVariable("BIFROSTCaveSampleCorridorVoidDepth",313.7);
   Control.addVariable("BIFROSTCaveSampleCorridorDefMat","LimestoneConcrete");
  Control.addVariable("BIFROSTCaveSampleCorridorNWallLayers",6);
  Control.addVariable("BIFROSTCaveSampleCorridorNRoofLayers",4);
  Control.addVariable("BIFROSTCaveSampleCorridorNFloorLayers",2);
  Control.addVariable("BIFROSTCaveSampleCorridorVoidMat","Air");
        Control.addVariable("BIFROSTCaveSampleCorridorB4CTileMat","Air");
        Control.addVariable("BIFROSTCaveSampleCorridorInnerB4CLayerThick",1.0);
  Control.addVariable("DefaultTileThick",0.0);



  FGen.setLayer(1,1.0,"Void"); 
  FGen.setLayer(2,0.0,"Void");
  FGen.setLayer(3,0.0,"Void");
 
  /* FGen.generateTaper(Control,"BIFROSTCaveDetAccess",121.1,
                         150,150,295,295);
  */

  SGen.generateShield(Control,"BIFROSTCaveDetAccess",120.0, 140, 160, 10, 1,12);
  Control.addVariable("BIFROSTCaveDetAccessRight",155); 
  Control.addVariable("BIFROSTCaveDetAccessNSeg",8); 
  Control.addVariable("BIFROSTCaveDetAccessLeft",95);
  Control.addVariable("BIFROSTCaveDetAccessHeight",250);
  Control.addVariable("BIFROSTCaveDetAccessDepth",0.0);
  // Control.addVariable("BIFROSTCaveDetAccessFront",230);
  //   Control.addVariable("BIFROSTCaveDetAccessYStep",-60.0);
    Control.addVariable("BIFROSTCaveDetAccessVoidLeft",65);
  Control.addVariable("BIFROSTCaveDetAccessVoidRight",65);
  Control.addVariable("BIFROSTCaveDetAccessSpacing",0.0);
  Control.addVariable("BIFROSTCaveDetAccessVoidHeight",220.0);
  Control.addVariable("BIFROSTCaveDetAccessVoidDepth",0.0);
  Control.addVariable("BIFROSTCaveDetAccessLength",120);
  Control.addVariable("BIFROSTCaveDetAccessDefMat","LimestoneConcrete");
  Control.addVariable("BIFROSTCaveDetAccessNWallLayersL",3);
  Control.addVariable("BIFROSTCaveDetAccessNWallLayersR",6);
  Control.addVariable("BIFROSTCaveDetAccessNRoofLayers",4);
  Control.addVariable("BIFROSTCaveDetAccessNFloorLayers",1);
  Control.addVariable("BIFROSTCaveDetAccessVoidMat","Air");
  Control.addVariable("BIFROSTCaveDetAccessB4CTileMat","B4CConcrete");
  Control.addVariable("BIFROSTCaveDetAccessInnerB4CLayerThick",1.0);
  //Control.addVariable("BIFROSTCaveDetAccessZStep",53.6);
  Control.addVariable("BIFROSTCaveDetAccessZStep",-313.7);
  Control.addVariable("BIFROSTCaveDetAccessXStep",330);
  //   Control.addVariable("BIFROSTCaveDetAccessYStep",-121);
  Control.addVariable("BIFROSTCaveDetAccessLayerMat0","Air");

  

  FGen.generateTaper(Control,"BIFROSTCaveDetAccessOuter",82.5,
                         150,150,220,220);
    Control.addVariable("BIFROSTCaveDetAccessOuterZStep",110.0);
    Control.addVariable("BIFROSTCaveDetAccessOuterYStep",-1.2);
    Control.addVariable("BIFROSTCaveDetAccessOuterXStep",-280);
  Control.addVariable("BIFROSTCaveDetAccessOuterLayerMat0","Air");
Control.addVariable("BIFROSTCaveDetAccessOuterLayerMat1","Void");

  
  
  SGen.generateShield(Control,"BIFROSTCaveDetectorLabyrinth",280.0, 140, 160, 10, 1,12);
  Control.addVariable("BIFROSTCaveDetectorLabyrinthLeft",445);
  Control.addVariable("BIFROSTCaveDetectorLabyrinthRight",515);
  Control.addVariable("BIFROSTCaveDetectorLabyrinthHeight",260);
  Control.addVariable("BIFROSTCaveDetectorLabyrinthDepth",0);
  Control.addVariable("BIFROSTCaveDetectorLabyrinthFront",210);
  Control.addVariable("BIFROSTCaveDetectorLabyrinthZStep",-313.7);
  Control.addVariable("BIFROSTCaveDetectorLabyrinthYStep",-105.);
  Control.addVariable("BIFROSTCaveDetectorLabyrinthVoidLeft",385);
  Control.addVariable("BIFROSTCaveDetectorLabyrinthVoidRight",395);
  Control.addVariable("BIFROSTCaveDetectorLabyrinthSpacing",0.0);
  Control.addVariable("BIFROSTCaveDetectorLabyrinthVoidHeight",220);
  Control.addVariable("BIFROSTCaveDetectorLabyrinthVoidDepth",0);
  Control.addVariable("BIFROSTCaveDetectorLabyrinthVoidFront",150);
  Control.addVariable("BIFROSTCaveDetectorLabyrinthDefMat","LimestoneConcrete");
  Control.addVariable("BIFROSTCaveDetectorLabyrinthNWallLayers",8);
  Control.addVariable("BIFROSTCaveDetectorLabyrinthNRoofLayers",7);
  Control.addVariable("BIFROSTCaveDetectorLabyrinthNFloorLayers",2);
  Control.addVariable("BIFROSTCaveDetectorLabyrinthVoidMat","Air");
  //Control.addVariable("BIFROSTCaveDetectorLabyrinthB4CTileMat","B4CConcrete");
  //  Control.addVariable("BIFROSTCaveDetectorLabyrinthInnerB4CLayerThick",01.0);


  //  SGen.generateShield(Control,
  //		      "BIFROSTCaveDetectorLabyrinthB4C",280.0, 140, 160, 10, 1,12);
  Control.addVariable("BIFROSTCaveDetectorLabyrinthB4CDepth",0);
  Control.addVariable("BIFROSTCaveDetectorLabyrinthB4CFront",210);
  Control.addVariable("BIFROSTCaveDetectorLabyrinthB4CZStep",-313.7);
  Control.addVariable("BIFROSTCaveDetectorLabyrinthB4CXStep",140.0);
 // Control.addVariable("BIFROSTCaveDetectorLabyrinthB4CYStep",-105.);
    Control.addVariable("BIFROSTCaveDetectorLabyrinthB4CVoidLeft",384);
  Control.addVariable("BIFROSTCaveDetectorLabyrinthB4CVoidRight",254);
  Control.addVariable("BIFROSTCaveDetectorLabyrinthB4CLeft",-385);
  Control.addVariable("BIFROSTCaveDetectorLabyrinthB4CRight",255);
  Control.addVariable("BIFROSTCaveDetectorLabyrinthB4CSpacing",0.0);
  Control.addVariable("BIFROSTCaveDetectorLabyrinthB4CHeight",220);
  Control.addVariable("BIFROSTCaveDetectorLabyrinthB4CVoidHeight",219);
  Control.addVariable("BIFROSTCaveDetectorLabyrinthB4CVoidDepth",0);
  Control.addVariable("BIFROSTCaveDetectorLabyrinthB4CFront",150);
  Control.addVariable("BIFROSTCaveDetectorLabyrinthB4CVoidFront",149);
  //  Control.addVariable("BIFROSTCaveDetectorLabyrinthB4CDefMat","B4CConcrete");
    Control.addVariable("BIFROSTCaveDetectorLabyrinthB4CDefMat","B-Poly");
  Control.addVariable("BIFROSTCaveDetectorLabyrinthB4CNWallLayers",2);
  Control.addVariable("BIFROSTCaveDetectorLabyrinthB4CNRoofLayers",2);
  Control.addVariable("BIFROSTCaveDetectorLabyrinthB4CNFloorLayers",1);
  Control.addVariable("BIFROSTCaveDetectorLabyrinthB4CVoidMat","Air");



  //  SGen.generateShield(Control,
  //		      "BIFROSTCaveDetectorLabyrinthB4Cexit",280.0, 140, 160, 10, 1,12);
  Control.addVariable("BIFROSTCaveDetectorLabyrinthB4CexitDepth",0);
  Control.addVariable("BIFROSTCaveDetectorLabyrinthB4CexitFront",210);
  Control.addVariable("BIFROSTCaveDetectorLabyrinthB4CexitZStep",0);
  Control.addVariable("BIFROSTCaveDetectorLabyrinthB4CexitXStep",184.0);
 // Control.addVariable("BIFROSTCaveDetectorLabyrinthB4CexitYStep",-105.);
    Control.addVariable("BIFROSTCaveDetectorLabyrinthB4CexitVoidLeft",384);
  Control.addVariable("BIFROSTCaveDetectorLabyrinthB4CexitVoidRight",200);
  Control.addVariable("BIFROSTCaveDetectorLabyrinthB4CexitLeft",-385);
  Control.addVariable("BIFROSTCaveDetectorLabyrinthB4CexitRight",201);
  Control.addVariable("BIFROSTCaveDetectorLabyrinthB4CexitSpacing",0.0);
  Control.addVariable("BIFROSTCaveDetectorLabyrinthB4CexitHeight",220);
  Control.addVariable("BIFROSTCaveDetectorLabyrinthB4CexitVoidHeight",219);
  Control.addVariable("BIFROSTCaveDetectorLabyrinthB4CexitVoidDepth",0);
  Control.addVariable("BIFROSTCaveDetectorLabyrinthB4CexitFront",150);
  Control.addVariable("BIFROSTCaveDetectorLabyrinthB4CexitVoidFront",149);
  //  Control.addVariable("BIFROSTCaveDetectorLabyrinthB4CexitDefMat","B4CConcrete");
    Control.addVariable("BIFROSTCaveDetectorLabyrinthB4CexitDefMat","B-Poly");
  Control.addVariable("BIFROSTCaveDetectorLabyrinthB4CexitNWallLayers",2);
  Control.addVariable("BIFROSTCaveDetectorLabyrinthB4CexitNRoofLayers",2);
  Control.addVariable("BIFROSTCaveDetectorLabyrinthB4CexitNFloorLayers",1);
  Control.addVariable("BIFROSTCaveDetectorLabyrinthB4CexitVoidMat","Air");


  
  
  Control.addVariable("DefaultTileThick",0.0);



  
  //Sgen: length, 0.5*width, height, depth, seg along, seg walls
  // in the drawings it is 105 for the length
  SGen.generateShield(Control,"BIFROSTCaveUpperBlockWall",120.0, 140, 115, 180, 4,4);
  Control.addVariable("BIFROSTCaveUpperBlockWallVoidLeft",90.0);
  Control.addVariable("BIFROSTCaveUpperBlockWallNRoofLayers",8);
    Control.addVariable("BIFROSTCaveUpperBlockWallVoidRight",70.0);
  Control.addVariable("BIFROSTCaveUpperBlockWallLeft",150.0);
    Control.addVariable("BIFROSTCaveUpperBlockWallRight",130.0);
      Control.addVariable("BIFROSTCaveUpperBlockWallVoidHeight",30);
      Control.addVariable("BIFROSTCaveUpperBlockWallHeight",110);
        Control.addVariable("BIFROSTCaveUpperBlockWallVoidDepth",179);
       Control.addVariable("BIFROSTCaveUpperBlockWallSpacing",0.0);
       Control.addVariable("BIFROSTCaveUpperBlockWallSpacingMat","LimestoneConcrete");
       Control.addVariable("BIFROSTCaveUpperBlockWallNFloorLayers",1);
       Control.addVariable("BIFROSTCaveUpperBlockWallInnerB4CLayerThick",1.0);
       Control.addVariable("BIFROSTCaveUpperBlockWallB4CTileMat","B4CConcrete");
       //	     	Control.addVariable("BIFROSTCaveUpperBlockWallPreZAngle",-90.0);
      Control.addVariable("BIFROSTCaveUpperBlockWallZStep",266.3);
      Control.addVariable("BIFROSTCaveUpperBlockWallDefMat","LimestoneConcrete");
      Control.addVariable("BIFROSTCaveUpperBlockWallVoidMat","Air");

      
	
SGen.generateShield(Control,"BIFROSTCaveUpperBlockRoof1",30.0, 140, 115, 180, 1,8);
        Control.addVariable("BIFROSTCaveUpperBlockRoof1NWallLayersL",6);
        Control.addVariable("BIFROSTCaveUpperBlockRoof1NWallLayersR",8);
  Control.addVariable("BIFROSTCaveUpperBlockRoof1NRoofLayers",4);
      Control.addVariable("BIFROSTCaveUpperBlockRoof1DefMat","LimestoneConcrete");
  Control.addVariable("BIFROSTCaveUpperBlockRoof1VoidLeft",90.0);
    Control.addVariable("BIFROSTCaveUpperBlockRoof1VoidRight",70.0);
    Control.addVariable("BIFROSTCaveUpperBlockRoof1Left",200.0);
    Control.addVariable("BIFROSTCaveUpperBlockRoof1Right",225.0);
      Control.addVariable("BIFROSTCaveUpperBlockRoof1VoidHeight",30);
      Control.addVariable("BIFROSTCaveUpperBlockRoof1Height",60);
        Control.addVariable("BIFROSTCaveUpperBlockRoof1VoidDepth",0);
        Control.addVariable("BIFROSTCaveUpperBlockRoof1Depth",0);
       Control.addVariable("BIFROSTCaveUpperBlockRoof1Spacing",0);
       Control.addVariable("BIFROSTCaveUpperBlockRoof1ZStep",0);
       Control.addVariable("BIFROSTCaveUpperBlockRoof1InnerB4CLayerThick",1.0);
       Control.addVariable("BIFROSTCaveUpperBlockRoof1B4CTileMat","B4CConcrete");
      Control.addVariable("BIFROSTCaveUpperBlockRoof1VoidMat","Air");

      //Reduce length of the block by thickness of borated lining of the cave!!!
      
      SGen.generateShield(Control,"BIFROSTCaveUpperBlockRoof2",40.0, 140, 115, 0, 1,8);
      Control.addVariable("BIFROSTCaveUpperBlockRoof2DefMat","LimestoneConcrete");
  Control.addVariable("BIFROSTCaveUpperBlockRoof2VoidLeft",90.0);
  Control.addVariable("BIFROSTCaveUpperBlockRoof2NWallLayersR",3);
  Control.addVariable("BIFROSTCaveUpperBlockRoof2NWallLayersL",7);
  Control.addVariable("BIFROSTCaveUpperBlockRoof2NRoofLayers",4);
    Control.addVariable("BIFROSTCaveUpperBlockRoof2VoidRight",195.0);
  Control.addVariable("BIFROSTCaveUpperBlockRoof2Left",200.0);
    Control.addVariable("BIFROSTCaveUpperBlockRoof2Right",225.0);
    Control.addVariable("BIFROSTCaveUpperBlockRoof2VoidHeight",30);
    Control.addVariable("BIFROSTCaveUpperBlockRoof2Height",60);
   
             Control.addVariable("BIFROSTCaveUpperBlockRoof2VoidDepth",0);
             Control.addVariable("BIFROSTCaveUpperBlockRoof2Depth",0);
            Control.addVariable("BIFROSTCaveUpperBlockRoof2Spacing",0.0);
       Control.addVariable("BIFROSTCaveUpperBlockRoof2SpacingMat","B4C");
       Control.addVariable("BIFROSTCaveUpperBlockRoof2InnerB4CLayerThick",1.0);
       Control.addVariable("BIFROSTCaveUpperBlockRoof2B4CTileMat","B4CConcrete");
       //       Control.addVariable("BIFROSTCaveUpperBlockRoof2B4CTileMat","B-Poly");
       //        Control.addVariable("BIFROSTCaveUpperBlockRoof2B4CTileMat","Air");
      Control.addVariable("BIFROSTCaveUpperBlockRoof2VoidMat","Air");
      Control.addVariable("BIFROSTCaveUpperBlockRoof2ZStep",0);



    SGen.generateShield(Control,"BIFROSTCaveUpperBlockHatch",45.0, 140, 89.9, 0, 1,3);
    Control.addVariable("BIFROSTCaveUpperBlockHatchDefMat","LimestoneConcrete");
  Control.addVariable("BIFROSTCaveUpperBlockHatchVoidLeft",170);
    Control.addVariable("BIFROSTCaveUpperBlockHatchVoidRight",195);
    Control.addVariable("BIFROSTCaveUpperBlockHatchLeft",200); // 17.5cm displ. from 150cm
    Control.addVariable("BIFROSTCaveUpperBlockHatchRight",225);
          Control.addVariable("BIFROSTCaveUpperBlockHatchHeight",60);
          Control.addVariable("BIFROSTCaveUpperBlockHatchVoidHeight",30);
	  //          Control.addVariable("BIFROSTCaveUpperBlockHatchRoofThick",53.5);
        Control.addVariable("BIFROSTCaveUpperBlockHatchNRoofLayers",4);
        Control.addVariable("BIFROSTCaveUpperBlockHatchVoidDepth",0);
       Control.addVariable("BIFROSTCaveUpperBlockHatchSpacing",0);
       Control.addVariable("BIFROSTCaveUpperBlockHatchInnerB4CLayerThick",1.0);
       Control.addVariable("BIFROSTCaveUpperBlockHatchB4CTileMat","B4CConcrete");
      Control.addVariable("BIFROSTCaveUpperBlockHatchVoidMat","Air");
            Control.addVariable("BIFROSTCaveUpperBlockHatchZStep",0);


	    /*
    SGen.generateShield(Control,"BIFROSTCaveUpperBlockHatchB4C",45.0, 140, 89.9, 0, 1,3);
    Control.addVariable("BIFROSTCaveUpperBlockHatchB4CDefMat","LimestoneConcrete");
  Control.addVariable("BIFROSTCaveUpperBlockHatchB4CVoidLeft",170);
    Control.addVariable("BIFROSTCaveUpperBlockHatchB4CVoidRight",195);
    Control.addVariable("BIFROSTCaveUpperBlockHatchB4CLeft",200); // 17.5cm displ. from 150cm
    Control.addVariable("BIFROSTCaveUpperBlockHatchB4CRight",225);
          Control.addVariable("BIFROSTCaveUpperBlockHatchB4CHeight",60);
          Control.addVariable("BIFROSTCaveUpperBlockHatchB4CVoidHeight",30);
	  //          Control.addVariable("BIFROSTCaveUpperBlockHatchB4CRoofThick",53.5);
        Control.addVariable("BIFROSTCaveUpperBlockHatchB4CNRoofLayers",4);
        Control.addVariable("BIFROSTCaveUpperBlockHatchB4CVoidDepth",0);
       Control.addVariable("BIFROSTCaveUpperBlockHatchB4CSpacing",0);
       Control.addVariable("BIFROSTCaveUpperBlockHatchB4CInnerB4CLayerThick",0.0);
      Control.addVariable("BIFROSTCaveUpperBlockHatchB4CVoidMat","Air");
            Control.addVariable("BIFROSTCaveUpperBlockHatchB4CZStep",0);

	    */

    SGen.generateShield(Control,"BIFROSTCaveUpperBlockHatch2",115.0, 140, 89.9, 0, 1,3);
    Control.addVariable("BIFROSTCaveUpperBlockHatch2DefMat","LimestoneConcrete");
  Control.addVariable("BIFROSTCaveUpperBlockHatch2VoidLeft",90);
    Control.addVariable("BIFROSTCaveUpperBlockHatch2VoidRight",70);
    Control.addVariable("BIFROSTCaveUpperBlockHatch2Left",150); // 17.5cm displ. from 150cm
    Control.addVariable("BIFROSTCaveUpperBlockHatch2Right",130);
          Control.addVariable("BIFROSTCaveUpperBlockHatch2Height",50);
          Control.addVariable("BIFROSTCaveUpperBlockHatch2VoidHeight",10);
	  //          Control.addVariable("BIFROSTCaveUpperBlockHatch2RoofThick",53.5);
        Control.addVariable("BIFROSTCaveUpperBlockHatch2NRoofLayers",4);
        Control.addVariable("BIFROSTCaveUpperBlockHatch2VoidDepth",0);
       Control.addVariable("BIFROSTCaveUpperBlockHatch2Spacing",0);
       Control.addVariable("BIFROSTCaveUpperBlockHatch2InnerB4CLayerThick",0.0);
      Control.addVariable("BIFROSTCaveUpperBlockHatch2VoidMat","LimestoneConcrete");
            Control.addVariable("BIFROSTCaveUpperBlockHatch2ZStep",60);
      


    //  SGen.generateShield(Control,"BIFROSTCaveRoofCableB4C1",25.0, 240, 25.1, 0, 1,2);
   Control.addVariable("BIFROSTCaveRoofCableB4C1Right",-50);
   Control.addVariable("BIFROSTCaveRoofCableB4C1Left",125.0);
  Control.addVariable("BIFROSTCaveRoofCableB4C1VoidLeft",124);
    Control.addVariable("BIFROSTCaveRoofCableB4C1VoidRight",25);
      Control.addVariable("BIFROSTCaveRoofCableB4C1VoidHeight",28);
      Control.addVariable("BIFROSTCaveRoofCableB4C1Height",29);
        Control.addVariable("BIFROSTCaveRoofCableB4C1Depth",1);
        Control.addVariable("BIFROSTCaveRoofCableB4C1VoidDepth",0);
       Control.addVariable("BIFROSTCaveRoofCableB4C1Spacing",0.0);
       Control.addVariable("BIFROSTCaveRoofCableB4C1SpacingMat","B4C");
       Control.addVariable("BIFROSTCaveRoofCableB4C1InnerB4CLayerThick",0.0);
       Control.addVariable("BIFROSTCaveRoofCableB4C1NWallLayers",2);
       Control.addVariable("BIFROSTCaveRoofCableB4C1NRoofLayers",2);
       Control.addVariable("BIFROSTCaveRoofCableB4C1NFloorLayers",2);
       //       Control.addVariable("BIFROSTCaveRoofCableB4C1B4CTileMat","B-Poly");
       //       Control.addVariable("BIFROSTCaveRoofCableB4C1DefMat","B-Poly");
       Control.addVariable("BIFROSTCaveRoofCableB4C1B4CTileMat","Air");
       Control.addVariable("BIFROSTCaveRoofCableB4C1DefMat","B4CConcrete");
  Control.addVariable("BIFROSTCaveRoofCableB4C1VoidFront",84);
  Control.addVariable("BIFROSTCaveRoofCableB4C1Front",85);
    Control.addVariable("BIFROSTCaveRoofCableB4C1YStep",0.0);
    Control.addVariable("BIFROSTCaveRoofCableB4C1ZStep",1.0);
    Control.addVariable("BIFROSTCaveRoofCableB4C1XStep",-70.0);
    Control.addVariable("BIFROSTCaveRoofCableB4C1VoidMat","Air");
    
   Control.addVariable("BIFROSTCaveRoofCableB4C2Right",80);
   Control.addVariable("BIFROSTCaveRoofCableB4C2Left",-120.0);
  Control.addVariable("BIFROSTCaveRoofCableB4C2VoidLeft",119);
    Control.addVariable("BIFROSTCaveRoofCableB4C2VoidRight",79);
      Control.addVariable("BIFROSTCaveRoofCableB4C2VoidHeight",28);
      Control.addVariable("BIFROSTCaveRoofCableB4C2Height",29);
        Control.addVariable("BIFROSTCaveRoofCableB4C2Depth",1);
        Control.addVariable("BIFROSTCaveRoofCableB4C2VoidDepth",0);
       Control.addVariable("BIFROSTCaveRoofCableB4C2Spacing",0.0);
       Control.addVariable("BIFROSTCaveRoofCableB4C2SpacingMat","B4C");
       Control.addVariable("BIFROSTCaveRoofCableB4C2InnerB4CLayerThick",0.0);
       Control.addVariable("BIFROSTCaveRoofCableB4C2NWallLayers",2);
       Control.addVariable("BIFROSTCaveRoofCableB4C2NRoofLayers",2);
       Control.addVariable("BIFROSTCaveRoofCableB4C2NFloorLayers",2);
       //       Control.addVariable("BIFROSTCaveRoofCableB4C2B4CTileMat","B-Poly");
       //       Control.addVariable("BIFROSTCaveRoofCableB4C2DefMat","B-Poly");
       Control.addVariable("BIFROSTCaveRoofCableB4C2B4CTileMat","Air");
       Control.addVariable("BIFROSTCaveRoofCableB4C2DefMat","B4CConcrete");
  Control.addVariable("BIFROSTCaveRoofCableB4C2VoidFront",44);
  Control.addVariable("BIFROSTCaveRoofCableB4C2Front",45);
    Control.addVariable("BIFROSTCaveRoofCableB4C2YStep",0.0);
   Control.addVariable("BIFROSTCaveRoofCableB4C2ZStep",1.0);
    Control.addVariable("BIFROSTCaveRoofCableB4C2XStep",90.0);
    Control.addVariable("BIFROSTCaveRoofCableB4C2VoidMat","Air");





	    

  SGen.generateShield(Control,"BIFROSTCaveHatch",81.0, 120, 40, 100, 10,2);
  Control.addVariable("BIFROSTCaveHatchSpacing",0.0);
  Control.addVariable("BIFROSTCaveHatchVoidLeft",90);
  Control.addVariable("BIFROSTCaveHatchVoidRight",70);
  Control.addVariable("BIFROSTCaveHatchLeft",130);//,95);
  Control.addVariable("BIFROSTCaveHatchNWallLayersL",3);//,95);
  Control.addVariable("BIFROSTCaveHatchNWallLayersR",3);//,95);
  Control.addVariable("BIFROSTCaveHatchRight",110);//75);
  Control.addVariable("BIFROSTCaveHatchVoidHeight",90);
  Control.addVariable("BIFROSTCaveHatchHeight",100);//95);
    Control.addVariable("BIFROSTCaveHatchDepth",350);//210);
  Control.addVariable("BIFROSTCaveHatchVoidDepth",350);//,210);
  Control.addVariable("BIFROSTCaveHatchNFloorLayers",1);
  Control.addVariable("BIFROSTCaveHatchDefMat","LimestoneConcrete");
  Control.addVariable("BIFROSTCaveHatchVoidMat","Air");
  Control.addVariable("BIFROSTCaveHatchInnerB4CLayerThick",1.0);
  Control.addVariable("BIFROSTCaveHatchB4CTileMat","B4CConcrete");
  //  Control.addVariable("BIFROSTCaveHatchB4CTileMat","Air");
 
  SGen.clearLayers(); 
  //  SGen.generateShield(Control,"BIFROSTHatch2",98.0, 120, 40, 100, 1,2);
  Control.addVariable("BIFROSTHatch2NSeg",1);
  Control.addVariable("BIFROSTHatch2NWallLayersR",2);
  Control.addVariable("BIFROSTHatch2NRoofLayers",2);
  Control.addVariable("BIFROSTHatch2NWallLayersL",2);
  Control.addVariable("BIFROSTHatch2NLayers",2);
  Control.addVariable("BIFROSTHatch2Length",98.0);
  Control.addVariable("BIFROSTHatch2Spacing",0.0);
  Control.addVariable("BIFROSTHatch2VoidLeft",90);
  Control.addVariable("BIFROSTHatch2VoidRight",70);
  Control.addVariable("BIFROSTHatch2Left",100);
  Control.addVariable("BIFROSTHatch2Right",80);
  Control.addVariable("BIFROSTHatch2VoidHeight",90);
  Control.addVariable("BIFROSTHatch2Height",100);
  Control.addVariable("BIFROSTHatch2Depth",140);
  Control.addVariable("BIFROSTHatch2VoidDepth",140);
  Control.addVariable("BIFROSTHatch2NFloorLayers",1);
  Control.addVariable("BIFROSTHatch2DefMat","Stainless304");
  Control.addVariable("BIFROSTHatch2VoidMat","Air");
Control.addVariable("BIFROSTHatch2InnerB4CLayerThick",1.0);
  Control.addVariable("BIFROSTHatch2B4CTileMat","B4CConcrete");
 
  
  //  SGen.generateShield(Control,"BIFROSTCaveHatch3",15.0, 120, 40, 100, 1,2);
    SGen.generateShield(Control,"BIFROSTCaveHatch3",10.0, 120, 40, 100, 1,2);
  Control.addVariable("BIFROSTCaveHatch3Spacing",0.0);
  Control.addVariable("BIFROSTCaveHatch3VoidLeft",90);
  Control.addVariable("BIFROSTCaveHatch3VoidRight",70);
  Control.addVariable("BIFROSTCaveHatch3Left",130);
  Control.addVariable("BIFROSTCaveHatch3Right",110);
  //  Control.addVariable("BIFROSTCaveHatch3VoidHeight",60);
    Control.addVariable("BIFROSTCaveHatch3VoidHeight",90);
  Control.addVariable("BIFROSTCaveHatch3Height",100);
  Control.addVariable("BIFROSTCaveHatch3Depth",140);
  Control.addVariable("BIFROSTCaveHatch3VoidDepth",140);
  Control.addVariable("BIFROSTCaveHatch3NFloorLayers",1);
  Control.addVariable("BIFROSTCaveHatch3DefMat","Stainless304");
  Control.addVariable("BIFROSTCaveHatch3VoidMat","Air");

Control.addVariable("BIFROSTCaveHatch3PreZAngle",90.0);
		     	Control.addVariable("BIFROSTCaveHatch3PreXYAngle",-90.0);
			//	Control.addVariable("BIFROSTCaveHatch3ZAngle",-90.0);
		Control.addVariable("BIFROSTCaveHatch3XYAngle",90.0);
  Control.addVariable("BIFROSTCaveHatch3InnerB4CLayerThick",1.0);
  Control.addVariable("BIFROSTCaveHatch3B4CTileMat","B4CConcrete");




  SGen.generateShield(Control,"BIFROSTCaveHatchRails",10.0, 120, 40, 100, 1,2);
  Control.addVariable("BIFROSTCaveHatchRailsSpacing",0.0);
  Control.addVariable("BIFROSTCaveHatchRailsVoidLeft",100);
  Control.addVariable("BIFROSTCaveHatchRailsVoidRight",120);
  Control.addVariable("BIFROSTCaveHatchRailsLeft",130);
  Control.addVariable("BIFROSTCaveHatchRailsRight",150);
  Control.addVariable("BIFROSTCaveHatchRailsVoidHeight",350);
  Control.addVariable("BIFROSTCaveHatchRailsHeight",350);
  Control.addVariable("BIFROSTCaveHatchRailsDepth",93);
  Control.addVariable("BIFROSTCaveHatchRailsVoidDepth",93);
  Control.addVariable("BIFROSTCaveHatchRailsNFloorLayers",1);
  Control.addVariable("BIFROSTCaveHatchRailsNRoofLayers",2);
  Control.addVariable("BIFROSTCaveHatchRailsDefMat","Stainless304");
  Control.addVariable("BIFROSTCaveHatchRailsVoidMat","Air");
  Control.addVariable("BIFROSTCaveHatchRailsRoofMat","Air");
Control.addVariable("BIFROSTCaveHatchRailsInnerB4CLayerThick",0.0);



      

 SGen.generateShield(Control,"BIFROSTCaveGallerySteel",4.0, 140, 28.95, 30, 1,2);
  Control.addVariable("BIFROSTCaveGallerySteelVoidLeft",50.0);
  Control.addVariable("BIFROSTCaveGallerySteelNRoofLayers",2);
    Control.addVariable("BIFROSTCaveGallerySteelVoidRight",30.0);
  Control.addVariable("BIFROSTCaveGallerySteelLeft",88.95);
    Control.addVariable("BIFROSTCaveGallerySteelRight",68.95);
      Control.addVariable("BIFROSTCaveGallerySteelVoidHeight",28.5);
        Control.addVariable("BIFROSTCaveGallerySteelVoidDepth",179);
       Control.addVariable("BIFROSTCaveGallerySteelSpacing",0.0);
       Control.addVariable("BIFROSTCaveGallerySteelSpacingMat","LimestoneConcrete");
       Control.addVariable("BIFROSTCaveGallerySteelNFloorLayers",1);
       Control.addVariable("BIFROSTCaveGallerySteelInnerB4CLayerThick",0.0);
       //      Control.addVariable("BIFROSTCaveGallerySteelB4CTileMat","B-Poly");
       //	     	Control.addVariable("BIFROSTCaveGallerySteelPreZAngle",-90.0);
      Control.addVariable("BIFROSTCaveGallerySteelZStep",266.3);
      Control.addVariable("BIFROSTCaveGallerySteelYStep",15.0);
      Control.addVariable("BIFROSTCaveGallerySteelDefMat","Stainless304");
      Control.addVariable("BIFROSTCaveGallerySteelVoidMat","Air");


      
 SGen.generateShield(Control,"BIFROSTCaveGalleryDoor",0.5, 140, 28.95, 30, 1,2);
  Control.addVariable("BIFROSTCaveGalleryDoorVoidLeft",50.0);
  Control.addVariable("BIFROSTCaveGalleryDoorNRoofLayers",2);
  Control.addVariable("BIFROSTCaveGalleryDoorNWallLayers",2);
    Control.addVariable("BIFROSTCaveGalleryDoorVoidRight",60.0);
    Control.addVariable("BIFROSTCaveGalleryDoorVoidLeft",80.0);
  Control.addVariable("BIFROSTCaveGalleryDoorLeft",86);
    Control.addVariable("BIFROSTCaveGalleryDoorRight",68);
      Control.addVariable("BIFROSTCaveGalleryDoorHeight",26.5);
      Control.addVariable("BIFROSTCaveGalleryDoorVoidHeight",18.5);
        Control.addVariable("BIFROSTCaveGalleryDoorDepth",179);
        Control.addVariable("BIFROSTCaveGalleryDoorVoidDepth",179);
       Control.addVariable("BIFROSTCaveGalleryDoorSpacing",0.0);
       Control.addVariable("BIFROSTCaveGalleryDoorSpacingMat","LimestoneConcrete");
       Control.addVariable("BIFROSTCaveGalleryDoorNFloorLayers",1);
       Control.addVariable("BIFROSTCaveGalleryDoorInnerB4CLayerThick",0.0);
       //      Control.addVariable("BIFROSTCaveGalleryDoorB4CTileMat","B-Poly");
       //	     	Control.addVariable("BIFROSTCaveGalleryDoorPreZAngle",-90.0);
      Control.addVariable("BIFROSTCaveGalleryDoorZStep",266.3);
      Control.addVariable("BIFROSTCaveGalleryDoorYStep",100.0);
      Control.addVariable("BIFROSTCaveGalleryDoorDefMat","B4CConcrete");
      Control.addVariable("BIFROSTCaveGalleryDoorVoidMat","B4CConcrete");


      

      

      //Only void and B4C lining for the cable routes     
  SGen.generateShield(Control,"BIFROSTCaveUpperCableRouteR",25.0, 240, 25.1, 0, 1,2);
   Control.addVariable("BIFROSTCaveUpperCableRouteRRight",61.2);
   Control.addVariable("BIFROSTCaveUpperCableRouteRLeft",-100.0);
  Control.addVariable("BIFROSTCaveUpperCableRouteRVoidLeft",60.0);
    Control.addVariable("BIFROSTCaveUpperCableRouteRVoidRight",61.1);
    Control.addVariable("BIFROSTCaveUpperCableRouteRXYAngle",180.0);
      Control.addVariable("BIFROSTCaveUpperCableRouteRVoidHeight",25);
        Control.addVariable("BIFROSTCaveUpperCableRouteRVoidDepth",0);
       Control.addVariable("BIFROSTCaveUpperCableRouteRSpacing",0.0);
       Control.addVariable("BIFROSTCaveUpperCableRouteRSpacingMat","B4C");
       Control.addVariable("BIFROSTCaveUpperCableRouteRInnerB4CLayerThick",0.0);
       Control.addVariable("BIFROSTCaveUpperCableRouteRNWallLayers",2);
       Control.addVariable("BIFROSTCaveUpperCableRouteRNRoofLayers",2);
       Control.addVariable("BIFROSTCaveUpperCableRouteRNFloorLayers",1);
       //       Control.addVariable("BIFROSTCaveUpperCableRouteRB4CTileMat","B-Poly");
       //       Control.addVariable("BIFROSTCaveUpperCableRouteRDefMat","B-Poly");
       Control.addVariable("BIFROSTCaveUpperCableRouteRB4CTileMat","Air");
       Control.addVariable("BIFROSTCaveUpperCableRouteRDefMat","Air");
  Control.addVariable("BIFROSTCaveUpperCableRouteRVoidFront",20);
  Control.addVariable("BIFROSTCaveUpperCableRouteRFront",20.1);
    Control.addVariable("BIFROSTCaveUpperCableRouteRYStep",90.0);
    // Control.addVariable("BIFROSTCaveUpperCableRouteRZStep",-25.0);
    Control.addVariable("BIFROSTCaveUpperCableRouteRXStep",-88.85);
    Control.addVariable("BIFROSTCaveUpperCableRouteRVoidMat","Air");
    Control.addVariable("BIFROSTCaveUpperCableRouteLVoidMat","Air");
   


  SGen.generateShield(Control,"BIFROSTCaveUpperCableRouteL",60.0, 240, 25.1, 0, 1,2);
   Control.addVariable("BIFROSTCaveUpperCableRouteLLeft",121.2);
   Control.addVariable("BIFROSTCaveUpperCableRouteLRight",-100.0);
  Control.addVariable("BIFROSTCaveUpperCableRouteLVoidLeft",120.0);
  Control.addVariable("BIFROSTCaveUpperCableRouteLVoidRight",121.1);
    Control.addVariable("BIFROSTCaveUpperCableRouteLXYAngle",180.0);
      Control.addVariable("BIFROSTCaveUpperCableRouteLVoidHeight",25);
        Control.addVariable("BIFROSTCaveUpperCableRouteLVoidDepth",0);
       Control.addVariable("BIFROSTCaveUpperCableRouteLSpacing",0.0);
       Control.addVariable("BIFROSTCaveUpperCableRouteLInnerB4CLayerThick",0.0);
       Control.addVariable("BIFROSTCaveUpperCableRouteLNWallLayers",2);
       Control.addVariable("BIFROSTCaveUpperCableRouteLNRoofLayers",2);
       Control.addVariable("BIFROSTCaveUpperCableRouteLNFloorLayers",1);
       Control.addVariable("BIFROSTCaveUpperCableRouteLB4CTileMat","B-Poly");
       Control.addVariable("BIFROSTCaveUpperCableRouteLDefMat","B-Poly");
  Control.addVariable("BIFROSTCaveUpperCableRouteLVoidFront",60);
  Control.addVariable("BIFROSTCaveUpperCableRouteLFront",60.1);
    Control.addVariable("BIFROSTCaveUpperCableRouteLYStep",89.95);
    Control.addVariable("BIFROSTCaveUpperCableRouteLXStep",68.85);
    //  Control.addVariable("BIFROSTCaveUpperCableRouteLZStep",-25.0);

    

  Control.addVariable("BIFROSTCaveHatchLockNSlices",2);
  Control.addVariable("BIFROSTCaveHatchLockNLayers0",1);
  Control.addVariable("BIFROSTCaveHatchLockNLayers1",4);
  //  Control.addVariable("BIFROSTCaveHatchLockNLayers2",5);
  Control.addVariable("BIFROSTCaveHatchLockWidth",25);
  Control.addVariable("BIFROSTCaveHatchLockHeight",159);
  Control.addVariable("BIFROSTCaveHatchLockDepth",0);
  Control.addVariable("BIFROSTCaveHatchLockLength0",0.1);
  Control.addVariable("BIFROSTCaveHatchLockLength1",17.9);
  Control.addVariable("BIFROSTCaveHatchLockMat0","B4CAluminium");
  Control.addVariable("BIFROSTCaveHatchLockMat1","Stainless304");
  // Control.addVariable("BIFROSTCaveHatchLockLength2",50.0);
  Control.addVariable("BIFROSTCaveHatchLockMat2","LimestoneConcrete");
  Control.addVariable("BIFROSTCaveHatchLockXStep", -105.5);
  //  Control.addVariable("BIFROSTCaveHatchLockYStep", 43.4);
    Control.addVariable("BIFROSTCaveHatchLockYStep", 12);
  Control.addVariable("BIFROSTCaveHatchLockZStep", -10);
    


  Control.addVariable("BIFROSTCaveHatchLock1NSlices",3);
  Control.addVariable("BIFROSTCaveHatchLock1NLayers0",1);
  Control.addVariable("BIFROSTCaveHatchLock1NLayers1",3);
  Control.addVariable("BIFROSTCaveHatchLock1NLayers2",5);
  Control.addVariable("BIFROSTCaveHatchLock1Width",194);
  Control.addVariable("BIFROSTCaveHatchLock1Height",209);
  Control.addVariable("BIFROSTCaveHatchLock1Depth",0);
  Control.addVariable("BIFROSTCaveHatchLock1Length0",0.1);
  Control.addVariable("BIFROSTCaveHatchLock1Length1",14.9);
  Control.addVariable("BIFROSTCaveHatchLock1Mat0","B4CAluminium");
  Control.addVariable("BIFROSTCaveHatchLock1Mat1","Stainless304");
  //  Control.addVariable("BIFROSTCaveHatchLock1Length2",65.0);
    Control.addVariable("BIFROSTCaveHatchLock1Length2",50.0);
  Control.addVariable("BIFROSTCaveHatchLock1Mat2","LimestoneConcrete");
  Control.addVariable("BIFROSTCaveHatchLock1XStep", 4.0);
  //  Control.addVariable("BIFROSTCaveHatchLock1YStep", 43.4);
  Control.addVariable("BIFROSTCaveHatchLock1YStep", 35.5);
  Control.addVariable("BIFROSTCaveHatchLock1ZStep", -10);




  Control.addVariable("BIFROSTCaveHatchLock2NSlices",2);
  Control.addVariable("BIFROSTCaveHatchLock2NLayers0",3);
  Control.addVariable("BIFROSTCaveHatchLock2NLayers1",5);
  Control.addVariable("BIFROSTCaveHatchLock2Width",15);
  Control.addVariable("BIFROSTCaveHatchLock2Height",209);
  Control.addVariable("BIFROSTCaveHatchLock2Depth",0);
  Control.addVariable("BIFROSTCaveHatchLock2Length0",17.4);
  //  Control.addVariable("BIFROSTCaveHatchLock2Length1",65.0);
    Control.addVariable("BIFROSTCaveHatchLock2Length1",50.0);
   Control.addVariable("BIFROSTCaveHatchLock2Mat0","Stainless304");
  Control.addVariable("BIFROSTCaveHatchLock2Mat1","LimestoneConcrete");
  Control.addVariable("BIFROSTCaveHatchLock2XStep", 108.5);
  //  Control.addVariable("BIFROSTCaveHatchLock2YStep", 42.2);
    Control.addVariable("BIFROSTCaveHatchLock2YStep", 34.3);
  Control.addVariable("BIFROSTCaveHatchLock2ZStep", -10);

  
  Control.addVariable("BIFROSTCaveHatchStreamBlockerNSlices",1);
  Control.addVariable("BIFROSTCaveHatchStreamBlockerNLayers0",1);
  // Control.addVariable("BIFROSTCaveHatchStreamBlockerWidth",15);
   Control.addVariable("BIFROSTCaveHatchStreamBlockerWidth",10);
  Control.addVariable("BIFROSTCaveHatchStreamBlockerHeight",209);
  Control.addVariable("BIFROSTCaveHatchStreamBlockerDepth",0);
  Control.addVariable("BIFROSTCaveHatchStreamBlockerLength0",2.5);
   Control.addVariable("BIFROSTCaveHatchStreamBlockerMat0","Stainless304");
   // Control.addVariable("BIFROSTCaveHatchStreamBlockerXStep", 92);
    Control.addVariable("BIFROSTCaveHatchStreamBlockerXStep", 95);
  Control.addVariable("BIFROSTCaveHatchStreamBlockerYStep", 1.25);
  Control.addVariable("BIFROSTCaveHatchStreamBlockerZStep", -10);
  


  
  /*
 //Cd sheet
  Control.addVariable("BIFROSTCaveSampleSheetNSlices",1);
  Control.addVariable("BIFROSTCaveSampleSheetNLayers0",1);
  Control.addVariable("BIFROSTCaveSampleSheetWidth",10);
  Control.addVariable("BIFROSTCaveSampleSheetHeight",10.0);
  Control.addVariable("BIFROSTCaveSampleSheetDepth",0);
  Control.addVariable("BIFROSTCaveSampleSheetLength0",0.1);
  Control.addVariable("BIFROSTCaveSampleSheetMat0","Cadmium");
  */
  
//D2O, 1L
  Control.addVariable("BIFROSTCaveSampleSheetNSlices",1);
  Control.addVariable("BIFROSTCaveSampleSheetNLayers0",1);
  Control.addVariable("BIFROSTCaveSampleSheetWidth",10);
  Control.addVariable("BIFROSTCaveSampleSheetHeight",10.0);
  Control.addVariable("BIFROSTCaveSampleSheetDepth",0);
  Control.addVariable("BIFROSTCaveSampleSheetLength0",10.0);
  Control.addVariable("BIFROSTCaveSampleSheetMat0","D2O");
  

  
  Control.addVariable("BIFROSTCaveLabyrinthPlateNSlices",3);
  Control.addVariable("BIFROSTCaveLabyrinthPlateNLayers0",1);
  Control.addVariable("BIFROSTCaveLabyrinthPlateNLayers1",1);
  Control.addVariable("BIFROSTCaveLabyrinthPlateNLayers2",1);
  Control.addVariable("BIFROSTCaveLabyrinthPlateWidth",140);
  Control.addVariable("BIFROSTCaveLabyrinthPlateHeight",205.0);
  Control.addVariable("BIFROSTCaveLabyrinthPlateDepth",0);
  Control.addVariable("BIFROSTCaveLabyrinthPlateLength0",90);
  Control.addVariable("BIFROSTCaveLabyrinthPlateLength1",1);
  Control.addVariable("BIFROSTCaveLabyrinthPlateLength2",10);
  Control.addVariable("BIFROSTCaveLabyrinthPlateMat0","Air");
  Control.addVariable("BIFROSTCaveLabyrinthPlateMat1","Air");
  Control.addVariable("BIFROSTCaveLabyrinthPlateMat2","Stainless304");
  Control.addVariable("BIFROSTCaveLabyrinthPlateXStep", 3.5);
  Control.addVariable("BIFROSTCaveLabyrinthPlateYStep", 88.0);
  Control.addVariable("BIFROSTCaveLabyrinthPlateZStep", -75.0);




 

  
  
  Control.addVariable("BIFROSTCaveDetNeutronDoorNSlices",2);
  Control.addVariable("BIFROSTCaveDetNeutronDoorNLayers0",5);
  Control.addVariable("BIFROSTCaveDetNeutronDoorNLayers1",1);
  Control.addVariable("BIFROSTCaveDetNeutronDoorWidth",150);
  Control.addVariable("BIFROSTCaveDetNeutronDoorHeight",300);
  Control.addVariable("BIFROSTCaveDetNeutronDoorDepth",0);
  Control.addVariable("BIFROSTCaveDetNeutronDoorLength0",10);
  Control.addVariable("BIFROSTCaveDetNeutronDoorLength1",1);
  Control.addVariable("BIFROSTCaveDetNeutronDoorMat0","B-Poly");
  Control.addVariable("BIFROSTCaveDetNeutronDoorMat1","Stainless304");
  Control.addVariable("BIFROSTCaveDetNeutronDoorXStep",0.0);
  Control.addVariable("BIFROSTCaveDetNeutronDoorZAngle",180.0);
  Control.addVariable("BIFROSTCaveDetNeutronDoorYStep",50);
  //  Control.addVariable("BIFROSTCaveDetNeutronDoorZStep", -30.05);



  
  Control.addVariable("BIFROSTCaveBeamstopNSlices",2);
  Control.addVariable("BIFROSTCaveBeamstopNLayers0",1);
  Control.addVariable("BIFROSTCaveBeamstopNLayers1",3);
  Control.addVariable("BIFROSTCaveBeamstopWidth",40);
  Control.addVariable("BIFROSTCaveBeamstopHeight",40);
  Control.addVariable("BIFROSTCaveBeamstopDepth",0);
  Control.addVariable("BIFROSTCaveBeamstopLength0",0.1);
  Control.addVariable("BIFROSTCaveBeamstopLength1",29.9);
  Control.addVariable("BIFROSTCaveBeamstopMat0","B4C");
  Control.addVariable("BIFROSTCaveBeamstopMat1","B-Poly");
  Control.addVariable("BIFROSTCaveBeamstopYStep", 60);
 
  
  nLayer=3;
  Control.addVariable("BIFROSTCaveLockCapNLayers",nLayer);
  Control.addVariable("BIFROSTCaveLockCapMat","Stainless304");
  Control.addVariable("BIFROSTCaveLockMat","B-Poly");


  


  
  for (int iwall = 0 ; iwall<nLayer; iwall++){
  Control.addVariable("BIFROSTCaveCapLayerThick"+std::to_string(iwall),15.0/nLayer);
  }
  
  //  Control.addVariable("BIFROSTCaveLockCapMat","Stainless"); to be implemented later
  Control.addVariable("BIFROSTCaveWallMat","LimestoneConcrete");
  Control.addVariable("BIFROSTCaveInnerMat","Air");
  Control.addVariable("BIFROSTCaveBeamstopMat","B4C");

  // Beam port through front of cave
  //  Control.addVariable("CaveCutShape","Circle");
  //  Control.addVariable("BIFROSTCaveCutRadius",20.0);

  /*
  nLayer=12;
  Control.addVariable("BIFROSTCaveNWallLayers",nLayer);

  for (int iwall = 0 ; iwall<nLayer; iwall++){
  Control.addVariable("BIFROSTCaveWLayerThick"+std::to_string(iwall),120.0/nLayer);
  }

    nLayer=14;
  Control.addVariable("BIFROSTCaveNRoofLayers",nLayer);

  for (int iwall = 0 ; iwall<nLayer; iwall++){
    Control.addVariable("BIFROSTCaveWLayerThick"+std::to_string(iwall),140.0/nLayer);
  }

  */


  
  Control.addVariable("BrickShapeInWidth",300);
  Control.addVariable("BrickShapeInHeight",200);
  Control.addVariable("BrickShapeInBrickWidth",90);
  Control.addVariable("BrickShapeInBrickHeight",60);
  Control.addVariable("BrickShapeInBrickThick",60);
  Control.addVariable("BrickShapeInGapWidthHorizontal",0.6);
  Control.addVariable("BrickShapeInGapWidthDepth",0.0);
  Control.addVariable("BrickShapeInGapWidthVertical",0.3);

  Control.addVariable("BrickShapeInOffsetVertical",20.0);
  Control.addVariable("BrickShapeInOffsetHorizontal",30.0);



  /*    FGen.generateTaper(Control,"BIFROSTCaveInletCut",120.0,
                         65,65,65,65);
    Control.addVariable("BIFROSTCaveInletCutLayerMat0","Air");
  */
  
    SGen.generateShield(Control,"BIFROSTCaveInletCut",120, 65, 65, 180.0, 10,7);
  Control.addVariable("BIFROSTCaveInletCutVoidLeft",32.5);
    Control.addVariable("BIFROSTCaveInletCutVoidRight",32.5);
      Control.addVariable("BIFROSTCaveInletCutVoidHeight",32.5);
        Control.addVariable("BIFROSTCaveInletCutVoidDepth",32.5);
  Control.addVariable("BIFROSTCaveInletCutLeft",102.5);
    Control.addVariable("BIFROSTCaveInletCutRight",102.5);
      Control.addVariable("BIFROSTCaveInletCutHeight",102.5);
        Control.addVariable("BIFROSTCaveInletCutDepth",102.5);
        Control.addVariable("BIFROSTCaveInletCutSpacing",0.0);
        Control.addVariable("BIFROSTCaveInletCutVoidMat","Air");
        Control.addVariable("BIFROSTCaveInletCutDefMat","LimestoneConcrete");
	Control.addVariable("BIFROSTCaveInletCutNFloorLayers",7);





    SGen.generateShield(Control,"BIFROSTCaveInletCut2",60, 65, 65, 180.0, 5,7);
  Control.addVariable("BIFROSTCaveInletCut2Left",32.5);
    Control.addVariable("BIFROSTCaveInletCut2Right",32.5);
      Control.addVariable("BIFROSTCaveInletCut2Height",32.5);
        Control.addVariable("BIFROSTCaveInletCut2Depth",32.5);
  Control.addVariable("BIFROSTCaveInletCut2VoidLeft",15);
    Control.addVariable("BIFROSTCaveInletCut2VoidRight",15);
      Control.addVariable("BIFROSTCaveInletCut2VoidHeight",15);
        Control.addVariable("BIFROSTCaveInletCut2VoidDepth",23.5);
        Control.addVariable("BIFROSTCaveInletCut2Spacing",0.0);
        Control.addVariable("BIFROSTCaveInletCut2VoidMat","Air");
        Control.addVariable("BIFROSTCaveInletCut2DefMat","LimestoneConcrete");
	Control.addVariable("BIFROSTCaveInletCut2NFloorLayers",2);
	Control.addVariable("BIFROSTCaveInletCut2NWallLayers",3);
	Control.addVariable("BIFROSTCaveInletCut2NRoofLayers",3);



	

  
  FGen.setLayer(1,0.1,"B4C");

  FGen.setLayer(2,0.1,"B4C");
  /*
    FGen.generateTaper(Control,"BIFROSTCaveInletCut",120.0,
                         65,65,65,65);
    Control.addVariable("BIFROSTCaveInletCutLayerMat0","Air");
    // Last 2 meters of the guide will be in the cave
    */
   FGen.generateTaper(Control,"BIFROSTCaveBeamstopCut",120.0,
                         44,44,44,44);
   Control.addVariable("BIFROSTCaveBeamstopCutLayerMat0","Air");
    // Last 2 meters of the guide will be in the cave
 




  FGen.setLayer(1,0.1,"B4C");
  FGen.setLayer(2,10,"LimestoneConcrete");
  FGen.setLayer(3,0.5,"Air");
  FGen.setLayer(4,0.0,"Void");


   
  Control.addVariable("BIFROSTCaveBeamstopTshapeNSlices",2);
  Control.addVariable("BIFROSTCaveBeamstopTshapeNLayers0",1);
  Control.addVariable("BIFROSTCaveBeamstopTshapeNLayers1",6);
  Control.addVariable("BIFROSTCaveBeamstopTshapeWidth", 40);
  Control.addVariable("BIFROSTCaveBeamstopTshapeHeight", 40);
  Control.addVariable("BIFROSTCaveBeamstopTshapeDepth",0);
  Control.addVariable("BIFROSTCaveBeamstopTshapeLength0",0.1);
  Control.addVariable("BIFROSTCaveBeamstopTshapeLength1",62.9);
  Control.addVariable("BIFROSTCaveBeamstopTshapeMat0","B4C");
  Control.addVariable("BIFROSTCaveBeamstopTshapeMat1","LimestoneConcrete");
  Control.addVariable("BIFROSTCaveBeamstopTshapeXStep", 0);
  Control.addVariable("BIFROSTCaveBeamstopTshapeYStep", 12);
  Control.addVariable("BIFROSTCaveBeamstopTshapeZStep", 0);


     
  Control.addVariable("BIFROSTCaveBeamstopPlugNSlices",1);
  Control.addVariable("BIFROSTCaveBeamstopPlugNLayers0",5);
  Control.addVariable("BIFROSTCaveBeamstopPlugWidth", 77);
  Control.addVariable("BIFROSTCaveBeamstopPlugHeight",77);
  Control.addVariable("BIFROSTCaveBeamstopPlugDepth",0);
  Control.addVariable("BIFROSTCaveBeamstopPlugLength0",56.);
  Control.addVariable("BIFROSTCaveBeamstopPlugMat0","LimestoneConcrete");
  Control.addVariable("BIFROSTCaveBeamstopPlugXStep", 0);
  Control.addVariable("BIFROSTCaveBeamstopPlugYStep", 28.7);
  Control.addVariable("BIFROSTCaveBeamstopPlugZStep", 0);

  //Focusing section of BIFROST optics
  //  FGen.setLayer(1,0.5,"Copper");
    length1 = 22.1142;
    Linx1 = 24.3645;Loutx1 = 2.25034;majoraxis_x1 = 24.3646;smallaxis_x1=0.03;
    Liny1 = 23.0344;Louty1 = 0.920233;majoraxis_y1 = 23.0345;smallaxis_y1=0.045;

  
    // Last 2 meters of the guide will be in the cave





  /* PipeGen.setMat("Aluminium");
       PipeGen.setWindow(-2.0,0.3);
       PipeGen.setPipe(7.0,0.5);
  PipeGen.setFlange(7.8,1.0);
       //  PipeGen.generatePipe(Control,"bifrostPipeOutA",2.0,600);
         PipeGen.generatePipe(Control,"bifrostPipeOutA",2.0,100);
  Control.addVariable("bifrostPipeOutAYStep",4.0);
  Control.addVariable("bifrostPipeOutACladdingThick",0.3);
  Control.addVariable("bifrostPipeOutACladdingMat","Mirrobor");
  Control.addVariable("bifrostPipeOutAWindowFrontMat","Aluminium");
  Control.addVariable("bifrostPipeOutAWindowActive",1);
  */
  
  // Last 2 meters of the guide will be in the cave
  PipeGen.setWindow(-2.0,0.1);
  PipeGen.setPipe(7.0,0.5);
  PipeGen.setFlange(7.8,1.0);
  PipeGen.generatePipe(Control,"PipeCave",yStep,350.0);
  Control.addVariable("PipeCaveWindowFrontMat","Aluminium");
  // Control.addVariable("PipeCaveWindowBackMat","Aluminium");
  Control.addVariable("PipeCaveWindowActive",1);
  Control.addVariable("PipeCaveYStep",-55);
  Control.addVariable("PipeCaveXYAngle",180);



  /*
  PipeGen.setFlange(9.5,1.0);
  PipeGen.setPipe(8.0,1.5);
  PipeGen.setWindow(-2.0,0.15); 
  */
  PipeGen.setPipe(16.5,0.5);
  PipeGen.setWindow(-2.0,0.1);
  PipeGen.setFlange(-4.0,2.0);
 
  yStep=50.0; //offset for pipe

  PipeGen.generatePipe(Control,"BIFROSTCaveGetLostPipe",1.0,158.0);
  Control.addVariable("BIFROSTCaveGetLostPipeYStep",50.0);
  Control.addVariable("BIFROSTCaveGetLostPipeRadiusStart",5.0);
  Control.addVariable("BIFROSTCaveGetLostPipeRadiusEnd",16.5);
  
  Control.addVariable("BIFROSTCaveGetLostPipeWindowBackMat","Aluminium");
  Control.addVariable("BIFROSTCaveGetLostPipeWindowActive",3);
  
  Control.addVariable("BIFROSTCaveGetLostPipeFlangeFrontRadius",6.8);
  Control.addVariable("BIFROSTCaveGetLostPipeFlangeFrontLength",2.0);
  Control.addVariable("BIFROSTCaveGetLostPipeWindowFrontRadius",6.8);
  Control.addVariable("BIFROSTCaveGetLostPipeWindowBackRadius",16.5);
  
  Control.addVariable("BIFROSTCaveGetLostPipeFlangeBackRadius",20.5);
  Control.addVariable("BIFROSTCaveGetLostPipeFlangeBackLength",2.0);
 
  Control.addVariable("BIFROSTCaveGetLostPipeWindowFrontMat","Aluminium");
 //  Control.addVariable("BIFROSTCaveGetLostPipeWindowBackMat","Aluminium");


    
   PipeGen.setPipe(16.5,0.5);
  PipeGen.setWindow(-2.0,0.1);
  PipeGen.setFlange(-4.0,2.0);
  yStep=2.0; //offset for pipe

  PipeGen.generatePipe(Control,"BIFROSTCaveBeamstopPipe",yStep,200.0);
  Control.addVariable("BIFROSTCaveBeamstopPipeWindowFrontMat","Aluminium");
  Control.addVariable("BIFROSTCaveBeamstopPipeWindowBackMat","Aluminium");



  
  PipeGen.generatePipe(Control,"heimdalTBeamstopPipe",0.0,125.0);
  Control.addVariable("heimdalTBeamstopPipeRadius",23.0);
  Control.addVariable("heimdalTBeamstopPipeYStep",847.0);


  //  Control.addVariable("heimdalTBeamstopPipeFlangeFrontRadius",16);
  Control.addVariable("heimdalTBeamstopPipeCladdingThick",1.0);
  Control.addVariable("heimdalTBeamstopPipeCladdingMat","Aluminium"); //Outer material
  Control.addVariable("heimdalTBeamstopPipeFeMat","B-Poly"); //Inner material
 
  Control.addVariable("heimdalTBeamstopPipeFlangeFrontLength",-0.0);
  Control.addVariable("heimdalTBeamstopPipeWindowFrontRadius",-16.0);
  Control.addVariable("heimdalTBeamstopPipeWindowBackRadius",23.0);
  Control.addVariable("heimdalTBeamstopPipeWindowBackThick",1.0);
  //  Control.addVariable("heimdalTBeamstopPipeWindowFrontMat","Void");
  
  //  Control.addVariable("heimdalTBeamstopPipeFlangeBackRadius",25);
  // Control.addVariable("heimdalTBeamstopPipeFlangeBackLength",1.0);

  Control.addVariable("heimdalTBeamstopPipeWindowBackMat","B4C");
  Control.addVariable("heimdalTBeamstopPipeWindowActive",2);



  PipeGen.setFlange(26,1.0);
  PipeGen.setPipe(25,1.5);

  PipeGen.generatePipe(Control,"heimdalTBeamstopPipeB",0.0,1.0);
  Control.addVariable("heimdalTBeamstopPipeBRadius",23.0);
  Control.addVariable("heimdalTBeamstopPipeBFeMat","Aluminium"); //Inner material
 
  Control.addVariable("heimdalTBeamstopPipeBFlangeFrontLength",-0.0);
  Control.addVariable("heimdalTBeamstopPipeBWindowFrontRadius",-16.0);
  Control.addVariable("heimdalTBeamstopPipeBWindowBackRadius",23.0);
  Control.addVariable("heimdalTBeamstopPipeBWindowBackLength",2.0);
  Control.addVariable("heimdalTBeamstopPipeBWindowBackThick",0.15);
  //  Control.addVariable("heimdalTBeamstopPipeBWindowFrontMat","Void");
  
  //  Control.addVariable("heimdalTBeamstopPipeBFlangeBackRadius",25);
  // Control.addVariable("heimdalTBeamstopPipeBFlangeBackLength",1.0);

  Control.addVariable("heimdalTBeamstopPipeBWindowBackMat","Aluminium");
  Control.addVariable("heimdalTBeamstopPipeBWindowActive",2);



  /*
 majoroffset_x = (length1+Linx1+Loutx1)/2 - Loutx1 - 3.46; 
 hIn
  =  2*sqrt(1-majoroffset_x*majoroffset_x/majoraxis_x1/majoraxis_x1)*smallaxis_x1;
 majoroffset_y=(length1+Liny1+Louty1)/2 - Louty1 - 3.46;
 vIn
  = 2*sqrt(1-majoroffset_y*majoroffset_y/majoraxis_y1/majoraxis_y1)*smallaxis_y1;
   majoroffset_x = (length1+Linx1+Loutx1)/2 - Loutx1 ;
 hOut
  =  2*sqrt(1-majoroffset_x*majoroffset_x/majoraxis_x1/majoraxis_x1)*smallaxis_x1;
   majoroffset_y=(length1+Liny1+Louty1)/2 - Louty1 ;
 vOut
  = 2*sqrt(1-majoroffset_y*majoroffset_y/majoraxis_y1/majoraxis_y1)*smallaxis_y1;
  FGen.setLayer(1,0.5,"Glass");
  FGen.setLayer(2,0.0,"Void");
 FGen.setLayer(3,0.0,"Void");

  FGen.generateTaper(Control,"FCave",346.0,
                         hIn*100,hOut*100,vIn*100,vOut*100);

  Control.addVariable("FCaveNShapeLayers",2);
  */
     double shieldThick = 7.0;
     Control.addVariable("DetectorInnerRadius",30.0);
     Control.addVariable("DetectorShieldingThickness",shieldThick);
     Control.addVariable("DetectorTankInternalRadius",37.0);
     Control.addVariable("DetectorTankWallThickness",15.0);
     Control.addVariable("DetectorTankExternalRadius",200.0);
     Control.addVariable("DetectorShelfWidth",35.0);
     Control.addVariable("DetectorShelfThick",5.0);
     Control.addVariable("DetectorTankDepth",90.0);
     Control.addVariable("DetectorCutoutHeight",8.0);
      
     Control.addVariable("DetectorTankWallMat","B-Poly");
     Control.addVariable("DetectorShieldMat","Lead");
  
  nLayer=3;
  Control.addVariable("DetectorShieldingNLayers",nLayer);
  nLayer=5;
  Control.addVariable("DetectorTankNLayers",nLayer);
  
  //Collimator
  Control.addVariable("CollimatorInnerRadius",37);  
  Control.addVariable("CollimatorMiddleRadius",47);
  //  Control.addVariable("CollimatorOuterRadius",52);
    Control.addVariable("CollimatorOuterRadius",72);
  Control.addVariable("CollimatorHeight",8);  
  Control.addVariable("CollimatorBladeAngle",0.650);
  Control.addVariable("CollimatorBladeThick",0.03);
  Control.addVariable("CollimatorStartAngle",-56.0);
  Control.addVariable("CollimatorEndAngle",56.0);
  Control.addVariable("CollimatorBladeMat","Boron");  
  Control.addVariable("CollimatorFilterMat","Be77K");  

  Control.addVariable("ConstructOptics",1);

  Control.addVariable("ConstructBricks",0);

  Control.addVariable("ConstructDetectorTank",0);
  Control.addVariable("ConstructSampleEnvironment",1);
      
  


  Control.addVariable("BMagnetBaseInnerRadius",4.0); // Space in the middle for the sample stick
  Control.addVariable("BMagnetBaseOuterRadius",27.5);
  
  Control.addVariable("BMagnetBaseNLayers",4);
  
  Control.addVariable("BMagnetBaseLRadius0",6.5);
  Control.addVariable("BMagnetBaseLRadius1",13.0);
  Control.addVariable("BMagnetBaseLRadius2",19.50);
  Control.addVariable("BMagnetBaseLRadius3",25.5);
  Control.addVariable("BMagnetBaseLThick",0.5); // Default value for Al ring thickness
  Control.addVariable("BMagnetBaseLThick0",1.0);
  Control.addVariable("BMagnetBaseLThick1",0.8);
  Control.addVariable("BMagnetBaseLThick2",0.6);
  Control.addVariable("BMagnetBaseLThick3",0.2);
  Control.addVariable("BMagnetBaseLTemp",300.0);
  
  Control.addVariable("BMagnetBaseTopOffset",1.0);
  Control.addVariable("BMagnetBaseBaseOffset",1.0);
  Control.addVariable("BMagnetBaseCutTopAngle",88.0);
  Control.addVariable("BMagnetBaseCutBaseAngle",88.0);

  // Control.addVariable("BMagnetBaseTopThick",10.0);
  // Control.addVariable("BMagnetBaseBaseThick",10.0);

   Control.addVariable("BMagnetBaseTopThick",2.0);
   Control.addVariable("BMagnetBaseBaseThick",2.0);

  Control.addVariable("BMagnetBaseApertureWidth",2.0); // width of opening in the rings for neutron beam passage
  Control.addVariable("BMagnetBaseApertureHeight",2.0);  // height of opening in the rings for neutron beam passage

  Control.addVariable("BMagnetBaseMat","Aluminium");
  Control.addVariable("BMagnetBaseSampleMat","Cadmium");
  Control.addVariable("BMagnetBaseVoidMat","Air");



  Control.addVariable("BSampleHolderThickness",0.5);
  Control.addVariable("BSampleHolderOuterRadius",3.5);
  Control.addVariable("BSampleHolderTop",60.0);
  Control.addVariable("BSampleHolderBottom",30.0);
  Control.addVariable("BSampleHolderTemp",300.0);
  
  Control.addVariable("BSampleSampleRadius",1.0);
  Control.addVariable("BSampleSampleHeight",2.0);
  Control.addVariable("BSampleSampleTemp",300.0);
 
  Control.addVariable("BSampleHolderMat","Aluminium");
  Control.addVariable("BSampleSampleMat","Cadmium");



  SGen.clearLayers();
  SGen.addWallMat(1,"LimestoneConcrete");
  SGen.addRoofMat(1,"LimestoneConcrete");
  SGen.addFloorMat(1,"LimestoneConcrete");

  
  //manually select length (1st arg of shield generator) to choose gap size 
  SGen.generateShield(Control,"BIFROSTCaveShieldGap1",30, 155, 155, 180.0, 2,6);
  Control.addVariable("BIFROSTCaveShieldGap1VoidLeft",118.0);
    Control.addVariable("BIFROSTCaveShieldGap1VoidRight",118.0);
      Control.addVariable("BIFROSTCaveShieldGap1VoidHeight",118.0);
        Control.addVariable("BIFROSTCaveShieldGap1VoidDepth",176.0);
        Control.addVariable("BIFROSTCaveShieldGap1VoidMat","Air");
	Control.addVariable("BIFROSTCaveShieldGap1NFloorLayers",2);


  SGen.generateShield(Control,"BIFROSTCaveShieldGap2",2000.0, 117.0, 117.0, 175.0, 1,8);
  Control.addVariable("BIFROSTCaveShieldGap2VoidLeft",32.0);
    Control.addVariable("BIFROSTCaveShieldGap2VoidRight",32.0);
      Control.addVariable("BIFROSTCaveShieldGap2VoidHeight",32.0);
        Control.addVariable("BIFROSTCaveShieldGap2VoidDepth",100.0);
	Control.addVariable("BIFROSTCaveShieldGap2FinalLayerThick",0.0);
      	Control.addVariable("BIFROSTCaveShieldGap2YStep",1.0);
       Control.addVariable("BIFROSTCaveShieldGap2NRoofLayers",8);
       Control.addVariable("BIFROSTCaveShieldGap2VoidMat","Air");


  
    return;
}
 
}  // NAMESPACE setVariable
