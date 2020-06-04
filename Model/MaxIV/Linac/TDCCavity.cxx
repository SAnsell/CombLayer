/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/Linac/TDCCavity.cxx
 *
 * Copyright (c) 2004-2020 by Konstantin Batkov
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
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"

#include "TDCCavity.h"

namespace tdcSystem
{

TDCCavity::TDCCavity(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedRotate(Key,6),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  attachSystem::FrontBackCut()
 /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

TDCCavity::TDCCavity(const TDCCavity& A) :
  attachSystem::ContainedComp(A),
  attachSystem::FixedRotate(A),
  attachSystem::CellMap(A),
  attachSystem::SurfMap(A),
  attachSystem::FrontBackCut(A),
  length(A.length),radius(A.radius),height(A.height),
  wallThick(A.wallThick),
  mainMat(A.mainMat),wallMat(A.wallMat),
  couplerThick(A.couplerThick),
  couplerWidth(A.couplerWidth)
  /*!
    Copy constructor
    \param A :: TDCCavity to copy
  */
{}

TDCCavity&
TDCCavity::operator=(const TDCCavity& A)
  /*!
    Assignment operator
    \param A :: TDCCavity to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedRotate::operator=(A);
      attachSystem::CellMap::operator=(A);
      attachSystem::SurfMap::operator=(A);
      attachSystem::FrontBackCut::operator=(A);
      length=A.length;
      radius=A.radius;
      height=A.height;
      wallThick=A.wallThick;
      mainMat=A.mainMat;
      wallMat=A.wallMat;
      couplerThick=A.couplerThick;
      couplerWidth=A.couplerWidth;
    }
  return *this;
}

TDCCavity*
TDCCavity::clone() const
/*!
  Clone self
  \return new (this)
 */
{
    return new TDCCavity(*this);
}

TDCCavity::~TDCCavity()
  /*!
    Destructor
  */
{}

void
TDCCavity::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("TDCCavity","populate");

  FixedRotate::populate(Control);

  length=Control.EvalVar<double>(keyName+"Length");
  radius=Control.EvalVar<double>(keyName+"Radius");
  height=Control.EvalVar<double>(keyName+"Height");
  wallThick=Control.EvalVar<double>(keyName+"WallThick");

  mainMat=ModelSupport::EvalMat<int>(Control,keyName+"MainMat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  couplerThick=Control.EvalVar<double>(keyName+"CouplerThick");
  couplerWidth=Control.EvalVar<double>(keyName+"CouplerWidth");

  return;
}

void
TDCCavity::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("TDCCavity","createSurfaces");

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

  ModelSupport::buildPlane(SMap,buildIndex+11,Origin+Y*(wallThick),Y);
  ModelSupport::buildPlane(SMap,buildIndex+12,Origin+Y*(length-wallThick),Y);

  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,radius);
  ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Y,radius+wallThick);

  ModelSupport::buildPlane(SMap,buildIndex+101,Origin+Y*(couplerThick),Y);
  ModelSupport::buildPlane(SMap,buildIndex+102,Origin+Y*(length-couplerThick),Y);

  ModelSupport::buildPlane(SMap,buildIndex+103,Origin-X*(couplerWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+104,Origin+X*(couplerWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+105,Origin-Z*(couplerWidth/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+106,Origin+Z*(couplerWidth/2.0),Z);
  const double couplerR(couplerWidth/sqrt(2.0));
  ModelSupport::buildCylinder(SMap,buildIndex+107,Origin,Y,couplerR);

  return;
}

void
TDCCavity::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("TDCCavity","createObjects");

  std::string Out;
  const std::string frontStr=getRuleStr("front");
  const std::string backStr=getRuleStr("back");

  // front coupler cell
  Out=ModelSupport::getComposite(SMap,buildIndex," 7 -101 103 -104 105 -106 ")+frontStr;
  makeCell("FrontCouplerCell",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," -7 -11 ")+frontStr;
  makeCell("FrontCouplerFrontWall",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," -7 11 -101 ");
  makeCell("FrontCouplerInnerVoid",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," -107 -101 (-103:104:-105:106) ")+frontStr;
  makeCell("FrontCouplerOuterVoid",System,cellIndex++,0,0.0,Out);

  // back coupler cell
  Out=ModelSupport::getComposite(SMap,buildIndex," 7 102 103 -104 105 -106 ")+backStr;
  makeCell("BackCouplerCell",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," -7 12 ")+backStr;
  makeCell("BackCouplerFrontWall",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," -7 102 -12 ");
  makeCell("BackCouplerInnerVoid",System,cellIndex++,mainMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," -107 102 (-103:104:-105:106) ")+backStr;
  makeCell("BackCouplerOuterVoid",System,cellIndex++,0,0.0,Out);



  Out=ModelSupport::getComposite(SMap,buildIndex," 101 -102 -7 ");
  makeCell("InnerVoid",System,cellIndex++,0,0.0,Out);


  Out=ModelSupport::getComposite(SMap,buildIndex," 101 -102 7 -17 ");
  makeCell("Wall",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 101 -102 17 -107 ");
  makeCell("OuterVoid",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," -107 ")+frontStr+backStr;
  addOuterSurf(Out);

  return;
}


void
TDCCavity::createLinks()
  /*!
    Create all the linkes
  */
{
  ELog::RegMethod RegA("TDCCavity","createLinks");

  ExternalCut::createLink("front",*this,0,Origin,Y);
  ExternalCut::createLink("back",*this,1,Origin,Y);

  return;
}

void
TDCCavity::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("TDCCavity","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // tdcSystem
