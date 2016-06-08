/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:    ESSBeam/vespa/VESPAvariables.cxx
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
#include "ShieldGenerator.h"
#include "FocusGenerator.h"
#include "ChopperGenerator.h"

namespace setVariable
{

 
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
  ELog::RegMethod RegA("VESPAvariables[F]","generatePipe");
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
generateRectangle(FuncDataBase& Control,
                  const std::string& keyName,
                  const double length,
                  const double H,const double V)
                   
  /*!
    Create general rectangle
    \param Control :: Data Base for variables
    \param keyName :: main name
    \param length :: length of pipe
    \param H :: horrizontal gap
    \param V :: Vertical gap
   */
{
  ELog::RegMethod RegA("VESPAvariables[F]","generateRectangle");

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

  Control.addVariable(keyName+"LayerThick1",0.5);  // glass thick
  Control.addVariable(keyName+"LayerThick2",1.5);

  Control.addVariable(keyName+"LayerMat0","Void");
  Control.addVariable(keyName+"LayerMat1","Aluminium");
  Control.addVariable(keyName+"LayerMat2","Void");       
  
  Control.addVariable(keyName+"0TypeID","Rectangle");
  Control.addVariable(keyName+"0Height",V);
  Control.addVariable(keyName+"0Width",H);
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
  ELog::RegMethod RegA("VespaVARIABLES","generateT0Chopper");

  setVariable::ChopperGenerator CGen;
  CGen.generateChopper(Control,chopperName,25.0,36.0,32.0);

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
VESPAvariables(FuncDataBase& Control)
  /*!
    Create all the beamline variables for vor
    \param Control :: DataBase
  */
{
  ELog::RegMethod RegA("VESPAvariables[F]","VESPAvariables");

  setVariable::FocusGenerator FGen;
  setVariable::ShieldGenerator SGen;
  SGen.addWall(1,20.0,"CastIron");
  SGen.addRoof(1,20.0,"CastIron");
  SGen.addFloor(1,20.0,"CastIron");
  SGen.addFloorMat(5,"Concrete");
  SGen.addRoofMat(5,"Concrete");
  SGen.addWallMat(5,"Concrete");
  
  // extent of beamline
  Control.addVariable("vespaStopPoint",0);
  
  // Bender in section so use cut system
  Control.addVariable("vespaFAXStep",0.0);        // Centre of thermal [-2]
  Control.addVariable("vespaFAYStep",0.0);       
  Control.addVariable("vespaFAZStep",0.0);       
  Control.addVariable("vespaFAXYAngle",0.0);
  Control.addVariable("vespaFAZAngle",0.0);
  Control.addVariable("vespaFABeamXYAngle",0.0);       

  Control.addVariable("vespaFALength",350.0);       
  Control.addVariable("vespaFANShapes",1);       
  Control.addVariable("vespaFANShapeLayers",3);
  Control.addVariable("vespaFAActiveShield",0);

  Control.addVariable("vespaFALayerThick1",1.0);  // glass thick
  Control.addVariable("vespaFALayerThick2",1.5);

  Control.addVariable("vespaFALayerMat0","Void");
  Control.addVariable("vespaFALayerMat1","Copper");
  Control.addVariable("vespaFALayerMat2","Void");       

  Control.addVariable("vespaFA0TypeID","Taper");
  Control.addVariable("vespaFA0HeightStart",5.0);  
  Control.addVariable("vespaFA0HeightEnd",7.6250);
  Control.addVariable("vespaFA0WidthStart",7.6); 
  Control.addVariable("vespaFA0WidthEnd",4.01);
  Control.addVariable("vespaFA0Length",350.0);

  generatePipe(Control,"vespaPipeA",46.0);
  FGen.generateTaper(Control,"vespaFB",44.0,4.0,3.5,7.6,8.0);   
  
  // VACBOX A : 6.10m target centre
  //  Length 100.7 + Width [87.0] + Height [39.0] void Depth/2 + front
  setVariable::ChopperGenerator CGen;
  CGen.generateChopper(Control,"vespaChopperA",8.0,12.0,5.55);
  
  // Double Blade chopper
  Control.addVariable("vespaWFMBladeAXStep",0.0);
  Control.addVariable("vespaWFMBladeAYStep",0.0);
  Control.addVariable("vespaWFMBladeAZStep",0.0);
  Control.addVariable("vespaWFMBladeAXYangle",0.0);
  Control.addVariable("vespaWFMBladeAZangle",0.0);

  Control.addVariable("vespaWFMBladeAGap",3.0);
  Control.addVariable("vespaWFMBladeAInnerRadius",25.0);
  Control.addVariable("vespaWFMBladeAOuterRadius",35.5);
  Control.addVariable("vespaWFMBladeANDisk",2);

  Control.addVariable("vespaWFMBladeA0Thick",0.2);
  Control.addVariable("vespaWFMBladeA1Thick",0.2);
  Control.addVariable("vespaWFMBladeAInnerMat","Inconnel");
  Control.addVariable("vespaWFMBladeAOuterMat","B4C");
  
  Control.addVariable("vespaWFMBladeANBlades",3);
  Control.addVariable("vespaWFMBladeA0PhaseAngle0",0.0);
  Control.addVariable("vespaWFMBladeA0OpenAngle0",24.31);
  Control.addVariable("vespaWFMBladeA0PhaseAngle1",90.0);
  Control.addVariable("vespaWFMBladeA0OpenAngle1",40.98);
  Control.addVariable("vespaWFMBladeA0PhaseAngle2",220.0);
  Control.addVariable("vespaWFMBladeA0OpenAngle2",57.32);

  Control.addVariable("vespaWFMBladeA1PhaseAngle0",0.0+30.0);
  Control.addVariable("vespaWFMBladeA1OpenAngle0",24.31);
  Control.addVariable("vespaWFMBladeA1PhaseAngle1",90.0+30.0);
  Control.addVariable("vespaWFMBladeA1OpenAngle1",40.98);
  Control.addVariable("vespaWFMBladeA1PhaseAngle2",220.0+30.0);
  Control.addVariable("vespaWFMBladeA1OpenAngle2",57.32);

  generatePipe(Control,"vespaPipeC",20.0);
  FGen.generateTaper(Control,"vespaFC",16.0,3.7,4.0,8.0,8.0);   
  //  Control.addVariable("vespaFBBeamYStep",4.0);


  // VACBOX A : 6.10m target centre
  //  Length 100.7 + Width [87.0] + Height [39.0] void Depth/2 + front
  CGen.generateChopper(Control,"vespaChopperB",9.0,12.0,5.55);
  
  // Double Blade chopper
  Control.addVariable("vespaWFMBladeBXStep",0.0);
  Control.addVariable("vespaWFMBladeBYStep",0.0);
  Control.addVariable("vespaWFMBladeBZStep",0.0);
  Control.addVariable("vespaWFMBladeBXYangle",0.0);
  Control.addVariable("vespaWFMBladeBZangle",0.0);

  Control.addVariable("vespaWFMBladeBGap",3.0);
  Control.addVariable("vespaWFMBladeBInnerRadius",25.0);
  Control.addVariable("vespaWFMBladeBOuterRadius",35.5);
  Control.addVariable("vespaWFMBladeBNDisk",2);

  Control.addVariable("vespaWFMBladeB0Thick",0.2);
  Control.addVariable("vespaWFMBladeB1Thick",0.2);
  Control.addVariable("vespaWFMBladeBInnerMat","Inconnel");
  Control.addVariable("vespaWFMBladeBOuterMat","B4C");
  
  Control.addVariable("vespaWFMBladeBNBlades",3);
  Control.addVariable("vespaWFMBladeB0PhaseAngle0",0.0);
  Control.addVariable("vespaWFMBladeB0OpenAngle0",24.31);
  Control.addVariable("vespaWFMBladeB0PhaseAngle1",90.0);
  Control.addVariable("vespaWFMBladeB0OpenAngle1",40.98);
  Control.addVariable("vespaWFMBladeB0PhaseAngle2",220.0);
  Control.addVariable("vespaWFMBladeB0OpenAngle2",57.32);

  Control.addVariable("vespaWFMBladeB1PhaseAngle0",0.0+30.0);
  Control.addVariable("vespaWFMBladeB1OpenAngle0",24.31);
  Control.addVariable("vespaWFMBladeB1PhaseAngle1",90.0+30.0);
  Control.addVariable("vespaWFMBladeB1OpenAngle1",40.98);
  Control.addVariable("vespaWFMBladeB1PhaseAngle2",220.0+30.0);
  Control.addVariable("vespaWFMBladeB1OpenAngle2",57.32);


  generatePipe(Control,"vespaPipeD",60.0);
  generateRectangle(Control,"vespaFD",56.0,4.0,8.0);   
  //  Control.addVariable("vespaFBBeamYStep",4.0);

    // VACBOX A : 6.10m target centre
  //  Length 100.7 + Width [87.0] + Height [39.0] void Depth/2 + front
  CGen.generateChopper(Control,"vespaChopperC",9.0,12.0,5.55);
  
  // Double Blade chopper
  Control.addVariable("vespaWFMBladeCXStep",0.0);
  Control.addVariable("vespaWFMBladeCYStep",0.0);
  Control.addVariable("vespaWFMBladeCZStep",0.0);
  Control.addVariable("vespaWFMBladeCXYangle",0.0);
  Control.addVariable("vespaWFMBladeCZangle",0.0);

  Control.addVariable("vespaWFMBladeCGap",3.0);
  Control.addVariable("vespaWFMBladeCInnerRadius",25.0);
  Control.addVariable("vespaWFMBladeCOuterRadius",35.5);
  Control.addVariable("vespaWFMBladeCNDisk",2);

  Control.addVariable("vespaWFMBladeC0Thick",0.2);
  Control.addVariable("vespaWFMBladeC1Thick",0.2);
  Control.addVariable("vespaWFMBladeCInnerMat","Inconnel");
  Control.addVariable("vespaWFMBladeCOuterMat","B4C");
  
  Control.addVariable("vespaWFMBladeCNBlades",3);
  Control.addVariable("vespaWFMBladeC0PhaseAngle0",0.0);
  Control.addVariable("vespaWFMBladeC0OpenAngle0",24.31);
  Control.addVariable("vespaWFMBladeC0PhaseAngle1",90.0);
  Control.addVariable("vespaWFMBladeC0OpenAngle1",40.98);
  Control.addVariable("vespaWFMBladeC0PhaseAngle2",220.0);
  Control.addVariable("vespaWFMBladeC0OpenAngle2",57.32);

  Control.addVariable("vespaWFMBladeC1PhaseAngle0",0.0+30.0);
  Control.addVariable("vespaWFMBladeC1OpenAngle0",24.31);
  Control.addVariable("vespaWFMBladeC1PhaseAngle1",90.0+30.0);
  Control.addVariable("vespaWFMBladeC1OpenAngle1",40.98);
  Control.addVariable("vespaWFMBladeC1PhaseAngle2",220.0+30.0);
  Control.addVariable("vespaWFMBladeC1OpenAngle2",57.32);

  generatePipe(Control,"vespaPipeE",270.0);
  FGen.generateTaper(Control,"vespaFE",266.0,4.0,7.5,8.0,7.5);   
  //  Control.addVariable("vespaFBBeamYStep",4.0);

  // VACBOX
  CGen.generateChopper(Control,"vespaChopperD",9.0,12.0,5.55);


  // Double Blade chopper
  Control.addVariable("vespaFOCBladeAXStep",0.0);
  Control.addVariable("vespaFOCBladeAYStep",0.0);
  Control.addVariable("vespaFOCBladeAZStep",0.0);
  Control.addVariable("vespaFOCBladeAXYangle",0.0);
  Control.addVariable("vespaFOCBladeAZangle",0.0);

  Control.addVariable("vespaFOCBladeAGap",3.0);
  Control.addVariable("vespaFOCBladeAInnerRadius",25.0);
  Control.addVariable("vespaFOCBladeAOuterRadius",35.5);
  Control.addVariable("vespaFOCBladeANDisk",2);

  Control.addVariable("vespaFOCBladeA0Thick",0.2);
  Control.addVariable("vespaFOCBladeA1Thick",0.2);
  Control.addVariable("vespaFOCBladeAInnerMat","Inconnel");
  Control.addVariable("vespaFOCBladeAOuterMat","B4C");
  
  Control.addVariable("vespaFOCBladeANBlades",1);
  Control.addVariable("vespaFOCBladeA0PhaseAngle0",0.0);
  Control.addVariable("vespaFOCBladeA0OpenAngle0",320.12);

  Control.addVariable("vespaFOCBladeA1PhaseAngle0",0.0+30.0);
  Control.addVariable("vespaFOCBladeA1OpenAngle0",320.12);

  generatePipe(Control,"vespaPipeF",135.0);
  FGen.generateTaper(Control,"vespaFF",130.0,8.0,7.5,9.0,8.5);  // NOT CORRECT

  //  Control.addVariable("vespaFBBeamYStep",4.0);
  
  // BEAM INSERT:
  Control.addVariable("vespaBInsertHeight",20.0);
  Control.addVariable("vespaBInsertWidth",28.0);
  Control.addVariable("vespaBInsertTopWall",1.0);
  Control.addVariable("vespaBInsertLowWall",1.0);
  Control.addVariable("vespaBInsertLeftWall",1.0);
  Control.addVariable("vespaBInsertRightWall",1.0);
  Control.addVariable("vespaBInsertWallMat","Stainless304");       

  // Guide in wall
  FGen.generateTaper(Control,"vespaFWall",308.0,9.0,9.0,8.5,8.5);

  SGen.generateShield(Control,"vespaShieldA",450.0,40.0,40.0,40.0,4,8);

  // Guide after wall [17.5m - 3.20] for wall
  generatePipe(Control,"vespaPipeOutA",435.0);  //
  Control.addVariable("vespaPipeOutARadius",9.0);
  FGen.generateTaper(Control,"vespaFOutA",420.0,9.0,11.0,8.5,10.0);


  Control.addVariable("vespaPitBYStep",-60.5);

  Control.addVariable("vespaPitBVoidHeight",167.0);
  Control.addVariable("vespaPitBVoidDepth",36.0);
  Control.addVariable("vespaPitBVoidWidth",246.0);
  Control.addVariable("vespaPitBVoidLength",105.0);
  
  Control.addVariable("vespaPitBFeHeight",70.0);
  Control.addVariable("vespaPitBFeDepth",60.0);
  Control.addVariable("vespaPitBFeWidth",60.0);
  Control.addVariable("vespaPitBFeFront",45.0);
  Control.addVariable("vespaPitBFeBack",70.0);
  Control.addVariable("vespaPitBFeMat","Stainless304");
  
  Control.addVariable("vespaPitBConcHeight",50.0);
  Control.addVariable("vespaPitBConcDepth",50.0);
  Control.addVariable("vespaPitBConcWidth",50.0);
  Control.addVariable("vespaPitBConcFront",50.0);
  Control.addVariable("vespaPitBConcBack",50.0);
  Control.addVariable("vespaPitBConcMat","Concrete");

  Control.addVariable("vespaPitBColletHeight",15.0);
  Control.addVariable("vespaPitBColletDepth",15.0);
  Control.addVariable("vespaPitBColletWidth",40.0);
  Control.addVariable("vespaPitBColletLength",5.0);
  Control.addVariable("vespaPitBColletMat","Tungsten");

  CGen.generateChopper(Control,"vespaChopperOutB",18.0,12.0,5.55);
  // Double Blade chopper
  Control.addVariable("vespaFOCBladeBXStep",0.0);
  Control.addVariable("vespaFOCBladeBYStep",0.0);
  Control.addVariable("vespaFOCBladeBZStep",0.0);
  Control.addVariable("vespaFOCBladeBXYangle",0.0);
  Control.addVariable("vespaFOCBladeBZangle",0.0);

  Control.addVariable("vespaFOCBladeBGap",3.0);
  Control.addVariable("vespaFOCBladeBInnerRadius",25.0);
  Control.addVariable("vespaFOCBladeBOuterRadius",35.5);
  Control.addVariable("vespaFOCBladeBNDisk",2);

  Control.addVariable("vespaFOCBladeB0Thick",0.2);
  Control.addVariable("vespaFOCBladeB1Thick",0.2);
  Control.addVariable("vespaFOCBladeBInnerMat","Inconnel");
  Control.addVariable("vespaFOCBladeBOuterMat","B4C");
  
  Control.addVariable("vespaFOCBladeBNBlades",1);
  Control.addVariable("vespaFOCBladeB0PhaseAngle0",0.0);
  Control.addVariable("vespaFOCBladeB0OpenAngle0",320.12);

  Control.addVariable("vespaFOCBladeB1PhaseAngle0",0.0+30.0);
  Control.addVariable("vespaFOCBladeB1OpenAngle0",320.12);


  // Guide after wall [17.5m - 3.20] for wall
  generatePipe(Control,"vespaPipeOutB",850.0);  //
  Control.addVariable("vespaPipeOutBRadius",9.0);
  FGen.generateTaper(Control,"vespaFOutB",746.0,9.0,11.0,8.5,10.0);

  SGen.generateShield(Control,"vespaShieldB",770.0,40.0,40.0,40.0,4,8);

  for(size_t i=0;i<4;i++)
    {
      const std::string
        shieldName("vespaShieldArray"+StrFunc::makeString(i));
      const std::string
        vacName("vespaVPipeArray"+StrFunc::makeString(i));
      const std::string
        focusName("vespaFocusArray"+StrFunc::makeString(i));

      SGen.generateShield(Control,shieldName,600.0,40.0,40.0,40.0,4,8);
      generatePipe(Control,vacName,598.0);  //
      generateRectangle(Control,focusName,594.0,10.0,10.0);
    }

  
  
  return;
}
 
}  // NAMESPACE setVariable
