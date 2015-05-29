/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   delft/H2Vac.cxx
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#include "localRotate.h"
#include "masterRotate.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfEqual.h"
#include "surfDivide.h"
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
#include "chipDataStore.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "H2Vac.h"

namespace delftSystem
{

H2Vac::H2Vac(const std::string& Key)  :
  attachSystem::ContainedComp(),attachSystem::FixedComp(Key,6),
  vacIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(vacIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

H2Vac::H2Vac(const H2Vac& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedComp(A),
  vacIndex(A.vacIndex),cellIndex(A.cellIndex),vacPosGap(A.vacPosGap),
  vacNegGap(A.vacNegGap),vacPosRadius(A.vacPosRadius),
  vacNegRadius(A.vacNegRadius),vacSide(A.vacSide),
  alPos(A.alPos),alNeg(A.alNeg),alSide(A.alSide),
  terPos(A.terPos),terNeg(A.terNeg),terSide(A.terSide),
  outPos(A.outPos),outNeg(A.outNeg),outSide(A.outSide),
  clearPos(A.clearPos),clearNeg(A.clearNeg),clearSide(A.clearSide),
  alMat(A.alMat),outMat(A.outMat)
  /*!
    Copy constructor
    \param A :: H2Vac to copy
  */
{}

H2Vac&
H2Vac::operator=(const H2Vac& A)
  /*!
    Assignment operator
    \param A :: H2Vac to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedComp::operator=(A);
      cellIndex=A.cellIndex;
      vacPosGap=A.vacPosGap;
      vacNegGap=A.vacNegGap;
      vacPosRadius=A.vacPosRadius;
      vacNegRadius=A.vacNegRadius;
      vacSide=A.vacSide;
      alPos=A.alPos;
      alNeg=A.alNeg;
      alSide=A.alSide;
      terPos=A.terPos;
      terNeg=A.terNeg;
      terSide=A.terSide;
      outPos=A.outPos;
      outNeg=A.outNeg;
      outSide=A.outSide;
      clearPos=A.clearPos;
      clearNeg=A.clearNeg;
      clearSide=A.clearSide;
      alMat=A.alMat;
      outMat=A.outMat;
    }
  return *this;
}

H2Vac::~H2Vac() 
 /*!
   Destructor
 */
{}

void
H2Vac::populate(const FuncDataBase& Control)
 /*!
   Populate all the variables
   \param Control :: Variable DataBase
 */
{
  ELog::RegMethod RegA("H2Vac","populate");
  
  // First get inner widths:
  vacPosGap=Control.EvalVar<double>(keyName+"VacPosGap");
  vacNegGap=Control.EvalVar<double>(keyName+"VacNegGap");
  vacPosRadius=Control.EvalVar<double>(keyName+"VacPosRadius");
  vacNegRadius=Control.EvalVar<double>(keyName+"VacNegRadius");

  // Vac values 
  vacSide=Control.EvalVar<double>(keyName+"VacSide");

  // Al values
  alPos=Control.EvalVar<double>(keyName+"AlPos");
  alNeg=Control.EvalVar<double>(keyName+"AlNeg");
  alSide=Control.EvalVar<double>(keyName+"AlSide");

  // Ter values
  terPos=Control.EvalVar<double>(keyName+"TerPos");
  terNeg=Control.EvalVar<double>(keyName+"TerNeg");
  terSide=Control.EvalVar<double>(keyName+"TerSide");

  // Out values
  outPos=Control.EvalVar<double>(keyName+"OutPos"); 
  outNeg=Control.EvalVar<double>(keyName+"OutNeg");
  outSide=Control.EvalVar<double>(keyName+"OutSide");

  // Out values
  clearPos=Control.EvalVar<double>(keyName+"ClearPos");
  clearNeg=Control.EvalVar<double>(keyName+"ClearNeg");
  clearSide=Control.EvalVar<double>(keyName+"ClearSide");
  
  // Material
  alMat=ModelSupport::EvalMat<int>(Control,keyName+"AlMat");
  outMat=ModelSupport::EvalMat<int>(Control,keyName+"OutMat");

  return;
}
  

Geometry::Vec3D
H2Vac::getDirection(const size_t side) const
  /*!
    Determine the exit direction
    \param side :: Direction number [0-5] (internally checked)
    \return dirction vector
  */
{
  switch (side) 
    {
    case 0:
      return -Y;
    case 1:
      return Y;
    case 2:
      return -X;
    case 3:
      return X;
    case 4:
      return -Z;
    }
  return Z;
}


Geometry::Vec3D
H2Vac::getSurfacePoint(const attachSystem::FixedComp& FC,
		       const size_t side,const size_t layer) const
  /*!
    Get the center point for the surfaces in each layer
    \param FC :: Interanl object
    \param side :: Index to side (back/front/left/right/up/down)
    \param layer :: Layer number : 0 is inner 4 is outer
    \return point on surface
  */
{
  ELog::RegMethod RegA("H2Vac","getSurfacePoint");

  if (side>2) 
    throw ColErr::IndexError<size_t>(side,3,"sideIndex");
  if (layer>4) 
    throw ColErr::IndexError<size_t>(side,3,"layer");
  
  const double frontDist[]={vacPosGap,alPos,terPos,outPos,clearPos};
  const double backDist[]={vacNegGap,alNeg,terNeg,outNeg,clearNeg};
  const double sideDist[]={vacSide,alSide,terSide,outSide,clearSide};

  const double* DPtr[]={backDist,frontDist,sideDist};

  const Geometry::Vec3D XYZ=getDirection(side);

  double sumVec(0.0);
  for(size_t i=0;i<=layer;i++)
    sumVec+=DPtr[side][i];
  
  return FC.getLinkPt(side)+XYZ*sumVec;
}

void
H2Vac::createUnitVector(const attachSystem::FixedComp& FC)
  /*!
    Create the unit vectors
    - Y Points down the Groove direction
    - X Across the Groove
    - Z up (towards the target)
    \param FC :: A Contained FixedComp to use as basis set
  */
{
  ELog::RegMethod RegA("H2Vac","createUnitVector");
  FixedComp::createUnitVector(FC);
  return;
}

void
H2Vac::createSurfaces(const attachSystem::FixedComp& FC)
  /*!
    Create All the surfaces
    \param FC :: A  FixedComp for the front/back surfaces
  */
{
  ELog::RegMethod RegA("H2Vac","createSurface");

  const double sideRadius=(Origin-FC.getLinkPt(2)).abs();
  ELog::EM<<"Side radius == "<<sideRadius<<ELog::endDebug;

  // Inner Layers:

  ModelSupport::buildCylinder(SMap,vacIndex+1,
			      getSurfacePoint(FC,1,0)-Y*vacPosRadius,
			      Z,vacPosRadius);
  ModelSupport::buildCylinder(SMap,vacIndex+2,
			      getSurfacePoint(FC,0,0)+Y*vacNegRadius,
			      Z,vacNegRadius);
  ModelSupport::buildCylinder(SMap,vacIndex+3,Origin,
			      Y,sideRadius+vacSide);

  // SECOND LAYER:
  ModelSupport::buildCylinder(SMap,vacIndex+11,
			      getSurfacePoint(FC,1,1)-Y*vacPosRadius,
			      Z,vacPosRadius);
  ModelSupport::buildCylinder(SMap,vacIndex+12,
			      getSurfacePoint(FC,0,1)+Y*vacNegRadius,
			      Z,vacNegRadius);
  ModelSupport::buildCylinder(SMap,vacIndex+13,Origin,Y,
			      sideRadius+vacSide+alSide);

  // TERTIARY LAYER:
  ModelSupport::buildCylinder(SMap,vacIndex+21,
			      getSurfacePoint(FC,1,2)-Y*vacPosRadius,
			      Z,vacPosRadius);
  ModelSupport::buildCylinder(SMap,vacIndex+22,
			      getSurfacePoint(FC,0,2)+Y*vacNegRadius,
			      Z,vacNegRadius);
  ModelSupport::buildCylinder(SMap,vacIndex+23,Origin,Y,
			      sideRadius+vacSide+alSide+terSide);


  // Outer AL LAYER:
  ModelSupport::buildCylinder(SMap,vacIndex+31,
			      getSurfacePoint(FC,1,3)-Y*vacPosRadius,
			      Z,vacPosRadius);
  ModelSupport::buildCylinder(SMap,vacIndex+32,
			      getSurfacePoint(FC,0,3)+Y*vacNegRadius,
			      Z,vacNegRadius);
  ModelSupport::buildCylinder(SMap,vacIndex+33,Origin,Y,
			      sideRadius+vacSide+alSide+terSide+outSide);
  
  // Outer Clearance
  ModelSupport::buildCylinder(SMap,vacIndex+41,
			      getSurfacePoint(FC,1,4)-Y*vacPosRadius,
			      Z,vacPosRadius);
  ModelSupport::buildCylinder(SMap,vacIndex+42,
			      getSurfacePoint(FC,0,4)+Y*vacNegRadius,
			      Z,vacNegRadius);
  ModelSupport::buildCylinder(SMap,vacIndex+43,Origin,Y,
			      sideRadius+vacSide+alSide
			      +terSide+outSide+clearSide);

  return;
}

void
H2Vac::createObjects(Simulation& System,const std::string& Exclude)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
    \param Exclude :: ContainedObject exclude string
  */
{
  ELog::RegMethod RegA("H2Vac","createObjects");
  
  std::string Out;
  Out=ModelSupport::getComposite(SMap,vacIndex,"-41 -42 -43 ");
  addOuterSurf(Out);

  // Inner 
  Out=ModelSupport::getComposite(SMap,vacIndex,"-1 -2 -3 ");
  Out+=" "+Exclude;
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

  // First Al layer
  Out=ModelSupport::getComposite(SMap,vacIndex,"-11 -12 -13 ( 1:2:3) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,alMat,0.0,Out));

  // Tertiay layer
  Out=ModelSupport::getComposite(SMap,vacIndex,"-21 -22 -23 (11:12:13) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

  // Tertiay layer
  Out=ModelSupport::getComposite(SMap,vacIndex,"-31 -32 -33 (21:22:23)");
  System.addCell(MonteCarlo::Qhull(cellIndex++,outMat,0.0,Out));

  // Outer clearance
  Out=ModelSupport::getComposite(SMap,vacIndex,"-41 -42 -43 (31:32:33)");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

  return;
}

void
H2Vac::createLinks()
  /*!
    Creates a full attachment set
  */
{

  return;
}

void
H2Vac::createAll(Simulation& System,
		 const attachSystem::FixedComp& FC,
		 const attachSystem::ContainedComp& CC)
  /*!
    Global creation of the vac-vessel
    \param System :: Simulation to add vessel to
    \param FC :: Previously build  moderator
    \param CC :: Container to wrap
  */
{
  ELog::RegMethod RegA("H2Vac","createAll");
  populate(System.getDataBase());
 
  createUnitVector(FC);                       // fixed 
  createSurfaces(FC);
  createObjects(System,CC.getExclude());
  createLinks();
  insertObjects(System);       

  return;
}

  
}  // NAMESPACE delftSystem
