/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeam/GratingMono.cxx
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "surfRegister.h"
#include "BaseVisit.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ContainedComp.h"
#include "GratingMono.h"


namespace xraySystem
{

GratingMono::GratingMono(const std::string& Key) :
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(Key,8),
  attachSystem::CellMap(),attachSystem::SurfMap()
  /*!
    Constructor
    \param Key :: Name of construction key
    \param Index :: Index number
  */
{
  nameSideIndex(0,"beamIn");
  nameSideIndex(1,"beamOut");
}


GratingMono::~GratingMono()
  /*!
    Destructor
   */
{}

void
GratingMono::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("GratingMono","populate");

  FixedOffset::populate(Control);

  grateTheta=Control.EvalVar<double>(keyName+"GrateTheta");
  mirrorTheta=Control.EvalVar<double>(keyName+"MirrorTheta");

  mOffset=Control.EvalVar<Geometry::Vec3D>(keyName+"MirrorOffset");
  mRotPt=Control.EvalVar<Geometry::Vec3D>(keyName+"MirrorRotate");
    
  mWidth=Control.EvalVar<double>(keyName+"MirrorWidth");
  mThick=Control.EvalVar<double>(keyName+"MirrorThick");
  mLength=Control.EvalVar<double>(keyName+"MirrorLength");

  gOffset=Control.EvalVar<Geometry::Vec3D>(keyName+"GrateOffset");
  gWidth=Control.EvalVar<double>(keyName+"GrateWidth");
  gThick=Control.EvalVar<double>(keyName+"GrateThick");
  gLength=Control.EvalVar<double>(keyName+"GrateLength");

  mBaseWidth=Control.EvalVar<double>(keyName+"MirrorBaseWidth");
  mBaseThick=Control.EvalVar<double>(keyName+"MirrorBaseThick");
  mBaseLength=Control.EvalVar<double>(keyName+"MirrorBaseLength");

  gBaseWidth=Control.EvalVar<double>(keyName+"GrateBaseWidth");
  gBaseThick=Control.EvalVar<double>(keyName+"GrateBaseThick");
  gBaseLength=Control.EvalVar<double>(keyName+"GrateBaseLength");

  xtalMat=ModelSupport::EvalMat<int>(Control,keyName+"XtalMat");
  baseMat=ModelSupport::EvalMat<int>(Control,keyName+"BaseMat");

  return;
}

void
GratingMono::createUnitVector(const attachSystem::FixedComp& FC,
                               const long int sideIndex)
  /*!
    Create the unit vectors.
    Note that it also set the view point that neutrons come from
    \param FC :: FixedComp for origin
    \param sideIndex :: direction for link
  */
{
  ELog::RegMethod RegA("GratingMono","createUnitVector");
  attachSystem::FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();  
  return;
}

void
GratingMono::createSurfaces()
  /*!
    Create planes for the silicon and Polyethene layers
  */
{
  ELog::RegMethod RegA("GratingMono","createSurfaces");

  // Construct true rotCent:
  MCentre=Origin+X*mOffset.X()+Y*mOffset.Y()+Z*mOffset.Z();
  MRotate=Origin+X*mRotPt.X()+Y*mRotPt.Y()+Z*mRotPt.Z();
  GCentre=Origin+X*gOffset.X()+Y*gOffset.Y()+Z*gOffset.Z();

  // mirror/grating xstal CENTRE AT ORIGIN 
  const Geometry::Quaternion QMX
    (Geometry::Quaternion::calcQRotDeg(mirrorTheta,X));

  const Geometry::Quaternion QGX
    (Geometry::Quaternion::calcQRotDeg(grateTheta,X));


  Geometry::Vec3D GX(X);
  Geometry::Vec3D GY(Y);
  Geometry::Vec3D GZ(Z);

  Geometry::Vec3D MX(X);
  Geometry::Vec3D MY(Y);
  Geometry::Vec3D MZ(Z);


  QGX.rotate(GY);
  QGX.rotate(GZ);

  QMX.rotate(MY);
  QMX.rotate(MZ);

  // Now rotate centre of mirror about mRotate
  MCentre-=MRotate;
  QMX.rotate(MCentre);
  MCentre+=MRotate;
  
  ModelSupport::buildPlane(SMap,buildIndex+101,MCentre-MY*(mLength/2.0),MY);
  ModelSupport::buildPlane(SMap,buildIndex+102,MCentre+MY*(mLength/2.0),MY);
  ModelSupport::buildPlane(SMap,buildIndex+103,MCentre-MX*(mWidth/2.0),MX);
  ModelSupport::buildPlane(SMap,buildIndex+104,MCentre+MX*(mWidth/2.0),MX);
  ModelSupport::buildPlane(SMap,buildIndex+105,MCentre-MZ*mThick,MZ);
  ModelSupport::buildPlane(SMap,buildIndex+106,MCentre,MZ);
  
  ModelSupport::buildPlane(SMap,buildIndex+201,GCentre-GY*(gLength/2.0),GY);
  ModelSupport::buildPlane(SMap,buildIndex+202,GCentre+GY*(gLength/2.0),GY);
  ModelSupport::buildPlane(SMap,buildIndex+203,GCentre-GX*(gWidth/2.0),GX);
  ModelSupport::buildPlane(SMap,buildIndex+204,GCentre+GX*(gWidth/2.0),GX);
  ModelSupport::buildPlane(SMap,buildIndex+205,GCentre,GZ);
  ModelSupport::buildPlane(SMap,buildIndex+206,GCentre+GZ*gThick,GZ);

  return; 
}

void
GratingMono::createObjects(Simulation& System)
  /*!
    Create the vaned moderator
    \param System :: Simulation to add results
   */
{
  ELog::RegMethod RegA("GratingMono","createObjects");

  std::string Out;
  // xstal A
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 " 101 -102 103 -104 105 -106 ");
  makeCell("XtalA",System,cellIndex++,xtalMat,0.0,Out);
  addOuterSurf(Out);
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 " 201 -202 203 -204 205 -206 ");
  makeCell("XtalB",System,cellIndex++,xtalMat,0.0,Out);
  addOuterUnionSurf(Out);
  
  return; 
}

void
GratingMono::createLinks()
  /*!
    Creates a full attachment set
  */
{
  ELog::RegMethod RegA("GratingMono","createLinks");


  // top surface going back down beamline to ring
  FixedComp::setConnect(0,MCentre,-Y);
  FixedComp::setLinkSurf(0,SMap.realSurf(buildIndex+106));

  // top surface going to experimental area
  FixedComp::setConnect(1,GCentre,Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+205));

  return;
}

void
GratingMono::createAll(Simulation& System,
			const attachSystem::FixedComp& FC,
			const long int sideIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComp to construct from
    \param sideIndex :: Side point
   */
{
  ELog::RegMethod RegA("GratingMono","createAll");
  populate(System.getDataBase());

  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       

  return;
}

}  // NAMESPACE xraySystem
