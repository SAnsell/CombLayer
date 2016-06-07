/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:    ESSBeam/dream/DREAMvariables.cxx
 *
 * Copyright (c) 2004-2016 by Stuart Ansell
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
#include "ChopperGenerator.h"
#include "essVariables.h"


namespace setVariable
{

  
void
generateChopper(FuncDataBase& Control,
                const std::string& keyName,
                const double yStep,
                const double length,
                const double voidLength)
  /*!
    Generate the chopper variables
    \param Control :: FuncDatabase to add variables to
    \param keyName :: basename for chopper
    \param yStep :: Origin step
    \param length :: Main length of chopper [outer]
    \param voidLength :: inner void length of chopper 
  */
{
  ELog::RegMethod RegA("DREAMvariables[F]","generateChopper");
  
  Control.addVariable(keyName+"YStep",yStep);
  Control.addVariable(keyName+"MainZStep",28.0);   // drawing [5962.2]
  Control.addVariable(keyName+"Height",86.5);
  Control.addVariable(keyName+"Width",86.5);
  Control.addVariable(keyName+"Length",length);  // drawing [5960.2]
  Control.addVariable(keyName+"ShortWidth",50.5);
  Control.addVariable(keyName+"MainRadius",38.122); // estimate
  Control.addVariable(keyName+"MainThick",voidLength);  // estimate
  
  Control.addVariable(keyName+"MotorRadius",12.00); // [5691.2]
  Control.addVariable(keyName+"MotorOuter",15.20); // [5691.2]
  Control.addVariable(keyName+"MotorStep",0.0); // estimate
  Control.addVariable(keyName+"MotorNBolt",24); 
  Control.addVariable(keyName+"MotorBoltRadius",0.50); //M10 inc thread
  Control.addVariable(keyName+"MotorSealThick",0.2);  
  Control.addVariable(keyName+"MortorSealMat","Poly");
  
  Control.addVariable(keyName+"PortRadius",10.0); // [5691.2]
  Control.addVariable(keyName+"PortOuter",12.65); // [5691.2]
  Control.addVariable(keyName+"PortStep",0.0); // estimate
  Control.addVariable(keyName+"PortNBolt",24); 
  Control.addVariable(keyName+"PortBoltRadius",0.40); //M8 inc
  Control.addVariable(keyName+"PortBoltAngOff",180.0/24.0);
  Control.addVariable(keyName+"PortSealThick",0.2);
  Control.addVariable(keyName+"PortSealMat","Poly");

  Control.addVariable(keyName+"RingNSection",12);
  Control.addVariable(keyName+"RingNTrack",12);
  Control.addVariable(keyName+"RingThick",0.4);
  Control.addVariable(keyName+"RingRadius",40.0);  
  Control.addVariable(keyName+"RingMat","Poly"); 

  // strange /4 because it is average of 1/2 lengths
  const std::string kItem=
    "-("+keyName+"Length+"+keyName+"MainThick)/4.0";
  Control.addParse<double>(keyName+"IPortAYStep",kItem);
 
  Control.addVariable(keyName+"IPortAWidth",11.6);  
  Control.addVariable(keyName+"IPortAHeight",11.6);
  Control.addVariable(keyName+"IPortALength",1.0);
  Control.addVariable(keyName+"IPortAMat","Aluminium");
  Control.addVariable(keyName+"IPortASealStep",0.5);
  Control.addVariable(keyName+"IPortASealThick",0.3); 
  Control.addVariable(keyName+"IPortASealMat","Poly");
  Control.addVariable(keyName+"IPortAWindow",0.3);
  Control.addVariable(keyName+"IPortAWindowMat","Aluminium");

  Control.addVariable(keyName+"IPortANBolt",8);
  Control.addVariable(keyName+"IPortABoltStep",1.0);
  Control.addVariable(keyName+"IPortABoltRadius",0.3);
  Control.addVariable(keyName+"IPortABoltMat","Stainless304");
  
  // PORT B
  Control.addParse<double>(keyName+"IPortBYStep",
        "("+keyName+"Length+"+keyName+"MainThick)/4.0");
  Control.addVariable(keyName+"IPortBWidth",12.0);  
  Control.addVariable(keyName+"IPortBHeight",12.0);
  Control.addVariable(keyName+"IPortBLength",1.0);
  Control.addVariable(keyName+"IPortBMat","Aluminium");
  Control.addVariable(keyName+"IPortBSealStep",0.5);
  Control.addVariable(keyName+"IPortBSealThick",0.3); 
  Control.addVariable(keyName+"IPortBSealMat","Poly");
  Control.addVariable(keyName+"IPortBWindow",0.3);
  Control.addVariable(keyName+"IPortBWindowMat","Aluminium");

  Control.addVariable(keyName+"IPortBNBolt",8);
  Control.addVariable(keyName+"IPortBBoltStep",1.0);
  Control.addVariable(keyName+"IPortBBoltRadius",0.3);
  Control.addVariable(keyName+"IPortBBoltMat","Stainless304");
    
  Control.addVariable(keyName+"BoltMat","Stainless304");
  Control.addVariable(keyName+"WallMat","Aluminium");
  Control.addVariable(keyName+"VoidMat","Void");
  return;
}

void
generatePipe(FuncDataBase& Control,
	     const std::string& keyName,
	     const double length)
  /*!
    Create general pipe
    \param Control :: Data Base for variables
    \param keyName :: main name
    \param length :: length of pipe
   */
{
  ELog::RegMethod RegA("DREAMvariables[F]","generatePipe");
    // VACUUM PIPES:
  Control.addVariable(keyName+"YStep",2.0);   // step + flange
  Control.addVariable(keyName+"Radius",8.0);
  Control.addVariable(keyName+"Length",length);
  Control.addVariable(keyName+"FeThick",1.0);
  Control.addVariable(keyName+"FlangeRadius",12.0);
  Control.addVariable(keyName+"FlangeLength",1.0);
  Control.addVariable(keyName+"FeMat","Stainless304");
  Control.addVariable(keyName+"WindowActive",3);
  Control.addVariable(keyName+"WindowRadius",10.0);
  Control.addVariable(keyName+"WindowThick",0.5);
  Control.addVariable(keyName+"WindowMat","Silicon300K");

  return;
}

void
generateFocusTaper(FuncDataBase& Control,
                   const std::string& keyName,
                   const double length,
                   const double HS,const double HE,
                   const double VS,const double VE)
                   
  /*!
    Create general focused taper
    \param Control :: Data Base for variables
    \param keyName :: main name
    \param length :: length of pipe
    \param HS :: Start of horrizontal
    \param HE :: End of horrizontal
    \param VS :: Start of horrizontal
    \param VE :: End of horrizontal
   */
{
  ELog::RegMethod RegA("DREAMvariables[F]","generateFocusTaper");

  Control.addVariable(keyName+"Length",length);       
  Control.addVariable(keyName+"XStep",0.0);       
  Control.addParse<double>(keyName+"YStep","-"+keyName+"Length/2.0");
  Control.copyVar(keyName+"BeamY",keyName+"YStep"); 
  Control.addVariable(keyName+"ZStep",0.0);       
  Control.addVariable(keyName+"XYAngle",0.0);       
  Control.addVariable(keyName+"ZAngle",0.0);

  Control.addVariable(keyName+"NShapes",1);       
  Control.addVariable(keyName+"NShapeLayers",3);
  Control.addVariable(keyName+"ActiveShield",0);

  Control.addVariable(keyName+"LayerThick1",0.4);  // glass thick
  Control.addVariable(keyName+"LayerThick2",1.5);

  Control.addVariable(keyName+"LayerMat0","Void");
  Control.addVariable(keyName+"LayerMat1","Aluminium");
  Control.addVariable(keyName+"LayerMat2","Void");       
  
  Control.addVariable(keyName+"0TypeID","Taper");
  Control.addVariable(keyName+"0HeightStart",VS);
  Control.addVariable(keyName+"0HeightEnd",VE);
  Control.addVariable(keyName+"0WidthStart",HS);
  Control.addVariable(keyName+"0WidthEnd",HE);
  Control.copyVar(keyName+"0Length",keyName+"Length");

  return;
}


void
generateT0Chopper(FuncDataBase& Control,
		  const std::string& diskName,
		  const std::string& midName,
		  const std::string chopperName)
  /*!
    Generate variables for a T0 chopper
    \param Control :: Variable database
    \param diskName :: Common name of disk object (A/B)
    \param midName :: Common name of inner focus unit
    \param chopperName :: commond name for chopper
   */
{
  ELog::RegMethod RegA("DreamVARIABLES","generateT0Chopper");
  
  generateChopper(Control,chopperName,25.0,36.0,32.0);

  // T0 Chopper disk A
  Control.addVariable(diskName+"AXStep",0.0);
  Control.addVariable(diskName+"AYStep",-12.0);
  Control.addVariable(diskName+"AZStep",0.0);
  Control.addVariable(diskName+"AXYangle",0.0);
  Control.addVariable(diskName+"AZangle",0.0);

  Control.addVariable(diskName+"AInnerRadius",20.0);
  Control.addVariable(diskName+"AOuterRadius",30.0);
  Control.addVariable(diskName+"ANDisk",1);

  Control.addVariable(diskName+"A0InnerThick",5.4);  // to include B4C
  Control.addVariable(diskName+"A0Thick",3.4);  // to include B4C
  Control.addVariable(diskName+"AInnerMat","Inconnel");
  Control.addVariable(diskName+"AOuterMat","Void");
  
  Control.addVariable(diskName+"ANBlades",2);
  Control.addVariable(diskName+"A0PhaseAngle0",95.0);
  Control.addVariable(diskName+"A0OpenAngle0",35.0);
  Control.addVariable(diskName+"A0PhaseAngle1",275.0);
  Control.addVariable(diskName+"A0OpenAngle1",25.0);

  // TMid guide
  Control.addVariable(midName+"XStep",0.0);       
  Control.addVariable(midName+"YStep",-8.0);       
  Control.addVariable(midName+"ZStep",0.0);       
  Control.addVariable(midName+"XYAngle",0.0);       
  Control.addVariable(midName+"ZAngle",0.0);
  Control.addVariable(midName+"Length",16.0);       
  
  Control.addVariable(midName+"BeamYStep",-8.0); 
  Control.addVariable(midName+"NShapes",1);       
  Control.addVariable(midName+"NShapeLayers",3);
  Control.addVariable(midName+"ActiveShield",0);

  Control.addVariable(midName+"LayerThick1",0.4);  // glass thick
  Control.addVariable(midName+"LayerThick2",1.5);

  Control.addVariable(midName+"LayerMat0","Void");
  Control.addVariable(midName+"LayerMat1","Aluminium");
  Control.addVariable(midName+"LayerMat2","Void");       
  
  Control.addVariable(midName+"0TypeID","Taper");
  Control.addVariable(midName+"0HeightStart",4.6);
  Control.addVariable(midName+"0HeightEnd",4.5);
  Control.addVariable(midName+"0WidthStart",2.06);
  Control.addVariable(midName+"0WidthEnd",2.36);
  Control.copyVar(midName+"0Length",midName+"Length");

  // T0 Chopper disk B
  Control.addVariable(diskName+"BXStep",0.0);
  Control.addVariable(diskName+"BYStep",12.0);
  Control.addVariable(diskName+"BZStep",0.0);
  Control.addVariable(diskName+"BXYangle",0.0);
  Control.addVariable(diskName+"BZangle",0.0);

  Control.addVariable(diskName+"BInnerRadius",20.0);
  Control.addVariable(diskName+"BOuterRadius",30.0);
  Control.addVariable(diskName+"BNDisk",1);

  Control.addVariable(diskName+"B0Thick",5.4);  // to include B4C
  Control.addVariable(diskName+"BInnerMat","Inconnel");
  Control.addVariable(diskName+"BOuterMat","Tungsten");
  
  Control.addVariable(diskName+"BNBlades",2);
  Control.addVariable(diskName+"B0PhaseAngle0",95.0);
  Control.addVariable(diskName+"B0OpenAngle0",145.0);
  Control.addVariable(diskName+"B0PhaseAngle1",275.0);
  Control.addVariable(diskName+"B0OpenAngle1",125.0);
  return;
}
  
void
DREAMvariables(FuncDataBase& Control)
  /*!
    Create all the beamline variables for vor
    \param Control :: DataBase
  */
{
  ELog::RegMethod RegA("DREAMvariables[F]","DREAMvariables");

  // extent of beamline
  Control.addVariable("dreamStopPoint",0);
  
  // Bender in section so use cut system
  Control.addVariable("dreamFAXStep",0.0);        // Centre of thermal [-2]
  Control.addVariable("dreamFAYStep",0.0);       
  Control.addVariable("dreamFAZStep",0.0);       
  Control.addVariable("dreamFAXYAngle",0.0);
  Control.addVariable("dreamFAZAngle",0.0);
  Control.addVariable("dreamFABeamXYAngle",0.0);       

  Control.addVariable("dreamFALength",350.0);       
  Control.addVariable("dreamFANShapes",1);       
  Control.addVariable("dreamFANShapeLayers",3);
  Control.addVariable("dreamFAActiveShield",0);

  Control.addVariable("dreamFALayerThick1",1.0);  // glass thick
  Control.addVariable("dreamFALayerThick2",1.5);

  Control.addVariable("dreamFALayerMat0","Void");
  Control.addVariable("dreamFALayerMat1","Copper");
  Control.addVariable("dreamFALayerMat2","Void");       

  Control.addVariable("dreamFA0TypeID","Taper");
  Control.addVariable("dreamFA0HeightStart",2.5); // guess
  Control.addVariable("dreamFA0HeightEnd",4.5);
  Control.addVariable("dreamFA0WidthStart",8.0); // NOT Centred
  Control.addVariable("dreamFA0WidthEnd",2.0);
  Control.addVariable("dreamFA0Length",350.0);

  generatePipe(Control,"dreamPipeA",46.0);
  generateFocusTaper(Control,"dreamFB",44.0,2.5,2.5,4.5,4.5);   
  //  Control.addVariable("dreamFBBeamYStep",4.0);
 
  // VACBOX A : 6.10m target centre
  //  Length 100.7 + Width [87.0] + Height [39.0] void Depth/2 + front
  generateChopper(Control,"dreamChopperA",55.0,12.0,6.55);
  
  // Double Blade chopper
  Control.addVariable("dreamDBladeXStep",0.0);
  Control.addVariable("dreamDBladeYStep",-2.0);
  Control.addVariable("dreamDBladeZStep",0.0);
  Control.addVariable("dreamDBladeXYangle",0.0);
  Control.addVariable("dreamDBladeZangle",0.0);

  Control.addVariable("dreamDBladeGap",1.0);
  Control.addVariable("dreamDBladeInnerRadius",22.5);
  Control.addVariable("dreamDBladeOuterRadius",33.5);
  Control.addVariable("dreamDBladeNDisk",2);

  Control.addVariable("dreamDBlade0Thick",0.2);
  Control.addVariable("dreamDBlade1Thick",0.2);
  Control.addVariable("dreamDBladeInnerMat","Inconnel");
  Control.addVariable("dreamDBladeOuterMat","B4C");
  
  Control.addVariable("dreamDBladeNBlades",2);
  Control.addVariable("dreamDBlade0PhaseAngle0",95.0);
  Control.addVariable("dreamDBlade0OpenAngle0",30.0);
  Control.addVariable("dreamDBlade1PhaseAngle0",95.0);
  Control.addVariable("dreamDBlade1OpenAngle0",30.0);

  Control.addVariable("dreamDBlade0PhaseAngle1",275.0);
  Control.addVariable("dreamDBlade0OpenAngle1",30.0);
  Control.addVariable("dreamDBlade1PhaseAngle1",275.0);
  Control.addVariable("dreamDBlade1OpenAngle1",30.0);


  // Single Blade chopper
  Control.addVariable("dreamSBladeXStep",0.0);
  Control.addVariable("dreamSBladeYStep",1.0);
  Control.addVariable("dreamSBladeZStep",0.0);
  Control.addVariable("dreamSBladeXYangle",0.0);
  Control.addVariable("dreamSBladeZangle",0.0);

  Control.addVariable("dreamSBladeInnerRadius",28.0);
  Control.addVariable("dreamSBladeOuterRadius",33.0);
  Control.addVariable("dreamSBladeNDisk",1);

  Control.addVariable("dreamSBlade0Thick",0.2);
  Control.addVariable("dreamSBladeInnerMat","Inconnel");
  Control.addVariable("dreamSBladeOuterMat","B4C");
  
  Control.addVariable("dreamSBladeNBlades",2);
  Control.addVariable("dreamSBlade0PhaseAngle0",95.0);
  Control.addVariable("dreamSBlade0OpenAngle0",30.0);
  Control.addVariable("dreamSBlade0PhaseAngle1",275.0);
  Control.addVariable("dreamSBlade0OpenAngle1",30.0);

    // VACUUM PIPE: SDisk to T0 (A)
  generatePipe(Control,"dreamPipeC",260.0);
  generateFocusTaper(Control,"dreamFC",252.0,2.06,2.36,4.6,4.5);   
  //  Control.addVariable("dreamFCBeamYStep",1.10); 

  // Band chopoper
  generateChopper(Control,"dreamChopperB",8.0,14.0,6.0);
  
  Control.addVariable("dreamBandADiskXStep",0.0);
  Control.addVariable("dreamBandADiskYStep",0.5);
  Control.addVariable("dreamBandADiskZStep",0.0);
  Control.addVariable("dreamBandADiskXYangle",0.0);
  Control.addVariable("dreamBandADiskZangle",0.0);

  // NOTE needs to be 750mm diameter!!!!
  Control.addVariable("dreamBandADiskInnerRadius",22.0);  
  Control.addVariable("dreamBandADiskOuterRadius",30.0);
  Control.addVariable("dreamBandADiskNDisk",1);

  Control.addVariable("dreamBandADisk0Thick",0.5);  // to include B4C
  Control.addVariable("dreamBandADiskInnerMat","Inconnel");
  Control.addVariable("dreamBandADiskOuterMat","Void");
  
  Control.addVariable("dreamBandADiskNBlades",2);
  Control.addVariable("dreamBandADisk0PhaseAngle0",95.0);
  Control.addVariable("dreamBandADisk0OpenAngle0",35.0);
  Control.addVariable("dreamBandADisk0PhaseAngle1",275.0);
  Control.addVariable("dreamBandADisk0OpenAngle1",25.0);

  
  generateChopper(Control,"dreamChopperC",20.0,36.0,32.0);
  generateT0Chopper(Control,"dreamT0Disk","dreamFT0Mid","dreamChopperC");

  // VACUUM PIPE: SDisk to T0 (A)
  generatePipe(Control,"dreamPipeD",430.0);
  generateFocusTaper(Control,"dreamFD",422.0,2.06,2.36,4.6,4.5); 

  generateChopper(Control,"dreamChopperD",10.0,9.0,3.55);

  Control.addVariable("dreamBandBDiskXStep",0.0);
  Control.addVariable("dreamBandBDiskYStep",0.5);
  Control.addVariable("dreamBandBDiskZStep",0.0);
  Control.addVariable("dreamBandBDiskXYangle",0.0);
  Control.addVariable("dreamBandBDiskZangle",0.0);

  Control.addVariable("dreamBandBDiskInnerRadius",22.0);
  Control.addVariable("dreamBandBDiskOuterRadius",30.0);
  Control.addVariable("dreamBandBDiskNDisk",1);

  Control.addVariable("dreamBandBDisk0Thick",0.5);  // to include B4C
  Control.addVariable("dreamBandBDiskInnerMat","Inconnel");
  Control.addVariable("dreamBandBDiskOuterMat","Void");
  
  Control.addVariable("dreamBandBDiskNBlades",2);
  Control.addVariable("dreamBandBDisk0PhaseAngle0",95.0);
  Control.addVariable("dreamBandBDisk0OpenAngle0",35.0);
  Control.addVariable("dreamBandBDisk0PhaseAngle1",275.0);
  Control.addVariable("dreamBandBDisk0OpenAngle1",25.0);

  generateChopper(Control,"dreamChopperE",20.0,36.0,32.0);
  generateT0Chopper(Control,"dreamT1Disk","dreamFT1Mid","dreamChopperE");

  // VACUUM PIPE: SDisk to T0 (A)
  generatePipe(Control,"dreamPipeE",500.0);
  generateFocusTaper(Control,"dreamFE",492.0,2.06,2.36,4.6,4.5); 

  // VACUUM PIPE: SDisk to T0 (A)
  generatePipe(Control,"dreamPipeF",500.0);
  generateFocusTaper(Control,"dreamFF",492.0,2.06,2.36,4.6,4.5); 




  // VACUUM PIPE: SDisk to T0 (A)
  generatePipe(Control,"dreamPipeG",800.0);
  generateFocusTaper(Control,"dreamFG",796.0,2.06,2.36,4.6,4.5);

  // VACUUM PIPE: SDisk to T0 (A)
  generatePipe(Control,"dreamPipeH",600.0);
  generateFocusTaper(Control,"dreamFH",566.0,2.06,2.36,4.6,4.5);

  // BEAM INSERT:
  Control.addVariable("dreamBInsertHeight",20.0);
  Control.addVariable("dreamBInsertWidth",28.0);
  Control.addVariable("dreamBInsertTopWall",1.0);
  Control.addVariable("dreamBInsertLowWall",1.0);
  Control.addVariable("dreamBInsertLeftWall",1.0);
  Control.addVariable("dreamBInsertRightWall",1.0);
  Control.addVariable("dreamBInsertWallMat","Stainless304");       

  // Guide in wall
  generateFocusTaper(Control,"dreamFWall",308.0,6.0,6.0,6.0,6.0);


  Control.addVariable("dreamShieldALength",1750.0-320);
  Control.addVariable("dreamShieldALeft",40.0);
  Control.addVariable("dreamShieldARight",40.0);
  Control.addVariable("dreamShieldAHeight",40.0);
  Control.addVariable("dreamShieldADepth",40.0);
  Control.addVariable("dreamShieldADefMat","Stainless304");
  Control.addVariable("dreamShieldANSeg",8);
  Control.addVariable("dreamShieldANWallLayers",8);
  Control.addVariable("dreamShieldANFloorLayers",3);
  Control.addVariable("dreamShieldANRoofLayers",8);
  Control.addVariable("dreamShieldAWallLen1",20.0);
  Control.addVariable("dreamShieldAWallMat1","CastIron");
  Control.addVariable("dreamShieldAWallMat5","Concrete");
  Control.addVariable("dreamShieldARoofLen1",20.0);
  Control.addVariable("dreamShieldAFloorLen1",20.0);

  // Guide after wall [17.5m - 3.20] for wall
  generatePipe(Control,"dreamPipeOutA",1430.0);  //
  Control.addVariable("dreamPipeOutARadius",6.0);
  //  Control.addVariable("dreamPipeOutAFlangeRadius",9.0);  

  generateFocusTaper(Control,"dreamFOutA",1422,4.5,4.97,2.24,3.05);

  // Guide after wall [+17.5m] after section 1  
  generatePipe(Control,"dreamPipeOutB",1750.0);
  Control.addVariable("dreamPipeOutBRadius",6.0);

  generateFocusTaper(Control,"dreamFOutB",2836,4.0,4.0,5.0,5.0);
  Control.addVariable("dreamFOutBLayerMat1","Glass");
  
  Control.addVariable("dreamShieldBLength",2850.0);
  Control.addVariable("dreamShieldBLeft",32.0);
  Control.addVariable("dreamShieldBRight",32.0);
  Control.addVariable("dreamShieldBHeight",32.0);
  Control.addVariable("dreamShieldBDepth",32.0);
  Control.addVariable("dreamShieldBDefMat","Stainless304");
  Control.addVariable("dreamShieldBNSeg",8);
  Control.addVariable("dreamShieldBNWallLayers",8);
  Control.addVariable("dreamShieldBNFloorLayers",3);
  Control.addVariable("dreamShieldBNRoofLayers",8);
  Control.addVariable("dreamShieldBWallLen1",20.0);
  Control.addVariable("dreamShieldBWallMat1","CastIron");
  Control.addVariable("dreamShieldBWallMat5","Concrete");

  Control.addVariable("dreamShieldBRoofLen1",20.0);
  Control.addVariable("dreamShieldBFloorLen1",20.0);
  

  // HUT:
  Control.addVariable("dreamCaveYStep",0.0);
  Control.addVariable("dreamCaveVoidFront",60.0);
  Control.addVariable("dreamCaveVoidHeight",300.0);
  Control.addVariable("dreamCaveVoidDepth",183.0);
  Control.addVariable("dreamCaveVoidWidth",400.0);
  Control.addVariable("dreamCaveVoidLength",1600.0);


  Control.addVariable("dreamCaveFeFront",25.0);
  Control.addVariable("dreamCaveFeLeftWall",15.0);
  Control.addVariable("dreamCaveFeRightWall",15.0);
  Control.addVariable("dreamCaveFeRoof",15.0);
  Control.addVariable("dreamCaveFeFloor",15.0);
  Control.addVariable("dreamCaveFeBack",15.0);

  Control.addVariable("dreamCaveConcFront",35.0);
  Control.addVariable("dreamCaveConcLeftWall",35.0);
  Control.addVariable("dreamCaveConcRightWall",35.0);
  Control.addVariable("dreamCaveConcRoof",35.0);
  Control.addVariable("dreamCaveConcFloor",50.0);
  Control.addVariable("dreamCaveConcBack",35.0);

  Control.addVariable("dreamCaveFeMat","Stainless304");
  Control.addVariable("dreamCaveConcMat","Concrete");

  // Guide after wall [+17.5m] after section 1  
  generatePipe(Control,"dreamPipeCaveA",500.0);
  Control.addVariable("dreamPipeCaveABRadius",6.0);
  Control.addVariable("dreamPipeCaveAYStep",0.0);

  generateFocusTaper(Control,"dreamFCaveA",496,4.0,4.0,5.0,5.0);
  Control.addVariable("dreamFCaveALayerMat1","Glass");
  
  return;
}
 
}  // NAMESPACE setVariable
