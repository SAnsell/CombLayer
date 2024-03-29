/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   delft/H2Vac.cxx
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

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
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
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "ExternalCut.h"
#include "H2Vac.h"

namespace delftSystem
{

H2Vac::H2Vac(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(Key,6),
  attachSystem::ExternalCut()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

H2Vac::H2Vac(const H2Vac& A) : 
  attachSystem::ContainedComp(A),
  attachSystem::FixedOffset(A),
  attachSystem::ExternalCut(A),
  vacPosGap(A.vacPosGap),
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
      attachSystem::FixedOffset::operator=(A);
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

  sideRadius=Control.EvalDefVar<double>(keyName+"SideRadius",0.0);
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
H2Vac::getDirection(const size_t sideIndex) const
  /*!
    Determine the exit direction
    \param side :: Direction number [0-5] (internally checked)
    \return dirction vector
  */
{
  const Geometry::Vec3D DAxis[6]={-Y,Y,-X,X,-Z,Z};
  return DAxis[sideIndex];
}


Geometry::Vec3D
H2Vac::getSurfacePoint(const attachSystem::FixedComp& FC,
		       const size_t layerIndex,
                       const long int sideIndex) const
  /*!
    Get the center point for the surfaces in each layer
    \param FC :: Interanl object
    \param layer :: Layer number : 0 is inner 4 is outer
    \param sideIndex :: Index to side (back/front/left/right/up/down)
    
    \return point on surface
  */
{
  ELog::RegMethod RegA("H2Vac","getSurfacePoint");

  /// accessor to origin:
  if (!sideIndex) return Origin;

  const size_t SI((sideIndex>0) ?
                  static_cast<size_t>(sideIndex-1) :
                  static_cast<size_t>(-1-sideIndex));
  
  if (SI>2) 
    throw ColErr::IndexError<long int>(sideIndex,3,"sideIndex");
  if (layerIndex>4) 
    throw ColErr::IndexError<size_t>(layerIndex,4,"layer");
  
  const double frontDist[]={vacPosGap,alPos,terPos,outPos,clearPos};
  const double backDist[]={vacNegGap,alNeg,terNeg,outNeg,clearNeg};
  const double sideDist[]={vacSide,alSide,terSide,outSide,clearSide};

  const double* DPtr[]={backDist,frontDist,sideDist};

  const Geometry::Vec3D XYZ=getDirection(SI);

  double sumVec(0.0);
  for(size_t i=0;i<=layerIndex;i++)
    sumVec+=DPtr[SI][i];
  
  return FC.getLinkPt(sideIndex)+XYZ*sumVec;
}

void
H2Vac::createSurfaces(const attachSystem::FixedComp& FC)
  /*!
    Create All the surfaces
    \param FC :: A  FixedComp for the front/back surfaces
  */
{
  ELog::RegMethod RegA("H2Vac","createSurfaces");


  if (sideRadius<Geometry::zeroTol)
    sideRadius=FC.getLinkDistance(0,3);
  
  ELog::EM<<"Side radius == "<<sideRadius<<ELog::endDiag;

  // Inner Layers:

  ModelSupport::buildCylinder(SMap,buildIndex+1,
			      getSurfacePoint(FC,0,2)-Y*vacPosRadius,
			      Z,vacPosRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+2,
			      getSurfacePoint(FC,0,1)+Y*vacNegRadius,
			      Z,vacNegRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+3,Origin,
			      Y,sideRadius+vacSide);

  // SECOND LAYER:
  ModelSupport::buildCylinder(SMap,buildIndex+11,
			      getSurfacePoint(FC,1,2)-Y*vacPosRadius,
			      Z,vacPosRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+12,
			      getSurfacePoint(FC,1,1)+Y*vacNegRadius,
			      Z,vacNegRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+13,Origin,Y,
			      sideRadius+vacSide+alSide);

  // TERTIARY LAYER:
  ModelSupport::buildCylinder(SMap,buildIndex+21,
			      getSurfacePoint(FC,2,2)-Y*vacPosRadius,
			      Z,vacPosRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+22,
			      getSurfacePoint(FC,2,1)+Y*vacNegRadius,
			      Z,vacNegRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+23,Origin,Y,
			      sideRadius+vacSide+alSide+terSide);


  // Outer AL LAYER:
  ModelSupport::buildCylinder(SMap,buildIndex+31,
			      getSurfacePoint(FC,3,2)-Y*vacPosRadius,
			      Z,vacPosRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+32,
			      getSurfacePoint(FC,3,1)+Y*vacNegRadius,
			      Z,vacNegRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+33,Origin,Y,
			      sideRadius+vacSide+alSide+terSide+outSide);
  
  // Outer Clearance
  ModelSupport::buildCylinder(SMap,buildIndex+41,
			      getSurfacePoint(FC,4,2)-Y*vacPosRadius,
			      Z,vacPosRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+42,
			      getSurfacePoint(FC,4,1)+Y*vacNegRadius,
			      Z,vacNegRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+43,Origin,Y,
			      sideRadius+vacSide+alSide
			      +terSide+outSide+clearSide);

  return;
}

void
H2Vac::createObjects(Simulation& System)

  /*!
    Adds a H2 Vacuum system to a moderator system
    \param System :: Simulation to create objects in
    \param Exclude :: ContainedObject exclude string
  */
{
  ELog::RegMethod RegA("H2Vac","createObjects");

  const HeadRule& ExcludeHR=getRule("Exclude");

  HeadRule HR;

  // Inner 
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-1 -2 -3");
  System.addCell(cellIndex++,0,0.0,HR*ExcludeHR);

  // First Al layer
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-11 -12 -13 ( 1:2:3)");
  System.addCell(cellIndex++,alMat,0.0,HR);

  // Tertiay layer
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-21 -22 -23 (11:12:13)");
  System.addCell(cellIndex++,0,0.0,HR);

  // Tertiay layer
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-31 -32 -33 (21:22:23)");
  System.addCell(cellIndex++,outMat,0.0,HR);

  // Outer clearance
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-41 -42 -43 (31:32:33)");
  System.addCell(cellIndex++,0,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-41 -42 -43 ");
  addOuterSurf(HR);

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
		 const long int sideIndex)
  /*!
    Global creation of the vac-vessel
    \param System :: Simulation to add vessel to
    \param FC :: Previously build  moderator
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("H2Vac","createAll");
  populate(System.getDataBase());
 
  createUnitVector(FC,sideIndex);                       // fixed 
  createSurfaces(FC);
  createObjects(System);
  createLinks();
  insertObjects(System);       

  return;
}

  
}  // NAMESPACE delftSystem
