/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/commonBeam/BladeBPMToyama.cxx
 *
 * Copyright (c) 2004-2026 by Konstantin Batkov
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
#include "Exception.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "Vec3D.h"
#include "surfRegister.h"
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
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"

#include "BladeBPMToyama.h"

namespace xraySystem
{

BladeBPMToyama::BladeBPMToyama(const std::string& Key)  :
  attachSystem::ContainedGroup("Base", "Feedthrough"),
  attachSystem::FixedRotate(Key,6),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  attachSystem::FrontBackCut()
 /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

BladeBPMToyama::BladeBPMToyama(const BladeBPMToyama& A) :
  attachSystem::ContainedGroup(A),
  attachSystem::FixedRotate(A),
  attachSystem::CellMap(A),
  attachSystem::SurfMap(A),
  attachSystem::FrontBackCut(A),
  length(A.length),
  chamberLength(A.chamberLength),
  chamberRadius(A.chamberRadius),
  chamberWallThick(A.chamberWallThick),
  chamberFlangeRadius(A.chamberFlangeRadius),
  chamberFlangeLength(A.chamberFlangeLength),
  insertFlangeRadius(A.insertFlangeRadius),
  insertFlangeLength(A.insertFlangeLength),
  insertInnerRadius(A.insertInnerRadius),
  insertOuterRadius(A.insertOuterRadius),
  insertPreOuterRadius(A.insertPreOuterRadius),
  insertLength(A.insertLength),
  portLength(A.portLength),
  portWallThick(A.portWallThick),
  portRadius(A.portRadius),
  portCapLength(A.portCapLength),
  portCapCentralLength(A.portCapCentralLength),
  portCapCentralRadius(A.portCapCentralRadius),
  portFlangeLength(A.portFlangeLength),
  portFlangeRadius(A.portFlangeRadius),
  chamberFlangeMat(A.chamberFlangeMat),chamberWallMat(A.chamberWallMat),
  insertMat(A.insertMat),
  insertPreMat(A.insertPreMat),
  insertFlangeMat(A.insertFlangeMat),
  voidMat(A.voidMat),
  airMat(A.airMat)
  /*!
    Copy constructor
    \param A :: BladeBPMToyama to copy
  */
{}

BladeBPMToyama&
BladeBPMToyama::operator=(const BladeBPMToyama& A)
  /*!
    Assignment operator
    \param A :: BladeBPMToyama to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedGroup::operator=(A);
      attachSystem::FixedRotate::operator=(A);
      attachSystem::CellMap::operator=(A);
      attachSystem::SurfMap::operator=(A);
      attachSystem::FrontBackCut::operator=(A);
      length=A.length;
      chamberLength=A.chamberLength;
      chamberRadius=A.chamberRadius;
      chamberWallThick=A.chamberWallThick;
      chamberFlangeRadius=A.chamberFlangeRadius;
      chamberFlangeLength=A.chamberFlangeLength;
      insertFlangeRadius=A.insertFlangeRadius;
      insertFlangeLength=A.insertFlangeLength;
      insertInnerRadius=A.insertInnerRadius;
      insertOuterRadius=A.insertOuterRadius;
      insertPreOuterRadius=A.insertPreOuterRadius;
      insertLength=A.insertLength;
      portLength=A.portLength;
      portWallThick=A.portWallThick;
      portRadius=A.portRadius;
      portFlangeLength=A.portFlangeLength;
      portCapLength=A.portCapLength;
      portCapCentralLength=A.portCapCentralLength;
      portCapCentralRadius=A.portCapCentralRadius;
      portFlangeRadius=A.portFlangeRadius;
      chamberFlangeMat=A.chamberFlangeMat;
      chamberWallMat=A.chamberWallMat;
      insertMat=A.insertMat;
      insertPreMat=A.insertPreMat;
      insertFlangeMat=A.insertFlangeMat;
      voidMat=A.voidMat;
      airMat=A.airMat;
    }
  return *this;
}

BladeBPMToyama*
BladeBPMToyama::clone() const
/*!
  Clone self
  \return new (this)
 */
{
    return new BladeBPMToyama(*this);
}

BladeBPMToyama::~BladeBPMToyama()
  /*!
    Destructor
  */
{}

void
BladeBPMToyama::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("BladeBPMToyama","populate");

  FixedRotate::populate(Control);

  length=Control.EvalVar<double>(keyName+"Length");
  chamberLength=Control.EvalVar<double>(keyName+"ChamberLength");
  chamberRadius=Control.EvalVar<double>(keyName+"ChamberRadius");
  chamberWallThick=Control.EvalVar<double>(keyName+"ChamberWallThick");
  chamberFlangeRadius=Control.EvalVar<double>(keyName+"ChamberFlangeRadius");
  chamberFlangeLength=Control.EvalVar<double>(keyName+"ChamberFlangeLength");

  insertFlangeRadius=Control.EvalVar<double>(keyName+"InsertFlangeRadius");
  if (insertFlangeRadius + Geometry::zeroTol > chamberFlangeRadius)
    throw ColErr::RangeError<double>(insertFlangeRadius,0.0,chamberFlangeRadius,"insertFlangeRadius");

  insertFlangeLength=Control.EvalVar<double>(keyName+"InsertFlangeLength");
  insertInnerRadius=Control.EvalVar<double>(keyName+"InsertInnerRadius");
  insertOuterRadius=Control.EvalVar<double>(keyName+"InsertOuterRadius");
  insertPreOuterRadius=Control.EvalVar<double>(keyName+"InsertPreOuterRadius");
  insertLength=Control.EvalVar<double>(keyName+"InsertLength");
  portLength=Control.EvalVar<double>(keyName+"PortLength");
  portWallThick=Control.EvalVar<double>(keyName+"PortWallThick");
  portRadius=Control.EvalVar<double>(keyName+"PortRadius");
  portFlangeLength=Control.EvalVar<double>(keyName+"PortFlangeLength");
  portCapLength=Control.EvalVar<double>(keyName+"PortCapLength");
  portCapCentralLength=Control.EvalVar<double>(keyName+"PortCapCentralLength");
  portCapCentralRadius=Control.EvalVar<double>(keyName+"PortCapCentralRadius");
  portFlangeRadius=Control.EvalVar<double>(keyName+"PortFlangeRadius");

  chamberFlangeMat=ModelSupport::EvalMat<int>(Control,keyName+"ChamberFlangeMat");
  chamberWallMat=ModelSupport::EvalMat<int>(Control,keyName+"ChamberWallMat");
  insertMat=ModelSupport::EvalMat<int>(Control,keyName+"InsertMat");
  insertPreMat=ModelSupport::EvalMat<int>(Control,keyName+"InsertPreMat");
  insertFlangeMat=ModelSupport::EvalMat<int>(Control,keyName+"InsertFlangeMat");
  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");
  airMat=ModelSupport::EvalMat<int>(Control,keyName+"AirMat");

  return;
}

void
BladeBPMToyama::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("BladeBPMToyama","createSurfaces");

  if (!frontActive())
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
      FrontBackCut::setFront(SMap.realSurf(buildIndex+1));

      ModelSupport::buildPlane(SMap,buildIndex+11,Origin+Y*(chamberFlangeLength),Y);
    } else
    {
      ModelSupport::buildShiftedPlane(SMap, buildIndex+11,
	      SMap.realPtr<Geometry::Plane>(getFrontRule().getPrimarySurface()),
				      chamberFlangeLength);
    }

  if (!backActive())
    {
      ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length),Y);
      FrontBackCut::setBack(-SMap.realSurf(buildIndex+2));

      ModelSupport::buildPlane(SMap,buildIndex+12,Origin+Y*(chamberLength-chamberFlangeLength),Y);
    } else
    {
      ModelSupport::buildShiftedPlane(SMap, buildIndex+12,
	      SMap.realPtr<Geometry::Plane>(getBackRule().getPrimarySurface()),
				      -chamberFlangeLength);
    }

  ModelSupport::buildShiftedPlane(SMap, buildIndex+21, buildIndex+12, Y, chamberFlangeLength);

  ModelSupport::buildShiftedPlane(SMap, buildIndex+22,
	      SMap.realPtr<Geometry::Plane>(getBackRule().getPrimarySurface()),
				      -insertFlangeLength);
  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,chamberRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Y,chamberRadius+chamberWallThick);
  ModelSupport::buildCylinder(SMap,buildIndex+27,Origin,Y,chamberFlangeRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+37,Origin,Y,insertFlangeRadius);

  ModelSupport::buildCylinder(SMap,buildIndex+107,Origin,Y,insertInnerRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+117,Origin,Y,insertPreOuterRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+127,Origin,Y,insertOuterRadius);

  ModelSupport::buildShiftedPlane(SMap, buildIndex+101, buildIndex+2, Y, -insertLength);
  ModelSupport::buildShiftedPlane(SMap, buildIndex+111, buildIndex+21, Y,  chamberFlangeLength);

  // Feedthrough port

  ModelSupport::buildPlane(SMap,buildIndex+205,Origin+Z*(portLength-portFlangeLength),Z);
  ModelSupport::buildPlane(SMap,buildIndex+206,Origin+Z*(portLength),Z);
  ModelSupport::buildPlane(SMap,buildIndex+215,Origin+Z*(portLength+portCapCentralLength),Z);
  ModelSupport::buildPlane(SMap,buildIndex+216,Origin+Z*(portLength+portCapLength),Z);
  ModelSupport::buildCylinder(SMap,buildIndex+207,Origin+Y*(chamberLength/2.0),Z,portRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+217,Origin+Y*(chamberLength/2.0),Z,portRadius+portWallThick);
  ModelSupport::buildCylinder(SMap,buildIndex+227,Origin+Y*(chamberLength/2.0),Z,portFlangeRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+237,Origin+Y*(chamberLength/2.0),Z,portCapCentralRadius);



  return;
}

void
BladeBPMToyama::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("BladeBPMToyama","createObjects");

  HeadRule HR;
  const HeadRule front(frontRule());
  const HeadRule back(backRule());

  HR=ModelSupport::getHeadRule(SMap,buildIndex," -7 -101 ");
  makeCell("ChamberVoid",System,cellIndex++,voidMat,0.0,HR*front);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 117 -7 101 -21");
  makeCell("ChamberVoidInsert",System,cellIndex++,voidMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," -107 101 ");
  makeCell("InsertVoid",System,cellIndex++,voidMat,0.0,HR*back);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 107 -117 101 -21 ");
  makeCell("InsertPre",System,cellIndex++,insertPreMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 107 -127 21 ");
  makeCell("Insert",System,cellIndex++,insertMat,0.0,HR*back);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,0," 11 -12 7 -17 (217:-60000M)");
  makeCell("ChamberWall",System,cellIndex++,chamberWallMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," -11 7 -27 ");
  makeCell("FlangeA",System,cellIndex++,chamberFlangeMat,0.0,HR*front);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,0," 11 -12 17 -27 (217:-60000M) ");
  makeCell("ChamberOuterVoid",System,cellIndex++,airMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 12 -21 7 -27 ");
  makeCell("FlangeB",System,cellIndex++,chamberFlangeMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 111 -22 127 -27 ");
  makeCell("InsertOuterVoid",System,cellIndex++,airMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 22 37 -27 ");
  makeCell("InsertOuterVoid",System,cellIndex++,airMat,0.0,HR*back);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 21 -111 127 -27 ");
  makeCell("InsertFrontFlange",System,cellIndex++,insertFlangeMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 22 127 -37 ");
  makeCell("InsertBackFlange",System,cellIndex++,insertFlangeMat,0.0,HR*back);


  HR=ModelSupport::getHeadRule(SMap,buildIndex,0," 7 -207 60000M -206 ");
  makeCell("FeedthroughPortVoid",System,cellIndex++,voidMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,0," 7 207 -217 60000M -206 ");
  makeCell("FeedthroughPortWall",System,cellIndex++,chamberWallMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 217 -227 205 -206");
  makeCell("FeedthroughPortFlange",System,cellIndex++,chamberFlangeMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,0," 217 -227 -205 27 60000M ");
  makeCell("FeedthroughPortFlangeAirBelow",System,cellIndex++,airMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 206 -215 -227 ");
  makeCell("FeedthroughPortCapLower",System,cellIndex++,chamberFlangeMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 215 -216 237 -227 ");
  makeCell("FeedthroughPortCapUpper",System,cellIndex++,chamberFlangeMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 215 -216 -237 ");
  makeCell("FeedthroughPortCapUpper",System,cellIndex++,airMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,0," -227 -216 27 60000M ");
  addOuterSurf("Feedthrough", HR);


  HR=ModelSupport::getHeadRule(SMap,buildIndex," -27 ");
  addOuterSurf("Base", HR*front*back);

  return;
}


void
BladeBPMToyama::createLinks()
  /*!
    Create all the links
  */
{
  ELog::RegMethod RegA("BladeBPMToyama","createLinks");

  FrontBackCut::createLinks(*this,Origin,Y);


  return;
}

void
BladeBPMToyama::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("BladeBPMToyama","createAll");

  populate(System.getDataBase());
  createCentredUnitVector(FC,sideIndex,-length/2.0);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // xraySystem
