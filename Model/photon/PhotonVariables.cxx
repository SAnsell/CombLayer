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
  Control.addVariable("laserSourceYStep",-5.0);
  Control.addVariable("laserSourceZStep",0.0);       

  Control.addVariable("laserSourceShape",0);
  Control.addVariable("laserSourceRadius",0.1);       
  Control.addVariable("laserSourceASpread",3.0);       
  Control.addVariable("laserSourceEStart",1.0);       
  Control.addVariable("laserSourceNE",10);       
  //  Control.addVariable("laserSourceEEnd",80.0);     

  Control.addVariable("laserSourceEnergy",
		      "1.5743253000e-01 1.7496055000e-01 1.9447280000e-01 "
		      "2.1294111000e-01 2.2970930000e-01 2.5530586000e-01 "
		      "2.8806970000e-01 3.2498354000e-01 3.7220290000e-01 "
		      "4.4595280000e-01 5.2631220000e-01 6.4986813000e-01 "
		      "7.7863586000e-01 1.0051130000e+00 1.2597324000e+00 "
		      "1.5545493000e+00 2.0373990000e+00 2.4402770000e+00 "
		      "2.9213471000e+00 3.3458118000e+00 3.8306620000e+00 "
		      "4.2575135000e+00 4.9498553000e+00 5.6690580000e+00 "
		      "6.4911210000e+00 7.7779536000e+00 8.6461070000e+00 "
		      "9.4671930000e+00 9.6176280000e+00 ");
  Control.addVariable("laserSourceEProb",
		      "1.1414313932e+08 8.3536723912e+07 6.4857300459e+07 "
		      "4.3851755669e+07 2.8437764164e+07 3.1014027386e+07 "
		      "2.7031914640e+07 2.1761931266e+07 1.8958009430e+07 "
		      "1.8320349539e+07 1.2958970279e+07 1.1471860693e+07 "
		      "7.3973640814e+06 8.2518069286e+06 5.3420942641e+06 "
		      "4.2153288883e+06 4.0740240641e+06 2.3159351182e+06 "
		      "2.1770310225e+06 1.3081641259e+06 1.1205574185e+06 "
		      "7.0480772070e+05 7.6011160786e+05 5.3774016332e+05 "
		      "4.4995782718e+05 4.2542994596e+05 1.9541100800e+05 "
		      "1.3202298449e+05 1.9013627678e+04 ");

  // PHOTONMOD3:
  Control.addVariable("ChamberLength",300.0);
  Control.addVariable("ChamberRadius",150.0);
  Control.addVariable("ChamberThick",5.0);
  Control.addVariable("ChamberBackThick",5.0);
  Control.addVariable("ChamberDoorThick",5.0);
  Control.addVariable("ChamberDoorStep",8.0);
  Control.addVariable("ChamberMat","Stainless304");
  Control.addVariable("ChamberVoidMat","Void");
  
  Control.addVariable("MetalContLength",20.0);
  Control.addVariable("MetalContRadius",5.0);
  Control.addVariable("MetalContThick",1.0);
  Control.addVariable("MetalContMat","Aluminium");

  Control.addVariable("MetalContFFlangeNSection",1);
  Control.addVariable("MetalContFFlangeNTrack",12);
  Control.addParse<double>("MetalContFFlangeRadius",
			   "MetalContRadius+MetalContThick");
  Control.addVariable("MetalContFFlangeThick",1.0);
  Control.addVariable("MetalContFFlangeDeltaRad",2.0);
  Control.addVariable("MetalContFFlangeMat","Stainless304");
  Control.addVariable("MetalContFFlangeNBolts",5);
  Control.addVariable("MetalContFFlangeBoltRadius",0.5);
  Control.addVariable("MetalContFFlangeBoltMat","Copper");

  Control.addVariable("MetalContFFlangeWindowFlag",1);
  Control.addVariable("MetalContFFlangeWindowMat","Aluminium");
  Control.addVariable("MetalContFFlangeWindowThick",0.3);

  
  
  
  // PHOTONMOD2:
 
  Control.addVariable("PModOuterHeight",7.0);
  Control.addVariable("PModOuterWidth",7.0);
  Control.addVariable("PModInnerHeight",5.0);
  Control.addVariable("PModInnerWidth",5.0);
  Control.addVariable("PModOuterMat","Aluminium");

  Control.addVariable("PModNLayer",6);
  Control.addVariable("PModThick0",0.3);    // void
  Control.addVariable("PModThick1",2.5);    // lead
  Control.addVariable("PModThick2",1.2);    // W
  Control.addVariable("PModThick3",4.0);    // Polystyrene
  Control.addVariable("PModThick4",0.2);    // Aluminium [with hole]
  Control.addVariable("PModThick5",0.2);    // Void

  Control.addVariable("PModVHeight0",3.0);
  Control.addVariable("PModVHeight1",0.0);
  Control.addVariable("PModVHeight2",0.0);
  Control.addVariable("PModVHeight3",0.0);
  Control.addVariable("PModVHeight4",3.0);
  Control.addVariable("PModVHeight5",0.0);

  Control.addVariable("PModVWidth0",3.0);
  Control.addVariable("PModVWidth1",0.0);
  Control.addVariable("PModVWidth1",0.0);
  Control.addVariable("PModVWidth2",0.0);
  Control.addVariable("PModVWidth3",0.0);
  Control.addVariable("PModVWidth4",3.0);
  Control.addVariable("PModVWidth5",0.0);    

  Control.addVariable("PModMat0","Void");
  Control.addVariable("PModMat1","Lead");
  Control.addVariable("PModMat2","Tungsten");
  Control.addVariable("PModMat3","Polystyrene");
  Control.addVariable("PModMat4","Aluminium");
  Control.addVariable("PModMat5","Void");

  Control.addVariable("TPlateYStep",22.0);
  Control.addVariable("TPlateHeight",5.40);
  Control.addVariable("TPlateWidth",5.40);
  Control.addVariable("TPlateDepth",0.1);
  Control.addVariable("TPlateDefMat","Void");

  Control.addVariable("THoldYStep",22.0);
  Control.addVariable("THoldRadius",6.0);
  Control.addVariable("THoldDefMat","Void");

  // PHOTON ONE:

  
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

  return;
}

}  // NAMESPACE setVariable
