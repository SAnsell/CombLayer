/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   delft/H2Groove.cxx
 *
 * Copyright (c) 2004-2022 by Stuart Ansell
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
#include "Vec3D.h"
#include "Quaternion.h"
#include "surfRegister.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Importance.h"
#include "Object.h"
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
#include "H2Groove.h"

namespace delftSystem
{

H2Groove::H2Groove(const std::string& Key,const int NG)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedRotate(Key,0),
  attachSystem::CellMap(),
  gID(NG)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

H2Groove::H2Groove(const H2Groove& A) : 
  attachSystem::ContainedComp(A),
  attachSystem::FixedRotate(A),
  attachSystem::CellMap(A),
  gID(A.gID),
  height(A.height),xyAngleA(A.xyAngleA),xyAngleB(A.xyAngleB),
  siTemp(A.siTemp),siMat(A.siMat)
  /*!
    Copy constructor
    \param A :: H2Groove to copy
  */
{}

H2Groove&
H2Groove::operator=(const H2Groove& A)
  /*!
    Assignment operator
    \param A :: H2Groove to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedRotate::operator=(A);
      attachSystem::CellMap::operator=(A);
      height=A.height;
      xyAngleA=A.xyAngleA;
      xyAngleB=A.xyAngleB;
      siTemp=A.siTemp;
      siMat=A.siMat;
    }
  return *this;
}

H2Groove::~H2Groove() 
  /*!
    Destructor
  */
{}

void
H2Groove::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("H2Groove","populate");

  FixedRotate::populate(Control);
  
  const std::string keyNum(keyName+std::to_string(gID));

  xStep=Control.EvalPair<double>(keyNum+"XStep",keyName+"XStep");
  yStep=Control.EvalPair<double>(keyNum+"YStep",keyName+"YStep");
  zStep=Control.EvalPair<double>(keyNum+"ZStep",keyName+"ZStep");

  height=Control.EvalPair<double>(keyNum+"Height",keyName+"Height");
  xyAngleA=Control.EvalPair<double>(keyNum+"XYAngleA",keyName+"XYAngleA");
  xyAngleB=Control.EvalPair<double>(keyNum+"XYAngleB",keyName+"XYAngleB");
  
  siMat=ModelSupport::EvalMat<int>(Control,keyNum+"SiMat",keyNum+"SiMat");
  siTemp=Control.EvalPair<double>(keyNum+"SiTemp",keyName+"SiTemp");

  return;
}
  
void
H2Groove::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("H2Groove","createSurfaces");

  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*height/2.0,Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*height/2.0,Z);

  Geometry::Vec3D PtA(Y);
  Geometry::Vec3D PtB(Y);
  const Geometry::Quaternion QxyA=
    Geometry::Quaternion::calcQRotDeg(xyAngleA,Z);

  const Geometry::Quaternion QxyB=
    Geometry::Quaternion::calcQRotDeg(xyAngleB,Z);

  QxyA.rotate(PtA);
  QxyB.rotate(PtB);

  ModelSupport::buildPlane(SMap,buildIndex+7,Origin,
			   Origin+PtA,
			   Origin+PtA+Z,X);
  ModelSupport::buildPlane(SMap,buildIndex+8,Origin,
			   Origin+PtB,
			   Origin+PtB+Z,X);
			   
  return;
}

void
H2Groove::createLinks()
  /*!
    Create links
  */
{
  ELog::RegMethod RegA("H2Groove","createLinks");

  return;
}
  
void
H2Groove::createObjects(Simulation& System)
  /*!
    Build an H2 Groove moderator
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("H2Groove","createObjects");
  
  HeadRule HR;
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"7 -8 5 -6");
  addOuterSurf(HR);
  System.addCell(MonteCarlo::Object(cellIndex++,siMat,siTemp,HR));
  
  return;
}
  
void
H2Groove::createAll(Simulation& System,
		    const attachSystem::FixedComp& FC,
		    const long int sideIndex)

  /*!
    Generic function to create everything
    \param System :: Simulation to create objects in
    \param FUnit :: Fixed Base unit
    \param CC :: Boundary contained object
  */
{
  ELog::RegMethod RegA("H2Groove","createAll");
  populate(System.getDataBase());

  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       

  
  return;
}
  
}  // NAMESPACE delftSystem
