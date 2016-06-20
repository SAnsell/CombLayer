/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   photon/PhotonVariables.cxx
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

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "variableSetup.h"

namespace setVariable
{

void
PhotonVariables(FuncDataBase& Control)
  /*!
    Function to set the control variables and constants
    -- This version is for Photon Moderator
    \param Control :: Function data base to add constants too
  */
{
  ELog::RegMethod RegA("PhotonVariables[F]","PhotonVariables");

  Control.addVariable("laserSourceParticleType",1);     

  Control.addVariable("laserSourceXStep",0.0);       
  Control.addVariable("laserSourceYStep",0.0);       
  Control.addVariable("laserSourceZStep",0.0);       
  Control.addVariable("laserSourceRadius",3.0);       
  Control.addVariable("laserSourceASpread",3.0);       
  Control.addVariable("laserSourceEStart",3.0);       
  Control.addVariable("laserSourceNE",10);       
  Control.addVariable("laserSourceEEnd",80.0);     
  Control.addVariable("laserSourceEnergy","0.8 1.0 2.0 4.0 8.0 10.0");
  Control.addVariable("laserSourceEProb"," 2e9 1e9 8e8 4e8 2e8 1e8");
    
  Control.addVariable("OuterXStep",0.0);
  Control.addVariable("OuterYStep",0.0);
  Control.addVariable("OuterZStep",0.0);
  Control.addVariable("OuterXYangle",0.0);
  Control.addVariable("OuterZangle",0.0);
  Control.addVariable("OuterNLayers",2);

  Control.addVariable("Outer0Radius",2.5);
  Control.addVariable("Outer1Radius",3.7);

  Control.addVariable("OuterHeight",8.0);
  Control.addVariable("Outer0Mat","Lead");
  Control.addVariable("Outer1Mat","Aluminium");
  
  Control.addVariable("PrimModXStep",0.0);
  Control.addVariable("PrimModYStep",0.0);
  Control.addVariable("PrimModZStep",0.0);
  Control.addVariable("PrimModXYangle",0.0);
  Control.addVariable("PrimModZangle",0.0);
  Control.addVariable("PrimModNLayers",4);

  Control.addVariable("PrimModOuterRadius",2.05);

  // Impossibly thin Be coat
  Control.addVariable("PrimMod0NUnits",1); 
  Control.addVariable("PrimMod0Thick",0.3);
  Control.addVariable("PrimMod0Mat0","Aluminium");

  // Tungsten centre layer
  Control.addVariable("PrimMod1NUnits",1);
  Control.addVariable("PrimMod1Thick",2.35);
  Control.addVariable("PrimMod1Mat0","Lead");

  // Carbon
  Control.addVariable("PrimMod2NUnits",1);
  Control.addVariable("PrimMod2Thick",1.20);
  Control.addVariable("PrimMod2Mat0","Tungsten");

  // Poly
  Control.addVariable("PrimMod3NUnits",1);
  Control.addVariable("PrimMod3Thick",4.0);
  Control.addVariable("PrimMod3Mat0","Poly");

  // // Impossibly thin Be coat
  // Control.addVariable("PrimMod2NUnits",1); 
  // Control.addVariable("PrimMod2Thick",0.025);
  // Control.addVariable("PrimMod2Mat0","Be300K");

  // // Void Gap 
  // Control.addVariable("PrimMod3NUnits",1); 
  // Control.addVariable("PrimMod3Thick",0.025);
  // Control.addVariable("PrimMod3Mat0","Void");

  // Zr layer
  Control.addVariable("PrimMod4NUnits",1); 
  Control.addVariable("PrimMod4Thick",1.225);
  Control.addVariable("PrimMod4Mat0","ZrH2");

  // Next Void Gap 
  Control.addVariable("PrimMod5CopyCell",3); 
  // Be Layer 
  Control.addVariable("PrimMod6CopyCell",2); 
  // W Layer 
  Control.addVariable("PrimMod7CopyCell",1); 
  // Be Layer 
  Control.addVariable("PrimMod8CopyCell",2); 
    //  Void Gap 
  Control.addVariable("PrimMod9CopyCell",3);
  //  ZrH
  Control.addVariable("PrimMod10CopyCell",4);

  
  // Next Void Gap 
  Control.addVariable("PrimMod11CopyCell",3); 
  // Be Layer 
  Control.addVariable("PrimMod12CopyCell",2); 
  // W Layer 
  Control.addVariable("PrimMod13CopyCell",1); 
  // Be Layer 
  Control.addVariable("PrimMod14CopyCell",2); 
    //  Void Gap 
  Control.addVariable("PrimMod15CopyCell",3);
  //  ZrH
  Control.addVariable("PrimMod16CopyCell",4);

  // Next Void Gap 
  Control.addVariable("PrimMod17CopyCell",3); 
  // Be Layer 
  Control.addVariable("PrimMod18CopyCell",2); 
  // W Layer 
  Control.addVariable("PrimMod19CopyCell",1); 
  // Be Layer 
  Control.addVariable("PrimMod20CopyCell",2); 
    //  Void Gap 
  Control.addVariable("PrimMod21CopyCell",3);
    
  Control.addVariable("CarbonXStep",0.0);
  Control.addVariable("CarbonYStep",0.0);
  Control.addVariable("CarbonZStep",0.0);
  Control.addVariable("CarbonXYangle",0.0);
  Control.addVariable("CarbonZangle",0.0);
  Control.addVariable("CarbonNLayers",3);
  Control.addVariable("CarbonOuterRadius",5.05);
  
    // Impossibly thin Be coat
  Control.addVariable("Carbon0NUnits",1); 
  Control.addVariable("Carbon0Thick",0.025);
  Control.addVariable("Carbon0Mat0","Be300K");
  // Graphite centre layer
  Control.addVariable("Carbon1NUnits",2);
  Control.addVariable("Carbon1Thick",1.225);
  Control.addVariable("Carbon1Radius0",4.0);
  Control.addVariable("Carbon1Mat0","Graphite");
  Control.addVariable("Carbon1Mat1","Graphite");

  //  Be Layer
  Control.addVariable("Carbon2CopyCell",0);

  // Moderator
  Control.addVariable("D2OModXStep",0.0);
  Control.addVariable("D2OModYStep",0.0);
  Control.addVariable("D2OModZStep",0.0);
  Control.addVariable("D2OModXYangle",0.0);
  Control.addVariable("D2OModZangle",0.0);
  Control.addVariable("D2OModOuterRadius",5.05);
  Control.addVariable("D2OModOuterHeight",21.275);
  Control.addVariable("D2OModOuterMat","Void");
  Control.addVariable("D2OModNTubes",0);

  Control.addVariable("D2OMod0Offset",Geometry::Vec3D(0,0,0));
  Control.addVariable("D2OMod0Radius",2.35);
  Control.addVariable("D2OMod0Height",7.03);
  Control.addVariable("D2OMod0Mat","Void");


  Control.addVariable("D2OMod1Offset",Geometry::Vec3D(-3.7,0,0));
  Control.addVariable("D2OMod1Radius",1.29);
  Control.addVariable("D2OMod1Height",7.03);
  Control.addVariable("D2OMod1Mat","Be300K");

  Control.addVariable("D2OMod2Offset",Geometry::Vec3D(3.7,0,0));
  Control.addVariable("D2OMod2Radius",1.29);
  Control.addVariable("D2OMod2Height",7.03);
  Control.addVariable("D2OMod2Mat","Be300K");


  // PHOTONMOD2:
  
  Control.addVariable("PModOuterHeight",7.0);
  Control.addVariable("PModOuterWidth",7.0);
  Control.addVariable("PModInnerHeight",5.0);
  Control.addVariable("PModInnerWidth",5.0);
  Control.addVariable("PModOuterMat","Stainless304");
  Control.addVariable("PModNLayer",3);

  Control.addVariable("PModThick0",0.3);
  Control.addVariable("PModThick1",1.7);
  Control.addVariable("PModThick2",1.0);

  Control.addVariable("PModVHeight0",3.0);
  Control.addVariable("PModVHeight1",0.5);
  Control.addVariable("PModVHeight2",0.0);

  Control.addVariable("PModVWidth0",3.0);
  Control.addVariable("PModVWidth1",0.5);
  Control.addVariable("PModVWidth2",0.0);

  Control.addVariable("PModMat0","Lead");
  Control.addVariable("PModMat1","Lead");
  Control.addVariable("PModMat2","Poly");

  Control.addVariable("TPlateYStep",22.0);
  Control.addVariable("TPlateHeight",5.40);
  Control.addVariable("TPlateWidth",5.40);
  Control.addVariable("TPlateDepth",0.1);
  Control.addVariable("TPlateDefMat","Void");

  Control.addVariable("THoldYStep",22.0);
  Control.addVariable("THoldRadius",6.0);
  Control.addVariable("THoldDefMat","Void");

  Control.addVariable("laserSourceShape",1);       
  Control.addVariable("laserSourceWidth",3.0);
  Control.addVariable("laserSourceHeight",3.0);       


  
  
  return;
}

}  // NAMESPACE setVariable
