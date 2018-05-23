/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   balder/FlangeMount.cxx
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
#include "stringCombine.h"
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
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "Simulation.h"
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
#include "FrontBackCut.h"
#include "SurfMap.h"

#include "FlangeMount.h" 

namespace xraySystem
{

FlangeMount::FlangeMount(const std::string& Key) : 
  attachSystem::FixedOffset(Key,6),
  attachSystem::ContainedGroup("Flange","Body"),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),attachSystem::FrontBackCut(),
  vacIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(vacIndex+1),inBeam(1),bladeCentreActive(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}



FlangeMount::~FlangeMount() 
  /*!
    Destructor
  */
{}

void
FlangeMount::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("FlangeMount","populate");
  
  FixedOffset::populate(Control);

  // Void + Fe special:
  plateThick=Control.EvalVar<double>(keyName+"PlateThick");
  plateRadius=Control.EvalVar<double>(keyName+"PlateRadius");

  threadRadius=Control.EvalVar<double>(keyName+"ThreadRadius");
  threadLength=Control.EvalVar<double>(keyName+"ThreadLength");
  
  inBeam=Control.EvalDefVar<int>(keyName+"InBeam",inBeam);

  bladeXYAngle=Control.EvalVar<double>(keyName+"BladeXYAngle");
  bladeLift=Control.EvalVar<double>(keyName+"BladeLift");
  bladeThick=Control.EvalVar<double>(keyName+"BladeThick");
  bladeHeight=Control.EvalVar<double>(keyName+"BladeHeight");
  bladeWidth=Control.EvalVar<double>(keyName+"BladeWidth");
  
  threadMat=ModelSupport::EvalMat<int>(Control,keyName+"ThreadMat");
  bladeMat=ModelSupport::EvalMat<int>(Control,keyName+"BladeMat");
  plateMat=ModelSupport::EvalMat<int>(Control,keyName+"PlateMat");


  bladeCentreActive=Control.EvalDefVar<int>
    (keyName+"BladeCentreActive",bladeCentreActive);

  return;
}

void
FlangeMount::createUnitVector(const attachSystem::FixedComp& FC,
                             const long int sideIndex)
  /*!
    Create the unit vectors
    We set the origin external to the front face of the sealing ring.
    and adjust the origin to the middle.
    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("FlangeMount","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
  
  return;
}

void
FlangeMount::calcThreadLength()
  /*!
    Internal function to calc threadLength based
    on the bladeCentre point [if set]
  */
{
  if (bladeCentreActive)
    {
      const Geometry::Vec3D DVec=bladeCentre-Origin;
      threadLength=std::abs(DVec.dotProd(Y));
    }
  return;
}

  
void
FlangeMount::createSurfaces()
  /*!
    Create the surfaces
    If front/back given it is at portLen from the wall and 
    length/2+portLen from origin.
  */
{
  ELog::RegMethod RegA("FlangeMount","createSurfaces");

  // front planes
  if (!frontActive())
    {
      ModelSupport::buildPlane(SMap,vacIndex+1,Origin,Y);
      FrontBackCut::setFront(SMap.realSurf(vacIndex+1));
    }
  ModelSupport::buildPlane(SMap,vacIndex+2,Origin+Y*plateThick,Y);
  ModelSupport::buildCylinder(SMap,vacIndex+7,Origin,Y,plateRadius);
  
  ModelSupport::buildCylinder(SMap,vacIndex+17,Origin,Y,threadRadius);

  const double lift((inBeam) ? 0.0 : bladeLift);
  Geometry::Vec3D PX(X);
  Geometry::Vec3D PY(Z);
  const Geometry::Vec3D PZ(-Y);

  const Geometry::Vec3D BCent(Origin+PZ*(threadLength-lift));

  const Geometry::Quaternion QR
    (Geometry::Quaternion::calcQRotDeg(bladeXYAngle,Y));
  QR.rotate(PX);
  QR.rotate(PY);
  ModelSupport::buildPlane(SMap,vacIndex+101,BCent-PY*(bladeThick/2.0),PY);
  ModelSupport::buildPlane(SMap,vacIndex+102,BCent+PY*(bladeThick/2.0),PY);
  ModelSupport::buildPlane(SMap,vacIndex+103,BCent-PX*(bladeWidth/2.0),PX);
  ModelSupport::buildPlane(SMap,vacIndex+104,BCent+PX*(bladeWidth/2.0),PX);
  ModelSupport::buildPlane(SMap,vacIndex+105,BCent-PZ*(bladeHeight/2.0),PZ);
  ModelSupport::buildPlane(SMap,vacIndex+106,BCent+PZ*(bladeHeight/2.0),PZ);

  return;
}

void
FlangeMount::createObjects(Simulation& System)
  /*!
    Adds the vacuum box
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("FlangeMount","createObjects");

  std::string Out;


  const std::string frontStr=frontRule();  
  const std::string frontComp=frontComplement();
  // Flange
  Out=ModelSupport::getComposite(SMap,vacIndex," -2 -7 ");
  makeCell("Void",System,cellIndex++,plateMat,0.0,Out+frontStr);
  addOuterSurf("Flange",Out+frontStr);
  
  // Thread
  Out=ModelSupport::getComposite(SMap,vacIndex," -17 -105 ");
  makeCell("Thread",System,cellIndex++,threadMat,0.0,Out+frontComp);
  addOuterSurf("Body",Out+frontComp);

  // blade
  Out=ModelSupport::getComposite(SMap,vacIndex," 101 -102 103 -104 105 -106 ");
  makeCell("Blade",System,cellIndex++,bladeMat,0.0,Out);
  addOuterUnionSurf("Body",Out+frontComp);
      
  return;
}
  
void
FlangeMount::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("FlangeMount","createLinks");

  //stufff for intersection


  //  FrontBackCut::createLinks(*this,Origin,Y);  //front and back

  return;
}

void
FlangeMount::setBladeCentre(const Geometry::Vec3D& Pt)
  /*!
    Set the blade centre
    \param Pt :: Centre point 
  */
{
  bladeCentreActive=1;
  bladeCentre=Pt;
  return;
}

void
FlangeMount::setBladeCentre(const attachSystem::FixedComp& FC,
			    const long int BIndex)
  /*!
    Set the blade centre
    \param FC :: FixedComp to use
    \param BIndex :: Link point index
  */
{
  bladeCentreActive=1;
  bladeCentre=FC.getLinkPt(BIndex);
  return;
}
  
void
FlangeMount::createAll(Simulation& System,
		     const attachSystem::FixedComp& FC,
		     const long int FIndex)
 /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("FlangeMount","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(FC,FIndex);
  calcThreadLength();
  createSurfaces();    
  createObjects(System);
  createLinks();
  insertObjects(System);   
  
  return;
}
  
}  // NAMESPACE xraySystem
