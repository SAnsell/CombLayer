/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   moderator/Reflector.cxx
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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
#include <memory>
#include <array>

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
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "inputParam.h"
#include "HeadRule.h"
#include "Object.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ReadFunctions.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "Bucket.h"
#include "CoolPad.h"
#include "RefCutOut.h"
#include "RefBolts.h"
#include "Reflector.h"

#include "insertObject.h"
#include "insertPlate.h"

namespace moderatorSystem
{

Reflector::Reflector(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(Key,12),
  attachSystem::SurfMap(),
  attachSystem::CellMap()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{
  FixedComp::nameSideIndex(10,"CornerCentre");
}

Reflector::Reflector(const Reflector& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedOffset(A),
  attachSystem::SurfMap(A),
  attachSystem::CellMap(A),
  xySize(A.xySize),zSize(A.zSize),cutSize(A.cutSize),
  defMat(A.defMat),
  Pads(A.Pads)
  /*!
    Copy constructor
    \param A :: Reflector to copy
  */
{}

Reflector&
Reflector::operator=(const Reflector& A)
  /*!
    Assignment operator
    \param A :: Reflector to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedComp::operator=(A);
      xySize=A.xySize;
      zSize=A.zSize;
      cutSize=A.cutSize;
      defMat=A.defMat;

      Pads=A.Pads;
    }
  return *this;
}

Reflector::~Reflector() 
  /*!
    Destructor
  */
{}

void
Reflector::populate(const FuncDataBase& Control)
/*!
  Populate all the variables
  \param Control :: Data base for variables
*/
{
  ELog::RegMethod RegA("Reflector","populate");
  
  FixedOffset::populate(Control);
  
  xySize=Control.EvalVar<double>(keyName+"XYSize");
  zSize=Control.EvalVar<double>(keyName+"ZSize");
  cutSize=Control.EvalVar<double>(keyName+"CutSize");
  cornerAngle=Control.EvalVar<double>(keyName+"CornerAngle");
  
  defMat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");

  const size_t nPads=Control.EvalVar<size_t>(keyName+"NPads");
  for(size_t i=0;i<nPads;i++)
    Pads.push_back(CoolPad("coolPad",i+1));

  return;
}
    
void
Reflector::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("Reflector","createSurface");

  // rotation of axis:
  const Geometry::Quaternion Qxy=
    Geometry::Quaternion::calcQRotDeg(cornerAngle,Z);
  Geometry::Vec3D XR(X);
  Geometry::Vec3D YR(Y);
  Qxy.rotate(XR);
  Qxy.rotate(YR);
  
  // Simple box planes

  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*xySize,Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*xySize,Y);
  SurfMap::addSurf("Front",SMap.realSurf(buildIndex+1));
  SurfMap::addSurf("Back",-SMap.realSurf(buildIndex+2));
  
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*xySize,X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*xySize,X);
  SurfMap::addSurf("Left",SMap.realSurf(buildIndex+3));
  SurfMap::addSurf("Right",-SMap.realSurf(buildIndex+4));

  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*zSize,Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*zSize,Z);
  SurfMap::addSurf("Base",SMap.realSurf(buildIndex+5));
  SurfMap::addSurf("Top",-SMap.realSurf(buildIndex+6));
 
  // Corner cuts:
  ModelSupport::buildPlane(SMap,buildIndex+11,Origin-YR*cutSize,YR);
  ModelSupport::buildPlane(SMap,buildIndex+12,Origin+YR*cutSize,YR);
  SurfMap::addSurf("CornerA",SMap.realSurf(buildIndex+11));
  SurfMap::addSurf("CornerB",-SMap.realSurf(buildIndex+12));
    
  ModelSupport::buildPlane(SMap,buildIndex+13,Origin-XR*cutSize,XR);
  ModelSupport::buildPlane(SMap,buildIndex+14,Origin+XR*cutSize,XR);
  SurfMap::addSurf("CornerC",SMap.realSurf(buildIndex+13));
  SurfMap::addSurf("CornerD",-SMap.realSurf(buildIndex+14));

  createLinks(XR,YR);

  return;
}

void
Reflector::createLinks(const Geometry::Vec3D& XR,
		       const Geometry::Vec3D& YR)
  /*!
    Build the links to the primary surfaces
    \param XR :: Size rotation direction
    \param YR :: Size rotation direction
  */
{
  ELog::RegMethod RegA("Reflector","createLinks");

  FixedComp::setConnect(0,Origin-Y*xySize,-Y);  // chipIR OPPOSITE
  FixedComp::setConnect(1,Origin+Y*xySize,Y);   // chipIR
  FixedComp::setConnect(2,Origin-X*xySize,-X);
  FixedComp::setConnect(3,Origin+X*xySize,X);
  FixedComp::setConnect(4,Origin-Z*zSize,-Z);
  FixedComp::setConnect(5,Origin+Z*zSize,Z);

  FixedComp::setConnect(6,Origin-YR*cutSize,-YR);
  FixedComp::setConnect(7,Origin+YR*cutSize,YR);
  FixedComp::setConnect(8,Origin-XR*cutSize,-XR);

  FixedComp::setConnect(10,Origin,YR);   // corner centre

  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+1));
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+2));
  FixedComp::setLinkSurf(2,-SMap.realSurf(buildIndex+3));
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+4));
  FixedComp::setLinkSurf(4,-SMap.realSurf(buildIndex+5));
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+6));
  FixedComp::setLinkSurf(6,-SMap.realSurf(buildIndex+11));
  FixedComp::setLinkSurf(7,SMap.realSurf(buildIndex+12));
  FixedComp::setLinkSurf(8,-SMap.realSurf(buildIndex+13));
  FixedComp::setLinkSurf(9,SMap.realSurf(buildIndex+14));


  return;
}

void
Reflector::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("Reflector","createObjects");

  std::string Out;
  Out=ModelSupport::getComposite(SMap,buildIndex,"1 -2 3 -4 5 -6 11 -12 13 -14");
  addOuterSurf(Out);

  makeCell("Reflector",System,cellIndex++,defMat,0.0,Out);

  for(CoolPad& PD : Pads)
    PD.addInsertCell(74123);
 
  for(CoolPad& PD : Pads)
    PD.createAll(System,*this,3);
      
  return;
}

/*
std::string
Reflector::getExclude() const 
  / *!
    Virtual function to add the cooling pads
    \return Full Exlcude path
  * /
{
  ELog::RegMethod RegA("Reflector","getExclude");

  std::string Out=ContainedComp::getExclude();
  for(const CoolPad& PD : Pads)
    Out+=PD.getExclude();
  return Out;
}
*/

void
Reflector::createAll(Simulation& System,
		     const attachSystem::FixedComp& FC,
		     const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
  */
{
  ELog::RegMethod RegA("Reflector","createAll");

  populate(System.getDataBase());

  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
	
  insertObjects(System);              

  return;
}

}  // NAMESPACE shutterSystem
