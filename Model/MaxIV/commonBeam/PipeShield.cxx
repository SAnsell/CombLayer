/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeam/PipeShield.cxx
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
#include "HeadRule.h"
#include "Object.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "ExternalCut.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "PipeShield.h"


namespace xraySystem
{

PipeShield::PipeShield(const std::string& Key) :
  attachSystem::ContainedGroup("Main","Wings"),
  attachSystem::FixedOffset(Key,2),
  attachSystem::CellMap(),attachSystem::SurfMap(),
  attachSystem::ExternalCut()
  /*!
    Default constructor
    \param Key :: Key name for variables
  */
{}

PipeShield::PipeShield(const PipeShield& A) : 
  attachSystem::ContainedGroup(A),
  attachSystem::FixedOffset(A),attachSystem::CellMap(A),
  attachSystem::SurfMap(A),attachSystem::ExternalCut(A),
  height(A.height),width(A.width),length(A.length),
  clearGap(A.clearGap),wallThick(A.wallThick),mat(A.mat),
  wallMat(A.wallMat)
  /*!
    Copy constructor
    \param A :: PipeShield to copy
  */
{}

PipeShield&
PipeShield::operator=(const PipeShield& A)
  /*!
    Assignment operator
    \param A :: PipeShield to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedGroup::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      attachSystem::CellMap::operator=(A);
      attachSystem::SurfMap::operator=(A);
      attachSystem::ExternalCut::operator=(A);
      height=A.height;
      width=A.width;
      length=A.length;
      clearGap=A.clearGap;
      wallThick=A.wallThick;
      mat=A.mat;
      wallMat=A.wallMat;
    }
  return *this;
}
  
void
PipeShield::populate(const FuncDataBase& Control)
  /*!
    Sets the size of the object
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("PipeShield","populate");

  FixedOffset::populate(Control);
  height=Control.EvalVar<double>(keyName+"Height");
  width=Control.EvalVar<double>(keyName+"Width");
  length=Control.EvalVar<double>(keyName+"Length");
  clearGap=Control.EvalVar<double>(keyName+"ClearGap");
  wallThick=Control.EvalVar<double>(keyName+"WallThick");
  
  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");

  wingThick=Control.EvalDefVar<double>(keyName+"WingThick",0.0);
  wingLength=Control.EvalDefVar<double>(keyName+"WingLength",0.0);
  wingMat=ModelSupport::EvalDefMat<int>(Control,keyName+"WingMat",-1);

  if (wingThick<Geometry::zeroTol || wingLength<Geometry::zeroTol)
    wingMat=-1;
  
  return;
}

void
PipeShield::createUnitVector(const attachSystem::FixedComp& FC,
				 const long int sideIndex)
  /*!
    Create the unit vectors: Note only to construct front/back surf
    \param FC :: Centre point
    \param sideIndex :: Side index
  */
{
  ELog::RegMethod RegA("PipeShield","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
  return;
}

void
PipeShield::createSurfaces()
  /*!
    Create All the surfaces
   */
{
  ELog::RegMethod RegA("PipeShield","createSurface");

  if (!isActive("front"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
      ExternalCut::setCutSurf("front",SMap.realSurf(buildIndex+1));
    }
  if (!isActive("back"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*length,Y);
      ExternalCut::setCutSurf("back",-SMap.realSurf(buildIndex+2));
    }

  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(height/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(height/2.0),Z);

  ModelSupport::buildPlane(SMap,buildIndex+13,
			   Origin-X*(wallThick+width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+14,
			   Origin+X*(wallThick+width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+15,
			   Origin-Z*(wallThick+height/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,
			   Origin+Z*(wallThick+height/2.0),Z);

  if (isActive("inner"))
    ExternalCut::makeExpandedSurf(SMap,"inner",buildIndex+7,Origin,clearGap);
  
  if (isActive("innerTwo"))
    ExternalCut::makeExpandedSurf(SMap,"innerTwo",
				  buildIndex+17,Origin,clearGap);


  if (wingMat>=0)
    {
      ModelSupport::buildPlane(SMap,buildIndex+101,Origin-Y*wingLength,Y);
      ModelSupport::buildPlane(SMap,buildIndex+103,
			       Origin-X*(wallThick+width/2.0-wingThick),X);
      ModelSupport::buildPlane(SMap,buildIndex+104,
			       Origin+X*(wallThick+width/2.0-wingThick),X);
    }      
  
  return;
}

void
PipeShield::createObjects(Simulation& System) 
  /*!
    Creates the colllimator block
    \param System :: Simuation for object
  */
{
  ELog::RegMethod RegA("PipeShield","createObjects");

  std::string Out;
  const std::string fStr(getRuleStr("front"));
  const std::string bStr(getRuleStr("back"));
  const std::string rAStr(getRuleStr("inner"));
  const std::string rBStr(getRuleStr("innerTwo"));

  // inner clearance gap
  if (!rAStr.empty())
    {
      Out=ModelSupport::getSetComposite(SMap,buildIndex," -7 ");
      Out+=fStr+bStr+rAStr;
      makeCell("clearGap",System,cellIndex++,0,0.0,Out);
    }

  if (!rBStr.empty())
    {
      Out=ModelSupport::getSetComposite(SMap,buildIndex," -17 ");
      Out+=fStr+bStr+rBStr;
      makeCell("clearGap",System,cellIndex++,0,0.0,Out);
    }

  Out=ModelSupport::getSetComposite(SMap,buildIndex," 3 -4 5 -6 7 17 ");
  Out+=fStr+bStr;
  makeCell("Main",System,cellIndex++,mat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,
				 " 13 -14 15 -16 (-3:4:-5:6) ");
  Out+=fStr+bStr;
  makeCell("Wall",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 13 -14 15 -16 ");
  Out+=fStr+bStr;

  addOuterSurf("Main",Out);


  if (wingMat>=0)
    {
      const std::string fComp(getComplementStr("front"));
      Out=ModelSupport::getComposite(SMap,buildIndex," 101 13 -103  15 -16 ");
      Out+=fComp;
      makeCell("WingA",System,cellIndex++,wingMat,0.0,Out);
	    
      addOuterSurf("Wings",Out);

      Out=ModelSupport::getComposite(SMap,buildIndex," 101 104 -14  15 -16 ");
      Out+=fComp;
      makeCell("WingB",System,cellIndex++,wingMat,0.0,Out);

      addOuterUnionSurf("Wings",Out);
    }
  
  return;
}

void
PipeShield::createLinks()
  /*!
    Construct the links for the system
   */
{
  ELog::RegMethod RegA("PipeShield","createLinks");

  ExternalCut::createLink("front",*this,0,Origin,-Y);
  ExternalCut::createLink("back",*this,1,Origin,Y);
  
  return;
}

void
PipeShield::createAll(Simulation& System,
                     const attachSystem::FixedComp& FC,
                     const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation 
    \param FC :: Fixed component to set axis etc
    \param sideIndex :: position of linkpoint
  */
{
  ELog::RegMethod RegA("PipeShield","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}
  
}  // NAMESPACE xraySystem
