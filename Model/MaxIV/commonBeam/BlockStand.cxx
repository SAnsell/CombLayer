/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeam/BlockStand.cxx
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
#include "BlockStand.h"


namespace xraySystem
{

BlockStand::BlockStand(const std::string& Key) :
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(Key,2),
  attachSystem::CellMap(),attachSystem::SurfMap(),
  attachSystem::ExternalCut()
  /*!
    Default constructor
    \param Key :: Key name for variables
  */
{}

BlockStand::BlockStand(const BlockStand& A) : 
  attachSystem::ContainedComp(A),
  attachSystem::FixedOffset(A),attachSystem::CellMap(A),
  attachSystem::SurfMap(A),attachSystem::ExternalCut(A),
  height(A.height),width(A.width),length(A.length),
  mat(A.mat)
  /*!
    Copy constructor
    \param A :: BlockStand to copy
  */
{}

BlockStand&
BlockStand::operator=(const BlockStand& A)
  /*!
    Assignment operator
    \param A :: BlockStand to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      attachSystem::CellMap::operator=(A);
      attachSystem::SurfMap::operator=(A);
      attachSystem::ExternalCut::operator=(A);
      height=A.height;
      width=A.width;
      length=A.length;
      mat=A.mat;
    }
  return *this;
}
  
void
BlockStand::populate(const FuncDataBase& Control)
  /*!
    Sets the size of the object
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("BlockStand","populate");

  FixedOffset::populate(Control);
  height=Control.EvalVar<double>(keyName+"Height");
  width=Control.EvalVar<double>(keyName+"Width");
  length=Control.EvalVar<double>(keyName+"Length");
  
  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");
  
  return;
}

void
BlockStand::createUnitVector(const attachSystem::FixedComp& FC,
				 const long int sideIndex)
  /*!
    Create the unit vectors: Note only to construct front/back surf
    \param FC :: Centre point
    \param sideIndex :: Side index
  */
{
  ELog::RegMethod RegA("BlockStand","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
  return;
}

void
BlockStand::createSurfaces()
  /*!
    Create All the surfaces
   */
{
  ELog::RegMethod RegA("BlockStand","createSurface");

  if (!isActive("floor"))
    throw ColErr::InContainerError<std::string>
      ("floor","from ExternalCut Surf");

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

  ExternalCut::makeShiftedSurf(SMap,"floor",buildIndex+6,1,Z,height);
  
  return;
}

void
BlockStand::createObjects(Simulation& System) 
  /*!
    Creates the colllimator block
    \param System :: Simuation for object
  */
{
  ELog::RegMethod RegA("BlockStand","createObjects");

  std::string Out;
  const std::string fStr(getRuleStr("front"));
  const std::string bStr(getRuleStr("back"));
  const std::string baseStr(getRuleStr("floor"));
  
  Out=ModelSupport::getSetComposite(SMap,buildIndex," 3 -4 -6 ");
  Out+=fStr+bStr;
  makeCell("Block",System,cellIndex++,mat,0.0,Out+baseStr);


  addOuterSurf(Out);
  
  return;
}

void
BlockStand::createLinks()
  /*!
    Construct the links for the system
   */
{
  ELog::RegMethod RegA("BlockStand","createLinks");

  FixedComp::setConnect(0,Origin-Y*(length/2.0),-Y);
  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+1));
  FixedComp::setConnect(1,Origin+Y*(length/2.0),Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+2));      
  
  return;
}

void
BlockStand::createAll(Simulation& System,
                     const attachSystem::FixedComp& FC,
                     const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation 
    \param FC :: Fixed component to set axis etc
    \param sideIndex :: position of linkpoint
  */
{
  ELog::RegMethod RegA("BlockStand","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}
  
}  // NAMESPACE xraySystem
