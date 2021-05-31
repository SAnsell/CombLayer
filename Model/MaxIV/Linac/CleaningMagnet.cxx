/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/Linac/CleaningMagnet.cxx
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

#include "CleaningMagnet.h"

namespace tdcSystem
{

CleaningMagnet::CleaningMagnet(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedRotate(Key,6),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  attachSystem::ExternalCut()
 /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

CleaningMagnet::CleaningMagnet(const CleaningMagnet& A) :
  attachSystem::ContainedComp(A),
  attachSystem::FixedRotate(A),
  attachSystem::CellMap(A),
  attachSystem::SurfMap(A),
  attachSystem::ExternalCut(A),
  length(A.length),width(A.width),height(A.height),
  gap(A.gap),
  yokeLength(A.yokeLength),
  yokeDepth(A.yokeDepth),
  yokeThick(A.yokeThick),
  mat(A.mat),yokeMat(A.yokeMat),
  voidMat(A.voidMat)
  /*!
    Copy constructor
    \param A :: CleaningMagnet to copy
  */
{}

CleaningMagnet&
CleaningMagnet::operator=(const CleaningMagnet& A)
  /*!
    Assignment operator
    \param A :: CleaningMagnet to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedRotate::operator=(A);
      attachSystem::CellMap::operator=(A);
      attachSystem::SurfMap::operator=(A);
      attachSystem::ExternalCut::operator=(A);
      length=A.length;
      width=A.width;
      height=A.height;
      gap=A.gap;
      yokeLength=A.yokeLength;
      yokeDepth=A.yokeDepth;
      yokeThick=A.yokeThick;
      mat=A.mat;
      yokeMat=A.yokeMat;
      voidMat=A.voidMat;
    }
  return *this;
}

CleaningMagnet*
CleaningMagnet::clone() const
/*!
  Clone self
  \return new (this)
 */
{
    return new CleaningMagnet(*this);
}

CleaningMagnet::~CleaningMagnet()
  /*!
    Destructor
  */
{}

void
CleaningMagnet::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("CleaningMagnet","populate");

  FixedRotate::populate(Control);

  length=Control.EvalVar<double>(keyName+"Length");
  width=Control.EvalVar<double>(keyName+"Width");
  height=Control.EvalVar<double>(keyName+"Height");
  gap=Control.EvalVar<double>(keyName+"Gap");
  yokeLength=Control.EvalVar<double>(keyName+"YokeLength");
  yokeDepth=Control.EvalVar<double>(keyName+"YokeDepth");
  yokeThick=Control.EvalVar<double>(keyName+"YokeThick");

  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");
  yokeMat=ModelSupport::EvalMat<int>(Control,keyName+"YokeMat");
  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");

  return;
}

void
CleaningMagnet::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("CleaningMagnet","createSurfaces");

  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length/2.0),Y);

  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(gap/2.0+width),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin-X*(gap/2.0),X);

  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(height/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(height/2.0),Z);

  ModelSupport::buildPlane(SMap,buildIndex+13,Origin+X*(gap/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+14,Origin+X*(gap/2.0+width),X);

  ModelSupport::buildPlane(SMap,buildIndex+101,Origin-Y*(yokeLength/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+102,Origin+Y*(yokeLength/2.0),Y);

  ModelSupport::buildShiftedPlane(SMap,buildIndex+103,buildIndex+3,X,-yokeThick);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+104,buildIndex+14,X,yokeThick);

  ModelSupport::buildPlane(SMap,buildIndex+105,Origin-Z*(yokeDepth),Z);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+106,buildIndex+105,X,yokeThick);

  return;
}

void
CleaningMagnet::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("CleaningMagnet","createObjects");

  std::string Out;
  const std::string ICell=isActive("Inner") ? getRuleStr("Inner") : "";

  // Core
  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -2 3 -4 5 -6 ");
  makeCell("Core",System,cellIndex++,mat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -2 4 -13 5 -6 ");
  makeCell("Gap",System,cellIndex++,voidMat,0.0,Out+ICell);

  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -2 13 -14 5 -6 ");
  makeCell("Core",System,cellIndex++,mat,0.0,Out);

  // Yoke
  Out=ModelSupport::getComposite(SMap,buildIndex," 103 -3 105 -6 101 -102 ");
  makeCell("Yoke",System,cellIndex++,yokeMat,0.0,Out);
  Out=ModelSupport::getComposite(SMap,buildIndex," 14 -104 105 -6 101 -102 ");
  makeCell("Yoke",System,cellIndex++,yokeMat,0.0,Out);
  Out=ModelSupport::getComposite(SMap,buildIndex," 3 -14 105 -106 101 -102 ");
  makeCell("Yoke",System,cellIndex++,yokeMat,0.0,Out);
  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -2 3 -14 106 -5 ");
  makeCell("YokeVoid",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 103 -3 105 -6 1 -101 ");
  makeCell("YokeVoid",System,cellIndex++,voidMat,0.0,Out);
  Out=ModelSupport::getComposite(SMap,buildIndex," 14 -104 105 -6 1 -101 ");
  makeCell("YokeVoid",System,cellIndex++,voidMat,0.0,Out);
  Out=ModelSupport::getComposite(SMap,buildIndex," 3 -14 105 -106 1 -101 ");
  makeCell("YokeVoid",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 103 -3 105 -6 102 -2 ");
  makeCell("YokeVoid",System,cellIndex++,voidMat,0.0,Out);
  Out=ModelSupport::getComposite(SMap,buildIndex," 14 -104 105 -6 102 -2 ");
  makeCell("YokeVoid",System,cellIndex++,voidMat,0.0,Out);
  Out=ModelSupport::getComposite(SMap,buildIndex," 3 -14 105 -106 102 -2 ");
  makeCell("YokeVoid",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 103 -104 105 -6 1 -2 ");
  addOuterSurf(Out);

  return;
}


void
CleaningMagnet::createLinks()
  /*!
    Create all the linkes
  */
{
  ELog::RegMethod RegA("CleaningMagnet","createLinks");

  FixedComp::setConnect(0,Origin-Y*(length/2.0),-Y);
  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+1));
  FixedComp::setConnect(1,Origin+Y*(length/2.0),Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+2));

  FixedComp::setConnect(2,Origin-X*(gap/2.0+width+yokeThick),-X);
  FixedComp::setLinkSurf(2,-SMap.realSurf(buildIndex+103));
  FixedComp::setConnect(3,Origin+X*(gap/2.0+width+yokeThick),X);
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+104));

  FixedComp::setConnect(4,Origin-Z*(yokeDepth),-Z);
  FixedComp::setLinkSurf(4,-SMap.realSurf(buildIndex+105));
  FixedComp::setConnect(5,Origin+Z*(height/2.0),Z);
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+6));

  return;
}

void
CleaningMagnet::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("CleaningMagnet","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // tdcSystem
