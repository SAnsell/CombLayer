/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   epbBuild/EPBline.cxx
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
#include <boost/shared_ptr.hpp>
#include <boost/array.hpp>
#include <boost/multi_array.hpp>

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
#include "SimProcess.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "LinearComp.h"
#include "ContainedComp.h"
#include "channel.h"
#include "boxValues.h"
#include "boxUnit.h"
#include "BoxLine.h"
#include "EPBline.h"

namespace epbSystem
{

EPBline::EPBline(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedComp(Key,10),
  epbIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(epbIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

EPBline::EPBline(const EPBline& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedComp(A),
  epbIndex(A.epbIndex),cellIndex(A.cellIndex),xStep(A.xStep),
  yStep(A.yStep),zStep(A.zStep),xyAngle(A.xyAngle),
  zAngle(A.zAngle),innerRad(A.innerRad),wallThick(A.wallThick),
  wallMat(A.wallMat),nSeg(A.nSeg),Pts(A.Pts),YVec(A.YVec),
  Cent(A.Cent)
  /*!
    Copy constructor
    \param A :: EPBline to copy
  */
{}

EPBline&
EPBline::operator=(const EPBline& A)
  /*!
    Assignment operator
    \param A :: EPBline to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedComp::operator=(A);
      cellIndex=A.cellIndex;
      xStep=A.xStep;
      yStep=A.yStep;
      zStep=A.zStep;
      xyAngle=A.xyAngle;
      zAngle=A.zAngle;
      innerRad=A.innerRad;
      wallThick=A.wallThick;
      wallMat=A.wallMat;
      nSeg=A.nSeg;
      Pts=A.Pts;
      YVec=A.YVec;
      Cent=A.Cent;
    }
  return *this;
}


EPBline::~EPBline() 
 /*!
   Destructor
 */
{}

void
EPBline::populate(const Simulation& System)
 /*!
   Populate all the variables
   \param System :: Simulation to use
 */
{
  ELog::RegMethod RegA("EPBline","populate");  
  const FuncDataBase& Control=System.getDataBase();


  xStep=Control.EvalVar<double>(keyName+"XStep");
  yStep=Control.EvalVar<double>(keyName+"YStep");
  zStep=Control.EvalVar<double>(keyName+"ZStep");
  xyAngle=Control.EvalVar<double>(keyName+"XYangle");
  zAngle=Control.EvalVar<double>(keyName+"Zangle");

  innerRad=Control.EvalVar<double>(keyName+"InnerRad");
  wallThick=Control.EvalVar<double>(keyName+"WallThick");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");

  nSeg=Control.EvalVar<size_t>(keyName+"NSeg")+1;
  Geometry::Vec3D PT;
  for(size_t i=0;i<nSeg;i++)
    {
      PT=Control.EvalVar<Geometry::Vec3D>(keyName+
					  StrFunc::makeString("Point",i+1));
      Pts.push_back(PT);
      if (i)
	YVec.push_back(PT-Pts[i-1]);
      else
	YVec.push_back(Y);
    }
  return;
}
  
void
EPBline::createUnitVector(const attachSystem::FixedComp& FC)
  /*!
    Create the unit vectors
    - Y Down the beamline
    \param FC :: FixedComp for origin and axis
  */
{
  ELog::RegMethod RegA("EPBline","createUnitVector");

  attachSystem::FixedComp::createUnitVector(FC);
  applyShift(xStep,yStep,zStep);
  applyAngleRotate(xyAngle,zAngle);

  for(size_t i=0;i<nSeg;i++)
    {
      Pts[i]=X*Pts[i].X()+Y*Pts[i].Y()+Z*Pts[i].Z();
      YVec[i]=X*YVec[i].X()+Y*YVec[i].Y()+Z*YVec[i].Z();
      //      Cent[i]=X*Cent[i].X()+Y*Cent[i].Y()+Z*Cent[i].Z();
      YVec[i].makeUnit();
    }
      
  return;
}



void
EPBline::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("EPBline","createSurface");

  int PI(epbIndex);
  for(size_t i=0;i<nSeg;i++)
    {
      ModelSupport::buildPlane(SMap,PI+1,Pts[i],YVec[i]);        
      PI+=100;
    }
  ModelSupport::buildPlane(SMap,epbIndex+3,Pts[0]-X*innerRad,X);        
  ModelSupport::buildPlane(SMap,epbIndex+4,Pts[0]+X*innerRad,X);        
  ModelSupport::buildPlane(SMap,epbIndex+13,
			   Pts[0]-X*(innerRad+wallThick),X);        
  ModelSupport::buildPlane(SMap,epbIndex+14,
			   Pts[0]+X*(innerRad+wallThick),X);        

  PI=epbIndex;
  for(size_t i=1;i<nSeg;i++)
    {
      const Geometry::Vec3D ZV=X*YVec[i];
      ModelSupport::buildPlane(SMap,PI+5,Pts[i]-
			       ZV*innerRad,ZV);        
      ModelSupport::buildPlane(SMap,PI+6,Pts[i]+
			       ZV*innerRad,ZV);        
      ModelSupport::buildPlane(SMap,PI+15,Pts[i]-
			       ZV*(innerRad+wallThick),ZV);        
      ModelSupport::buildPlane(SMap,PI+16,Pts[i]+
			       ZV*(innerRad+wallThick),ZV);        
      PI+=100;
    }

  return;
}

void
EPBline::createObjects(Simulation& System)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("EPBline","createObjects");
  
  std::string Out;

  int PI(epbIndex);
  for(size_t i=1;i<nSeg;i++)
    {
      Out=ModelSupport::getComposite(SMap,PI,epbIndex, "1 -101 3M -4M 5 -6");
      System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
      Out=ModelSupport::getComposite(SMap,PI,epbIndex, 
				     "1 -101 13M -14M 15 -16 (-3M:4M:-5:6)");
      System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));
      Out=ModelSupport::getComposite(SMap,PI,epbIndex, "1 -101 13M -14M 15 -16");
      addOuterUnionSurf(Out);
      PI+=100;
    }

  return;
}

void
EPBline::createLinks()
  /*!
    Creates a full attachment set [Internal]
  */
{

  FixedComp::setConnect(0,Origin,-Y);     
  FixedComp::setLinkSurf(0,-SMap.realSurf(epbIndex+1));

  std::string Out;
  int PI(epbIndex);
  for(size_t i=1;i<nSeg;i++)
    {
      FixedComp::setConnect(1+i,Pts[i],YVec[i]);     
      Out=ModelSupport::getComposite(SMap,PI,epbIndex,
				     "(-13M:14M:-15:16:-1:101)");
      FixedComp::addLinkSurf(1+i,Out);
      // FixedComp::setLinkSurf(1+i,-SMap.realSurf(epbIndex+13));
      // FixedComp::addLinkSurf(1+i,SMap.realSurf(epbIndex+14));
      // FixedComp::addLinkSurf(1+i,-SMap.realSurf(PI+5));
      // FixedComp::addLinkSurf(1+i,SMap.realSurf(PI+6));
      // FixedComp::addLinkSurf(1+i,-SMap.realSurf(PI+1));
      // FixedComp::addLinkSurf(1+i,SMap.realSurf(PI+101));
      PI+=100;
    }
  FixedComp::setConnect(1,Pts.back(),YVec.back());     
  FixedComp::setLinkSurf(1,SMap.realSurf(PI+1));

  return;
}

void
EPBline::createAll(Simulation& System,
		   const attachSystem::FixedComp& FC)

  /*!
    Global creation of the hutch
    \param System :: Simulation to add vessel to
    \param FC :: FixedComp for origin
  */
{
  ELog::RegMethod RegA("EPBline","createAll");
  populate(System);
  createUnitVector(FC);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       

  return;
}

  
}  // NAMESPACE epbSystem
