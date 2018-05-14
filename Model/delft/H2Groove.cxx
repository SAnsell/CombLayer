/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   delft/H2Groove.cxx
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#include "stringCombine.h"
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
#include "Qhull.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
// #include "SecondTrack.h"
#include "ContainedComp.h"
#include "pipeUnit.h"
#include "PipeLine.h"
#include "H2Groove.h"

namespace delftSystem
{

H2Groove::H2Groove(const std::string& Key,const int NG)  :
  attachSystem::ContainedComp(),attachSystem::FixedOffset(Key,0),
  gIndex(NG),siIndex(ModelSupport::objectRegister::Instance().cell(Key,NG)),
  cellIndex(siIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

H2Groove::H2Groove(const H2Groove& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedOffset(A),
  gIndex(A.gIndex),siIndex(A.siIndex),cellIndex(A.cellIndex),
  face(A.face),height(A.height),xyAngleA(A.xyAngleA),xyAngleB(A.xyAngleB),
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
      attachSystem::FixedOffset::operator=(A);
      cellIndex=A.cellIndex;
      face=A.face;
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

  FixedOffset::populate(Control);

  
  const std::string keyNum(keyName+StrFunc::makeString(gIndex));

  face=Control.EvalPair<int>(keyNum+"Face",keyName+"Face");

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
H2Groove::createUnitVector(const attachSystem::FixedComp& FUnit,
			   const long int sideIndex)
  /*!
    Create the unit vectors
    - Y Points down the H2Groove direction
    - X Across the H2Groove
    - Z up (towards the target)
    \param FUnit :: Fixed unit that it is connected to 
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("H2Groove","createUnitVector");
  
  // Opposite since other face:
  attachSystem::FixedComp::createUnitVector(FUnit,sideIndex);
  FixedOffset::applyOffset();

  return;
}
  
void
H2Groove::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("H2Groove","createSurfaces");

  ModelSupport::buildPlane(SMap,siIndex+5,Origin-Z*height/2.0,Z);
  ModelSupport::buildPlane(SMap,siIndex+6,Origin+Z*height/2.0,Z);

  const Geometry::Quaternion QxyA=
    Geometry::Quaternion::calcQRotDeg(xyAngleA,Z);
  Geometry::Vec3D PtA(Y*face);

  const Geometry::Quaternion QxyB=
    Geometry::Quaternion::calcQRotDeg(xyAngleB,Z);
  Geometry::Vec3D PtB(Y*face);

  QxyA.rotate(PtA);
  QxyB.rotate(PtB);

  ModelSupport::buildPlane(SMap,siIndex+7,Origin,
			   Origin+PtA,
			   Origin+PtA+Z,X);
  ModelSupport::buildPlane(SMap,siIndex+8,Origin,
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
H2Groove::createObjects(Simulation& System,
			const attachSystem::ContainedComp& CC)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("H2Groove","createObjects");
  
  std::string Out;
  Out=ModelSupport::getComposite(SMap,siIndex,"7 -8 5 -6 ");
  addOuterSurf(Out);
  Out+=CC.getContainer();
  System.addCell(MonteCarlo::Qhull(cellIndex++,siMat,siTemp,Out));
  
  return;
}
  
void
H2Groove::createAll(Simulation& System,
		    const attachSystem::FixedComp& FUnit,
		    const attachSystem::ContainedComp& CC)
  /*!
    Generic function to create everything
    \param System :: Simulation to create objects in
    \param FUnit :: Fixed Base unit
    \param CC :: Boundary contained object
  */
{
  ELog::RegMethod RegA("H2Groove","createAll");
  populate(System.getDataBase());

  if (face)
    {
      if (face<0)
	createUnitVector(FUnit,1);
      else 
	createUnitVector(FUnit,2);

      createSurfaces();
      createObjects(System,CC);
      createLinks();
      insertObjects(System);       
    }
  
  return;
}
  
}  // NAMESPACE delftSystem
