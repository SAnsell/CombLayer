/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   R3Comoon/PreBendPipe.cxx
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

#include "PreBendPipe.h"

namespace xraySystem
{

PreBendPipe::PreBendPipe(const std::string& Key) : 
  attachSystem::FixedOffset(Key,6),
  attachSystem::ContainedComp(),
  attachSystem::ExternalCut(),
  attachSystem::CellMap()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}


PreBendPipe::~PreBendPipe() 
  /*!
    Destructor
  */
{}

void
PreBendPipe::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase for variables
  */
{
  ELog::RegMethod RegA("PreBendPipe","populate");

  FixedOffset::populate(Control);

  length=Control.EvalVar<double>(keyName+"Length");
  radius=Control.EvalVar<double>(keyName+"Radius");
  straightLength=Control.EvalVar<double>(keyName+"StraightLength");

  wallThick=Control.EvalVar<double>(keyName+"WallThick");
  centreXStep=Control.EvalVar<double>(keyName+"CentreXStep");

  flangeARadius=Control.EvalVar<double>(keyName+"FlangeARadius");
  flangeALength=Control.EvalVar<double>(keyName+"FlangeALength");
  flangeBRadius=Control.EvalVar<double>(keyName+"FlangeBRadius");
  flangeBLength=Control.EvalVar<double>(keyName+"FlangeBLength");

  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  flangeMat=ModelSupport::EvalMat<int>(Control,keyName+"FlangeMat");
  
  return;
}

void
PreBendPipe::createUnitVector(const attachSystem::FixedComp& FC,
			      const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: FixedComp to attach to
    \param sideIndex :: Link point
  */
{
  ELog::RegMethod RegA("PreBendPipe","createUnitVector");
  
  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
  return;
}

void
PreBendPipe::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("PreBendPipe","createSurface");

  // Do outer surfaces (vacuum ports)
  if (!isActive("front"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
      setCutSurf("front",SMap.realSurf(buildIndex+1));
    }
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*length,Y);

  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,radius);
  ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Y,radius+wallThick);

  ModelSupport::buildPlane(SMap,buildIndex+101,Origin+Y*(straightLength),Y);

  const Geometry::Vec3D coneCent=Origin+Y*(straightLength);
  const Geometry::Vec3D PtA=Origin+Y*Length - X*radius;
  const Geometry::Vec3D PtB=Origin+Y*Length + X*(radius+centreXStep);
  ModelSupport::buildCone(SMap,buildIndex+107,
			  Origin+Y*(straightLength),Y);  

  
  // flange cylinder/planes
  ModelSupport::buildCylinder(SMap,buildIndex+1007,Origin,Y,flangeARadius);
  ModelSupport::buildPlane(SMap,buildIndex+1001,Origin+Y*flangeALength),Y);

  ModelSupport::buildCylinder(SMap,buildIndex+2007,Origin,Y,flangeBRadius);
  ModelSupport::buildPlane
  (SMap,buildIndex+2001,Origin+Y*(length-flangeBLength),Y);


  return;
}

void
PreBendPipe::createObjects(Simulation& System)
  /*!
    Builds all the objects
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("PreBendPipe","createObjects");

  const std::string frontSurf(ExternalCut::getRuleStr("front"));
  
  std::string Out;

  Out=ModelSupport::getComposite
    (SMap,buildIndex," -2 (-107 : -207 : (-103 205 -206 203) )");
  makeCell("void",System,cellIndex++,voidMat,0.0,Out+frontSurf);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," 1001 -2 3 -4 5 -6 107  207 (-205 : 206 : 103 : -203)");
  makeCell("Outer",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," -1001  111 112 113 114 115 116 107 (-205:206:103)");
  makeCell("ElectronWall",System,cellIndex++,wallMat,0.0,Out+frontSurf);

  Out=ModelSupport::getComposite
    (SMap,buildIndex,"11 -1001 -4 5 -6 112 116 (-111:-113:-115:-114) ");
  makeCell("ElectronOuter",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," -1001  (-112:-116) 207 (-205:206:-203) "
                  " 215 -216 (-217 : 203)");
  makeCell("PhotonWall",System,cellIndex++,wallMat,0.0,Out+frontSurf);


  Out=ModelSupport::getComposite
    (SMap,buildIndex,"11 -1001 3  5 -6 (-112:-116) (-215:216) ");
  makeCell("PhotonOuter",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex,"11 -1001 3 215 -216 217 -203 ");
  makeCell("PhotonOuter",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," -7 -11  (-3:4:-5:6) ");
  makeCell("FlangeA",System,cellIndex++,flangeMat,0.0,Out+frontSurf);

  Out=ModelSupport::getComposite
    (SMap,buildIndex,"-11 -4 5 -6 112 116 (-111:-113:-115:-114) ");
  makeCell("ElectronOutFA",System,cellIndex++,flangeMat,0.0,Out+frontSurf);

  Out=ModelSupport::getComposite
    (SMap,buildIndex,"-11 3 215 -216 217 -203 ");
  makeCell("PhotonOutFA",System,cellIndex++,flangeMat,0.0,Out+frontSurf);

  Out=ModelSupport::getComposite
    (SMap,buildIndex,"-11 3  5 -6 (-112:-116) (-215:216) ");
  makeCell("PhotonOutFA",System,cellIndex++,flangeMat,0.0,Out+frontSurf);

  
  Out=ModelSupport::getComposite
    (SMap,buildIndex," -17 7 -11 ");
  makeCell("FlangeAOuter",System,cellIndex++,0,0.0,Out+frontSurf);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," -17 (-3:4:-5:6) 12 -2 ");
  makeCell("FlangeB",System,cellIndex++,flangeMat,0.0,Out);
  
  Out=ModelSupport::getComposite
    (SMap,buildIndex," -17 (-3:4:-5:6) 11 -12 ");
  makeCell("OuterVoid",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," -2 -17 ");
  addOuterSurf(Out+frontSurf);

  return;
}

void 
PreBendPipe::createLinks()
  /*!
    Create the linked units
   */
{
  ELog::RegMethod RegA("PreBendPipe","createLinks");

  
  return;
}

void
PreBendPipe::createAll(Simulation& System,
		      const attachSystem::FixedComp& FC,
		      const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Fixed point track 
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("PreBendPipe","createAll");
  
  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);   
  
  return;
}
  
}  // NAMESPACE xraySystem
