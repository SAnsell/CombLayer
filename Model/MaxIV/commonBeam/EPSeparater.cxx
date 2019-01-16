/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeam/EPSeparator.cxx
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
#include "surfDIter.h"
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

#include "EPSeparator.h"

namespace xraySystem
{

EPSeparator::EPSeparator(const std::string& Key) : 
  attachSystem::FixedOffset(Key,6),
  attachSystem::ContainedComp(),
  attachSystem::ExternalCut(),
  attachSystem::CellMap()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}


EPSeparator::~EPSeparator() 
  /*!
    Destructor
  */
{}

void
EPSeparator::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase for variables
  */
{
  ELog::RegMethod RegA("EPSeparator","populate");

  FixedOffset::populate(Control);

  length=Control.EvalVar<double>(keyName+"Length");
  photonOuterGap=Control.EvalVar<double>(keyName+"PhotonOuterGap");
  photonRadius=Control.EvalVar<double>(keyName+"PhotonRadius");
  photonAGap=Control.EvalVar<double>(keyName+"PhotonAGap");
  photonBGap=Control.EvalVar<double>(keyName+"PhotonBGap");
  electronRadius=Control.EvalVar<double>(keyName+"ElectronRadius");
  initEPSeparation=Control.EvalVar<double>(keyName+"InitEPSeparation");
  electronAngle=Control.EvalVar<double>(keyName+"ElectronAngle");

  wallPhoton=Control.EvalVar<double>(keyName+"WallPhoton");
  wallElectron=Control.EvalVar<double>(keyName+"WallElectron");
  wallHeight=Control.EvalVar<double>(keyName+"WallHeight");

  voidMat=ModelSupport::EvalDefMat<int>(Control,keyName+"VoidMat",0);
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");

  return;
}

void
EPSeparator::createUnitVector(const attachSystem::FixedComp& FC,
    	                     const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: FixedComp to attach to
    \param sideIndex :: Link point
  */
{
  ELog::RegMethod RegA("EPSeparator","createUnitVector");
  
  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
  return;
}

void
EPSeparator::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("EPSeparator","createSurface");

  const Geometry::Vec3D eCent(Origin+X*initEPSeparation);
  Geometry::Vec3D eX(X);
  Geometry::Vec3D eY(Y);
  const Geometry::Quaternion QR=
    Geometry::Quaternion::calcQRotDeg(electronAngle,-Z);
  QR.rotate(eX);
  QR.rotate(eY);

  ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*length,Y);
  
  const Geometry::Vec3D photonCent(Origin+X*(photonRadius-photonOuterGap));
  ELog::EM<<"Photo == "<<photonCent<<ELog::endDiag;
  // photon cylinder is offset:
  ModelSupport::buildCylinder(SMap,buildIndex+7,photonCent,Y,photonRadius);
  // dividing plane
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin,X);

  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(photonAGap/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(photonAGap/2.0),Z);

  // electron Cylinder
  ModelSupport::buildCylinder(SMap,buildIndex+1007,eCent,eY,electronRadius);
  // electron divide plane
  ModelSupport::buildPlane(SMap,buildIndex+1004,eCent,eX);


  ModelSupport::buildPlane(SMap,buildIndex+13,Origin-X*wallPhoton,X);
  ModelSupport::buildPlane(SMap,buildIndex+14,Origin+X*wallElectron,eX);
  
  ModelSupport::buildPlane(SMap,buildIndex+15,Origin-Z*(wallHeight/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,Origin+Z*(wallHeight/2.0),Z);

  return;
}

void
EPSeparator::createObjects(Simulation& System)
  /*!
    Builds all the objects
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("EPSeparator","createObjects");
  
  std::string Out;
  // Outer steel
  Out=ModelSupport::getComposite
    (SMap,buildIndex," 1 -2 (-7:3) 5 -6 1007 -1004 ");
  makeCell("photonVoid",System,cellIndex++,voidMat,0.0,Out);
  
  Out=ModelSupport::getComposite(SMap,buildIndex,"1 -2 -1007 ");
  makeCell("electronVoid",System,cellIndex++,voidMat,0.0,Out);
  
  Out=ModelSupport::getComposite
    (SMap,buildIndex," 1 -2 13 -14 15 -16 1007 (1004 : -1007 : 6 : -5 : (7 -3))");
  makeCell("Wall",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -2 13 -14 15 -16 ");
  addOuterSurf(Out);

  return;
}

void 
EPSeparator::createLinks()
  /*!
    Create the linked units
   */
{
  ELog::RegMethod RegA("EPSeparator","createLinks");
  
  return;
}


void
EPSeparator::createAll(Simulation& System,
		      const attachSystem::FixedComp& FC,
		      const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Fixed point track 
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("EPSeparator","createAll");
  
  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);   
  
  return;
}
  
}  // NAMESPACE epbSystem
