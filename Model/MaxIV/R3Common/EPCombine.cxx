/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   R3Comoon/EPCombine.cxx
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
#include <set>
#include <map>
#include <string>
#include <algorithm>
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
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfEqual.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "Rules.h"
#include "SurInter.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "SimProcess.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"  
#include "FixedComp.h"
#include "FixedOffset.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "ExternalCut.h" 
#include "BaseMap.h"
#include "SurfMap.h"
#include "CellMap.h" 

#include "EPCombine.h"

namespace xraySystem
{

EPCombine::EPCombine(const std::string& Key) : 
  attachSystem::FixedOffset(Key,6),
  attachSystem::ContainedComp(),
  attachSystem::ExternalCut(),
  attachSystem::CellMap()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}


EPCombine::~EPCombine() 
  /*!
    Destructor
  */
{}

void
EPCombine::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase for variables
  */
{
  ELog::RegMethod RegA("EPCombine","populate");

  FixedOffset::populate(Control);

  length=Control.EvalVar<double>(keyName+"Length");

  photonXStep=Control.EvalVar<double>(keyName+"PhotonXStep");
  electronXStep=Control.EvalVar<double>(keyName+"ElectronXStep");
  photonXYAngle=Control.EvalVar<double>(keyName+"PhotonXYAngle");
  electronXYAngle=Control.EvalVar<double>(keyName+"ElectronXYAngle");

  electronRadius=Control.EvalVar<double>(keyName+"ElectronRadius");
  photonRadius=Control.EvalVar<double>(keyName+"PhotonRadius");

  skinThick=Control.EvalVar<double>(keyName+"SkinThick");

  wallStartLen=Control.EvalVar<double>(keyName+"WallStartLen") ;
  wallWidth=Control.EvalVar<double>(keyName+"WallWidth");
  wallHeight=Control.EvalVar<double>(keyName+"WallHeight");

  flangeAXStep=Control.EvalVar<double>(keyName+"FlangeAXStep");
  flangeARadius=Control.EvalVar<double>(keyName+"FlangeARadius");
  flangeALength=Control.EvalVar<double>(keyName+"FlangeALength");

  flangeBXStep=Control.EvalVar<double>(keyName+"FlangeBXStep");
  flangeBRadius=Control.EvalVar<double>(keyName+"FlangeBRadius");
  flangeBLength=Control.EvalVar<double>(keyName+"FlangeBLength");

  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  flangeMat=ModelSupport::EvalMat<int>(Control,keyName+"FlangeMat");
  
  return;
}

void
EPCombine::createUnitVector(const attachSystem::FixedComp& FC,
			      const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: FixedComp to attach to
    \param sideIndex :: Link point
  */
{
  ELog::RegMethod RegA("EPCombine","createUnitVector");
  
  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
  return;
}

void
EPCombine::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("EPCombine","createSurface");

  // Do outer surfaces (vacuum ports)
  if (!isActive("front"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
      setCutSurf("front",SMap.realSurf(buildIndex+1));
    }
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*length,Y);
  
  const Geometry::Quaternion photonQ=
    Geometry::Quaternion::calcQRotDeg(photonXYAngle,Z);
  const Geometry::Vec3D PX=photonQ.makeRotate(X);
  const Geometry::Vec3D PY=photonQ.makeRotate(Y);
  const Geometry::Vec3D POrigin(Origin+X*photonXStep);

  const Geometry::Quaternion electronQ=
    Geometry::Quaternion::calcQRotDeg(electronXYAngle,Z);
  const Geometry::Vec3D EX=electronQ.makeRotate(X);
  const Geometry::Vec3D EY=electronQ.makeRotate(Y);
  const Geometry::Vec3D EOrigin(Origin+X*electronXStep);


  ModelSupport::buildPlane(SMap,buildIndex+102,Origin+Y*(wallStartLen),Y);

  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(wallWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(wallWidth/2.0),X);

  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(wallHeight/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(wallHeight/2.0),Z);
    

  // flange cylinder/plangs
  ModelSupport::buildCylinder
    (SMap,buildIndex+7,Origin+X*flangeAStep,Y,flangeARadius);
  ModelSupport::buildCylinder
    (SMap,buildIndex+17,Origin+X*flangeBStep,Y,flangeBRadius);

  
  ModelSupport::buildPlane
    (SMap,buildIndex+11,Origin+Y*flangeALength,Y);
  ModelSupport::buildPlane
    (SMap,buildIndex+12,Origin+Y*(length-flangeBLength),Y);
  
  // electron inner :
  // divider
  ModelSupport::buildPlane(SMap,buildIndex+103,EOrigin,EX);
  ModelSupport::buildCylinder(SMap,buildIndex+107,EOrigin,EY,electronRadius);
  
  // photon inner :
  ModelSupport::buildCylinder(SMap,buildIndex+207,POrigin,PY,photonRadius);
  ModelSupport::buildPlane(SMap,buildIndex+203,POrigin,PX);
  ModelSupport::buildPlane(SMap,buildIndex+205,POrigin-Z*photonRadius,Z);
  ModelSupport::buildPlane(SMap,buildIndex+206,POrigin+Z*photonRadius,Z);
  
  // Electron walls: [All point to center]
  const Geometry::Vec3D XZ((EX+Z).unit());
  const Geometry::Vec3D mXZ((Z-EX).unit());
  ModelSupport::buildPlane
    (SMap,buildIndex+211,EOrigin-Z*(electronRadius+skinThick),Z);
  ModelSupport::buildPlane
    (SMap,buildIndex+212,EOrigin-XZ*(electronRadius+skinThick),XZ);
  ModelSupport::buildPlane
    (SMap,buildIndex+213,EOrigin+mXZ*(electronRadius+skinThick),-mXZ);
  ModelSupport::buildPlane
    (SMap,buildIndex+214,EOrigin+Z*(electronRadius+skinThick),-Z);
  ModelSupport::buildPlane
    (SMap,buildIndex+215,EOrigin+XZ*(electronRadius+skinThick),-XZ);
  ModelSupport::buildPlane
    (SMap,buildIndex+216,EOrigin-mXZ*(electronRadius+skinThick),mXZ);

  
  // Photon Wall

  ModelSupport::buildCylinder
    (SMap,buildIndex+217,POrigin,PY,photonRadius+skinThick);
  ModelSupport::buildPlane
    (SMap,buildIndex+215,POrigin-Z*(photonRadius+skinThick),Z);
  ModelSupport::buildPlane
    (SMap,buildIndex+216,POrigin+Z*(photonRadius+skinThick),Z);

  return;
}

void
EPCombine::createObjects(Simulation& System)
  /*!
    Builds all the objects
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("EPCombine","createObjects");

  const std::string frontSurf(ExternalCut::getRuleStr("front"));
  
  std::string Out;

  Out=ModelSupport::getComposite
    (SMap,buildIndex," -2 (-107 : -207 : (-103 205 -206 203) )");
  makeCell("void",System,cellIndex++,voidMat,0.0,Out+frontSurf);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," -2 3 -4 5 -6 (107  207 (-205 : 206 : 103 : -203)");
  makeCell("Outer",System,cellIndex++,wallMat,0.0,Out+frontSurf);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," -7 -11  (-3:4:-5:6) ");
  makeCell("FlangeA",System,cellIndex++,flangeMat,0.0,Out+frontSurf);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," -7 (-3:4:-5:6) 12 -2 ");
  makeCell("FlangeB",System,cellIndex++,flangeMat,0.0,Out);
  
  Out=ModelSupport::getComposite
    (SMap,buildIndex," -7 (-3:4:-5:6) 11 -12 ");
  makeCell("OuterVoid",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," -2 -7 ");
  addOuterSurf(Out+frontSurf);

  return;
}

void 
EPCombine::createLinks()
  /*!
    Create the linked units
   */
{
  ELog::RegMethod RegA("EPCombine","createLinks");

  
  return;
}

void
EPCombine::createAll(Simulation& System,
		      const attachSystem::FixedComp& FC,
		      const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Fixed point track 
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("EPCombine","createAll");
  
  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);   
  
  return;
}
  
}  // NAMESPACE xraySystem
