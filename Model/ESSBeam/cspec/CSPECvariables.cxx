/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:    ESSBeam/cspec/CSPECvariables.cxx
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
   */
{
  ELog::RegMethod RegA("CSPECvariables[F]","generateChopper");
  
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
  ELog::RegMethod RegA("CSPECvariables[F]","generatePipe");
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
  ELog::RegMethod RegA("CSPECvariables[F]","generateFocusTaper");

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
  ELog::RegMethod RegA("CspecVARIABLES","generateT0Chopper");
  
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
CSPECvariables(FuncDataBase& Control)
  /*!
    Create all the beamline variables for vor
    \param Control :: DataBase
  */
{
  ELog::RegMethod RegA("CSPECvariables[F]","CSPECvariables");

  // extent of beamline
  Control.addVariable("cspecStopPoint",0);
  
  // Bender in section so use cut system
  Control.addVariable("cspecFAXStep",0.0);        // Centre of thermal [-2]
  Control.addVariable("cspecFAYStep",0.0);       
  Control.addVariable("cspecFAZStep",0.0);       
  Control.addVariable("cspecFAXYAngle",0.0);
  Control.addVariable("cspecFAZAngle",0.0);
  Control.addVariable("cspecFABeamXYAngle",0.0);       

  Control.addVariable("cspecFALength",350.0);       
  Control.addVariable("cspecFANShapes",1);       
  Control.addVariable("cspecFANShapeLayers",3);
  Control.addVariable("cspecFAActiveShield",0);

  Control.addVariable("cspecFALayerThick1",1.0);  // glass thick
  Control.addVariable("cspecFALayerThick2",1.5);

  Control.addVariable("cspecFALayerMat0","Void");
  Control.addVariable("cspecFALayerMat1","Copper");
  Control.addVariable("cspecFALayerMat2","Void");       

  Control.addVariable("cspecFA0TypeID","Taper");
  Control.addVariable("cspecFA0HeightStart",3.5); // guess
  Control.addVariable("cspecFA0HeightEnd",4.5);
  Control.addVariable("cspecFA0WidthStart",8.0); // NOT Centred
  Control.addVariable("cspecFA0WidthEnd",2.0);
  Control.addVariable("cspecFA0Length",350.0);

  generatePipe(Control,"cspecPipeA",46.0);
  generateFocusTaper(Control,"cspecFB",44.0,1.88,2.06,4.6,4.5);   
  //  Control.addVariable("cspecFBBeamYStep",4.0);


  // Bender in section so use cut system  
  Control.addVariable("cspecBBXStep",0.0);       
  Control.addVariable("cspecBBYStep",0.0);       
  Control.addVariable("cspecBBZStep",0.0);       
  Control.addVariable("cspecBBXYAngle",0.0);
  Control.addVariable("cspecBBZAngle",0.0);
  Control.addVariable("cspecBBBeamXYAngle",0.0);       

  Control.addVariable("cspecBBLength",400.0);       
  Control.addVariable("cspecBBNShapes",1);       
  Control.addVariable("cspecBBNShapeLayers",3);
  Control.addVariable("cspecBBActiveShield",0);

  Control.addVariable("cspecBBLayerThick1",0.4);  // glass thick
  Control.addVariable("cspecBBLayerThick2",1.5);

  Control.addVariable("cspecBBLayerMat0","Void");
  Control.addVariable("cspecBBLayerMat1","Aluminium");
  Control.addVariable("cspecBBLayerMat2","Void");       

  Control.addVariable("cspecBB0TypeID","Bend");
  Control.addVariable("cspecBB0AHeight",3.0);
  Control.addVariable("cspecBB0BHeight",3.0);
  Control.addVariable("cspecBB0AWidth",3.0);
  Control.addVariable("cspecBB0BWidth",3.0);
  Control.addVariable("cspecBB0Length",400.0);
  Control.addVariable("cspecBB0AngDir",0.0);
  Control.addVariable("cspecBB0Radius",20000.0);

  generatePipe(Control,"cspecPipeB",300.0);
  
  // VACBOX A : 6.10m target centre
  //  Length 100.7 + Width [87.0] + Height [39.0] void Depth/2 + front
  generateChopper(Control,"cspecChopperA",55.0,9.0,3.55);
  
  // Double Blade chopper
  Control.addVariable("cspecDBladeXStep",0.0);
  Control.addVariable("cspecDBladeYStep",0.0);
  Control.addVariable("cspecDBladeZStep",0.0);
  Control.addVariable("cspecDBladeXYangle",0.0);
  Control.addVariable("cspecDBladeZangle",0.0);

  Control.addVariable("cspecDBladeGap",1.0);
  Control.addVariable("cspecDBladeInnerRadius",22.5);
  Control.addVariable("cspecDBladeOuterRadius",33.5);
  Control.addVariable("cspecDBladeNDisk",2);

  Control.addVariable("cspecDBlade0Thick",0.2);
  Control.addVariable("cspecDBlade1Thick",0.2);
  Control.addVariable("cspecDBladeInnerMat","Inconnel");
  Control.addVariable("cspecDBladeOuterMat","B4C");
  
  Control.addVariable("cspecDBladeNBlades",2);
  Control.addVariable("cspecDBlade0PhaseAngle0",95.0);
  Control.addVariable("cspecDBlade0OpenAngle0",30.0);
  Control.addVariable("cspecDBlade1PhaseAngle0",95.0);
  Control.addVariable("cspecDBlade1OpenAngle0",30.0);

  Control.addVariable("cspecDBlade0PhaseAngle1",275.0);
  Control.addVariable("cspecDBlade0OpenAngle1",30.0);
  Control.addVariable("cspecDBlade1PhaseAngle1",275.0);
  Control.addVariable("cspecDBlade1OpenAngle1",30.0);

  generateChopper(Control,"cspecChopperB",5.0,9.0,3.55);

  // Single Blade chopper
  Control.addVariable("cspecSBladeXStep",0.0);
  Control.addVariable("cspecSBladeYStep",0.0);
  Control.addVariable("cspecSBladeZStep",0.0);
  Control.addVariable("cspecSBladeXYangle",0.0);
  Control.addVariable("cspecSBladeZangle",0.0);

  Control.addVariable("cspecSBladeInnerRadius",28.0);
  Control.addVariable("cspecSBladeOuterRadius",33.0);
  Control.addVariable("cspecSBladeNDisk",1);

  Control.addVariable("cspecSBlade0Thick",0.2);
  Control.addVariable("cspecSBladeInnerMat","Inconnel");
  Control.addVariable("cspecSBladeOuterMat","B4C");
  
  Control.addVariable("cspecSBladeNBlades",2);
  Control.addVariable("cspecSBlade0PhaseAngle0",95.0);
  Control.addVariable("cspecSBlade0OpenAngle0",30.0);
  Control.addVariable("cspecSBlade0PhaseAngle1",275.0);
  Control.addVariable("cspecSBlade0OpenAngle1",30.0);

  
  // VACUUM PIPE: SDisk to T0 (A)
  generatePipe(Control,"cspecPipeB",10.0);
  generateFocusTaper(Control,"cspecFC",8.8,2.06,2.36,4.6,4.5);   
  Control.addVariable("cspecFCBeamYStep",1.10); 

  generateChopper(Control,"cspecChopperC",20.0,36.0,32.0);
  generateT0Chopper(Control,"cspecT0Disk","cspecFT0Mid","cspecChopperC");

  // VACUUM PIPE: SDisk to T0 (A)
  generatePipe(Control,"cspecPipeD",130.0);
  generateFocusTaper(Control,"cspecFD",122.0,2.06,2.36,4.6,4.5); 

  generateChopper(Control,"cspecChopperD",10.0,9.0,3.55);

  Control.addVariable("cspecBandADiskXStep",0.0);
  Control.addVariable("cspecBandADiskYStep",0.5);
  Control.addVariable("cspecBandADiskZStep",0.0);
  Control.addVariable("cspecBandADiskXYangle",0.0);
  Control.addVariable("cspecBandADiskZangle",0.0);

  Control.addVariable("cspecBandADiskInnerRadius",22.0);
  Control.addVariable("cspecBandADiskOuterRadius",30.0);
  Control.addVariable("cspecBandADiskNDisk",1);

  Control.addVariable("cspecBandADisk0Thick",0.5);  // to include B4C
  Control.addVariable("cspecBandADiskInnerMat","Inconnel");
  Control.addVariable("cspecBandADiskOuterMat","Void");
  
  Control.addVariable("cspecBandADiskNBlades",2);
  Control.addVariable("cspecBandADisk0PhaseAngle0",95.0);
  Control.addVariable("cspecBandADisk0OpenAngle0",35.0);
  Control.addVariable("cspecBandADisk0PhaseAngle1",275.0);
  Control.addVariable("cspecBandADisk0OpenAngle1",25.0);


  // VACUUM PIPE: SDisk to T0 (A)
  generatePipe(Control,"cspecPipeE",160.0);
  generateFocusTaper(Control,"cspecFE",152.0,2.06,2.36,4.6,4.5); 

  generateChopper(Control,"cspecChopperE",10.0,9.0,3.55);

  Control.addVariable("cspecBandBDiskXStep",0.0);
  Control.addVariable("cspecBandBDiskYStep",0.5);
  Control.addVariable("cspecBandBDiskZStep",0.0);
  Control.addVariable("cspecBandBDiskXYangle",0.0);
  Control.addVariable("cspecBandBDiskZangle",0.0);

  Control.addVariable("cspecBandBDiskInnerRadius",22.0);
  Control.addVariable("cspecBandBDiskOuterRadius",30.0);
  Control.addVariable("cspecBandBDiskNDisk",1);

  Control.addVariable("cspecBandBDisk0Thick",0.5);  // to include B4C
  Control.addVariable("cspecBandBDiskInnerMat","Inconnel");
  Control.addVariable("cspecBandBDiskOuterMat","Void");
  
  Control.addVariable("cspecBandBDiskNBlades",2);
  Control.addVariable("cspecBandBDisk0PhaseAngle0",95.0);
  Control.addVariable("cspecBandBDisk0OpenAngle0",35.0);
  Control.addVariable("cspecBandBDisk0PhaseAngle1",275.0);
  Control.addVariable("cspecBandBDisk0OpenAngle1",25.0);

  // VACUUM PIPE: SDisk to T0 (A)
  generatePipe(Control,"cspecPipeF",40.0);
  generateFocusTaper(Control,"cspecFF",32.0,2.06,2.36,4.6,4.5);
  
  generateT0Chopper(Control,"cspecT1Disk","cspecFT1Mid","cspecChopperG");


  // VACUUM PIPE: SDisk to T0 (A)
  generatePipe(Control,"cspecPipeG",800.0);
  generateFocusTaper(Control,"cspecFG",796.0,2.06,2.36,4.6,4.5);

  // VACUUM PIPE: SDisk to T0 (A)
  generatePipe(Control,"cspecPipeH",600.0);
  generateFocusTaper(Control,"cspecFH",566.0,2.06,2.36,4.6,4.5);

  // BEAM INSERT:
  Control.addVariable("cspecBInsertHeight",20.0);
  Control.addVariable("cspecBInsertWidth",28.0);
  Control.addVariable("cspecBInsertTopWall",1.0);
  Control.addVariable("cspecBInsertLowWall",1.0);
  Control.addVariable("cspecBInsertLeftWall",1.0);
  Control.addVariable("cspecBInsertRightWall",1.0);
  Control.addVariable("cspecBInsertWallMat","Stainless304");       

  // Guide in wall
  generateFocusTaper(Control,"cspecFWall",308.0,6.0,6.0,6.0,6.0);


  Control.addVariable("cspecShieldALength",1750.0-320);
  Control.addVariable("cspecShieldALeft",40.0);
  Control.addVariable("cspecShieldARight",40.0);
  Control.addVariable("cspecShieldAHeight",40.0);
  Control.addVariable("cspecShieldADepth",40.0);
  Control.addVariable("cspecShieldADefMat","Stainless304");
  Control.addVariable("cspecShieldANSeg",8);
  Control.addVariable("cspecShieldANWallLayers",8);
  Control.addVariable("cspecShieldANFloorLayers",3);
  Control.addVariable("cspecShieldANRoofLayers",8);
  Control.addVariable("cspecShieldAWallLen1",20.0);
  Control.addVariable("cspecShieldAWallMat1","CastIron");
  Control.addVariable("cspecShieldAWallMat5","Concrete");
  Control.addVariable("cspecShieldARoofLen1",20.0);
  Control.addVariable("cspecShieldAFloorLen1",20.0);

  // Guide after wall [17.5m - 3.20] for wall
  generatePipe(Control,"cspecPipeOutA",1430.0);  //
  Control.addVariable("cspecPipeOutARadius",6.0);
  //  Control.addVariable("cspecPipeOutAFlangeRadius",9.0);  

  generateFocusTaper(Control,"cspecFOutA",1422,4.5,4.97,2.24,3.05);

  // Guide after wall [+17.5m] after section 1  
  generatePipe(Control,"cspecPipeOutB",1750.0);
  Control.addVariable("cspecPipeOutBRadius",6.0);

  generateFocusTaper(Control,"cspecFOutB",1736,4.0,4.0,5.0,5.0);
  Control.addVariable("cspecFOutBLayerMat1","Glass");

  
  Control.addVariable("cspecShieldBLength",1750.0);
  Control.addVariable("cspecShieldBLeft",32.0);
  Control.addVariable("cspecShieldBRight",32.0);
  Control.addVariable("cspecShieldBHeight",32.0);
  Control.addVariable("cspecShieldBDepth",32.0);
  Control.addVariable("cspecShieldBDefMat","Stainless304");
  Control.addVariable("cspecShieldBNSeg",8);
  Control.addVariable("cspecShieldBNWallLayers",8);
  Control.addVariable("cspecShieldBNFloorLayers",3);
  Control.addVariable("cspecShieldBNRoofLayers",8);
  Control.addVariable("cspecShieldBWallLen1",20.0);
  Control.addVariable("cspecShieldBWallMat1","CastIron");
  Control.addVariable("cspecShieldBWallMat5","Concrete");

  Control.addVariable("cspecShieldBRoofLen1",20.0);
  Control.addVariable("cspecShieldBFloorLen1",20.0);
  

  // HUT:
  Control.addVariable("cspecCaveYStep",0.0);
  Control.addVariable("cspecCaveVoidFront",60.0);
  Control.addVariable("cspecCaveVoidHeight",300.0);
  Control.addVariable("cspecCaveVoidDepth",183.0);
  Control.addVariable("cspecCaveVoidWidth",400.0);
  Control.addVariable("cspecCaveVoidLength",1600.0);


  Control.addVariable("cspecCaveFeFront",25.0);
  Control.addVariable("cspecCaveFeLeftWall",15.0);
  Control.addVariable("cspecCaveFeRightWall",15.0);
  Control.addVariable("cspecCaveFeRoof",15.0);
  Control.addVariable("cspecCaveFeFloor",15.0);
  Control.addVariable("cspecCaveFeBack",15.0);

  Control.addVariable("cspecCaveConcFront",35.0);
  Control.addVariable("cspecCaveConcLeftWall",35.0);
  Control.addVariable("cspecCaveConcRightWall",35.0);
  Control.addVariable("cspecCaveConcRoof",35.0);
  Control.addVariable("cspecCaveConcFloor",50.0);
  Control.addVariable("cspecCaveConcBack",35.0);

  Control.addVariable("cspecCaveFeMat","Stainless304");
  Control.addVariable("cspecCaveConcMat","Concrete");

  
  return;
}
 
}  // NAMESPACE setVariable
