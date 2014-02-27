/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   imat/IMatBulkInsert.cxx
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#include <numeric>
#include <iterator>
#include <boost/shared_ptr.hpp>
#include <boost/array.hpp>

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
#include "surfEqual.h"
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
#include "SimProcess.h"
#include "SurInter.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "SecondTrack.h"
#include "TwinComp.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "BulkInsert.h"
#include "IMatBulkInsert.h"


namespace shutterSystem
{

IMatBulkInsert::IMatBulkInsert(const size_t ID,const std::string& BKey,
			       const std::string& IKey)  : 
  BulkInsert(ID,BKey),keyName(IKey),
  insIndex(ModelSupport::objectRegister::Instance().cell(IKey)),
  cellIndex(insIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param ID :: Shutter number
    \param BKey :: Bulk Key
    \param IKey :: Insert Key to use
  */
{}

IMatBulkInsert::IMatBulkInsert(const IMatBulkInsert& A) : 
  BulkInsert(A),
  keyName(A.keyName),insIndex(A.insIndex),
  cellIndex(A.cellIndex),xStep(A.xStep),yStep(A.yStep),
  zStep(A.zStep),xyAngle(A.xyAngle),zAngle(A.zAngle),
  frontGap(A.frontGap),width(A.width),height(A.height),
  defMat(A.defMat)
  /*!
    Copy constructor
    \param A :: IMatBulkInsert to copy
  */
{}

IMatBulkInsert&
IMatBulkInsert::operator=(const IMatBulkInsert& A)
  /*!
    Assignment operator
    \param A :: IMatBulkInsert to copy
    \return *this
  */
{
  if (this!=&A)
    {
      BulkInsert::operator=(A);
      cellIndex=A.cellIndex;
      xStep=A.xStep;
      yStep=A.yStep;
      zStep=A.zStep;
      xyAngle=A.xyAngle;
      zAngle=A.zAngle;
      frontGap=A.frontGap;
      width=A.width;
      height=A.height;
      defMat=A.defMat;
    }
  return *this;
}

IMatBulkInsert::~IMatBulkInsert() 
  /*!
    Destructor
  */
{}

void
IMatBulkInsert::populate(const Simulation& System)
  /*!
    Populate all the variables
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("IMatBulkInsert","populate");

  const FuncDataBase& Control=System.getDataBase();

  xStep=Control.EvalVar<double>(keyName+"XStep");
  yStep=Control.EvalVar<double>(keyName+"YStep");
  zStep=Control.EvalVar<double>(keyName+"ZStep");
  xyAngle=Control.EvalVar<double>(keyName+"XYAngle");
  zAngle=Control.EvalVar<double>(keyName+"ZAngle");

  frontGap=Control.EvalVar<double>(keyName+"FrontGap");
  width=Control.EvalVar<double>(keyName+"Width");
  height=Control.EvalVar<double>(keyName+"Height");
  
  defMat=ModelSupport::EvalMat<int>(Control,keyName+"DefMat");
    
  return;
}

void
IMatBulkInsert::createUnitVector()
  /*!
    Create the unit vectors
  */
{
  ELog::RegMethod RegA("IMatBulkInsert","createUnitVector");

  bEnter+=bX*xStep+bY*yStep+bZ*zStep;
  if (fabs(xyAngle)>Geometry::zeroTol ||
      fabs(zAngle)>Geometry::zeroTol)
    {
      // ADDITIONAL ROTATIONS TO 
      const Geometry::Quaternion Qz=
	Geometry::Quaternion::calcQRotDeg(zAngle,bX);
      const Geometry::Quaternion Qxy=
	Geometry::Quaternion::calcQRotDeg(xyAngle,bZ);

      Qz.rotate(bY);
      Qz.rotate(bZ);
      Qxy.rotate(bX);
      Qxy.rotate(bY);
      Qxy.rotate(bZ);
    }

  return;
}

void
IMatBulkInsert::createSurfaces()
  /*!
    Create all the surfaces
  */
{
  ELog::RegMethod RegA("IMatBulkInsert","createSurface");

  ModelSupport::buildPlane(SMap,insIndex+3,
			   bEnter-bX*width/2.0,bX);
  ModelSupport::buildPlane(SMap,insIndex+4,
			   bEnter+bX*width/2.0,bX);
  ModelSupport::buildPlane(SMap,insIndex+5,
			   bEnter-bZ*height/2.0,bZ);
  ModelSupport::buildPlane(SMap,insIndex+6,
			   bEnter+bZ*height/2.0,bZ);
  return;
}

void 
IMatBulkInsert::createObjects(Simulation& System)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("IMatBulkInsert","createObjects");

  const std::string dSurf=divideStr();
  std::string Out;

  System.removeCell(innerVoid);
  System.removeCell(outerVoid);

  Out=ModelSupport::getComposite(SMap,surfIndex,insIndex,
				 "7 -17 3M -4M 5M -6M ")+dSurf;
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

  Out=ModelSupport::getComposite(SMap,surfIndex,insIndex,
				 "7 -17 3 -4 -5 6 "
                                 " (-3M : 4M : -5M : 6M) ")+dSurf;
  System.addCell(MonteCarlo::Qhull(cellIndex++,defMat,0.0,Out));

  // Outer void
  Out=ModelSupport::getComposite(SMap,surfIndex,insIndex,
				 "17 -27 3M -4M 5M -6M ")+dSurf;
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  Out=ModelSupport::getComposite(SMap,surfIndex,insIndex,
				 "17 -27 13 -14 -15 16 "
                                 " (-3M : 4M : -5M : 6M) ")+dSurf;
  System.addCell(MonteCarlo::Qhull(cellIndex++,defMat,0.0,Out));


  
  return;
}

void
IMatBulkInsert::createLinks() 
  /*!
    Create the links
  */
{
  ELog::RegMethod RegA("IMatBulkInsert","createLinks");

  FixedComp::setLinkSurf(2,SMap.realSurf(insIndex+3));
  FixedComp::setLinkSurf(3,SMap.realSurf(insIndex+4));
  FixedComp::setLinkSurf(4,SMap.realSurf(insIndex+5));
  FixedComp::setLinkSurf(5,SMap.realSurf(insIndex+6));
  
  return;
}

void
IMatBulkInsert::createAll(Simulation& System,
			const shutterSystem::GeneralShutter& GS)
  /*!
    Create the shutter
    \param System :: Simulation to process
    \param GS :: GeneralShutter object to use
   */
{
  ELog::RegMethod RegA("IMatBulkInsert","createAll");

  BulkInsert::createAll(System,GS);

  populate(System);
  createUnitVector();
  createSurfaces();
  createObjects(System);
  createLinks();
  return;
}
  
}  // NAMESPACE shutterSystem
