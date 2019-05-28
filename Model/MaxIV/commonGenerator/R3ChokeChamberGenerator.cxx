/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonGenerator/R3ChokeChamberGenerator.cxx
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#include <stack>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <numeric>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"

#include "CFFlanges.h"
#include "R3ChokeChamberGenerator.h"

namespace setVariable
{

R3ChokeChamberGenerator::R3ChokeChamberGenerator() :
  radius(CF100::innerRadius),wallThick(CF100::wallThick),
  length(22.8),flangeRadius(CF100::flangeRadius),
  flangeLength(CF100::flangeLength),

  inletXStep(0.5),inletWidth(6.2),inletHeight(1.0),inletLength(10.0),
  inletThick(0.5),flangeInletRadius(CF63::flangeRadius),
  flangeInletLength(CF63::flangeLength),

  electronXStep(0.81),electronXYAngle(0.0),
  electronRadius(CF40::innerRadius),electronLength(10.0),
  electronThick(CF40::wallThick),
  flangeElectronRadius(CF40::flangeRadius),
  flangeElectronLength(CF40::flangeLength),

  photonXStep(0),
  photonRadius(1.0),photonLength(17.0),
  photonThick(CF40::wallThick),
  flangePhotonRadius(CF40::flangeRadius),
  flangePhotonLength(CF40::flangeLength),

  sideRadius(CF63::innerRadius),sideLength(11.5),
  sideThick(CF63::wallThick),
  flangeSideRadius(CF63::flangeRadius),
  flangeSideLength(CF63::flangeLength),
  voidMat("Void"),wallMat("Copper"),flangeMat("Stainless304")
  /*!
    Constructor and defaults
  */
{}


R3ChokeChamberGenerator::~R3ChokeChamberGenerator() 
 /*!
   Destructor
 */
{}

void
R3ChokeChamberGenerator::setRadius(const double R)

  /*!
    Set the surface radius
    \param R :: surface radius
   */
{
  radius=R;
  return;
}

void
R3ChokeChamberGenerator::setMaterial(const std::string& WMat,
				     const std::string& FMat)
  /*!
    Set the materials
    \param WMat :: Wall Material
    \param FMat :: Flange Material
  */
{
  wallMat=WMat;
  flangeMat=FMat;
  return;
}

				  
void
R3ChokeChamberGenerator::generateChamber(FuncDataBase& Control,
					 const std::string& keyName) const
   /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables 
    \param keyName :: head name for variable
  */
{
  ELog::RegMethod RegA("R3ChokeChamberGenerator","generateChamber");
  
  Control.addVariable(keyName+"Radius",radius);
  Control.addVariable(keyName+"WallThick",wallThick);
  Control.addVariable(keyName+"Length",length);
  Control.addVariable(keyName+"FlangeRadius",flangeRadius);
  Control.addVariable(keyName+"FlangeLength",flangeLength);

  Control.addVariable(keyName+"InletXStep",inletXStep);
  Control.addVariable(keyName+"InletWidth",inletWidth);
  Control.addVariable(keyName+"InletHeight",inletHeight);
  Control.addVariable(keyName+"InletLength",inletLength);
  Control.addVariable(keyName+"InletThick",inletThick);
  Control.addVariable(keyName+"FlangeInletRadius",flangeInletRadius);
  Control.addVariable(keyName+"FlangeInletLength",flangeInletLength);

  Control.addVariable(keyName+"ElectronXStep",electronXStep);
  Control.addVariable(keyName+"ElectronXYAngle",electronXYAngle);
  Control.addVariable(keyName+"ElectronRadius",electronRadius);
  Control.addVariable(keyName+"ElectronLength",electronLength);
  Control.addVariable(keyName+"ElectronThick",electronThick);
  Control.addVariable(keyName+"FlangeElectronRadius",flangeElectronRadius);
  Control.addVariable(keyName+"FlangeElectronLength",flangeElectronLength);

  Control.addVariable(keyName+"PhotonXStep",photonXStep);
  Control.addVariable(keyName+"PhotonRadius",photonRadius);
  Control.addVariable(keyName+"PhotonLength",photonLength);
  Control.addVariable(keyName+"PhotonThick",photonThick);
  Control.addVariable(keyName+"FlangePhotonRadius",flangePhotonRadius);
  Control.addVariable(keyName+"FlangePhotonLength",flangePhotonLength);
  Control.addVariable(keyName+"SideRadius",sideRadius);
  Control.addVariable(keyName+"SideLength",sideLength);
  Control.addVariable(keyName+"SideThick",sideThick);
  Control.addVariable(keyName+"FlangeSideRadius",flangeSideRadius);
  Control.addVariable(keyName+"FlangeSideLength",flangeSideLength);

  Control.addVariable(keyName+"VoidMat",voidMat);
  Control.addVariable(keyName+"WallMat",wallMat);
  Control.addVariable(keyName+"FlangeMat",flangeMat);


  return;

}

  
}  // NAMESPACE setVariable
