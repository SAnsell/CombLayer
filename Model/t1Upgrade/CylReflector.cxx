/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   t1Upgrade/CylReflector.cxx
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
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "World.h"
#include "CylReflector.h"

namespace ts1System
{

CylReflector::CylReflector(const std::string& Key)  :
  attachSystem::ContainedComp(),attachSystem::FixedOffset(Key,10),
  attachSystem::CellMap(),
  refIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(refIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

CylReflector::CylReflector(const CylReflector& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedOffset(A),
  attachSystem::CellMap(A),
  refIndex(A.refIndex),cellIndex(A.cellIndex),
  nLayer(A.nLayer),radius(A.radius),
  height(A.height),depth(A.depth),COffset(A.COffset),
  Mat(A.Mat)
  /*!
    Copy constructor
    \param A :: CylReflector to copy
  */
{}

CylReflector&
CylReflector::operator=(const CylReflector& A)
  /*!
    Assignment operator
    \param A :: CylReflector to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      attachSystem::CellMap::operator=(A);
      cellIndex=A.cellIndex;
      nLayer=A.nLayer;
      radius=A.radius;
      height=A.height;
      depth=A.depth;
      COffset=A.COffset;
      Mat=A.Mat;
    }
  return *this;
}

CylReflector::~CylReflector() 
  /*!
    Destructor
  */
{}

void
CylReflector::populate(const FuncDataBase& Control)
 /*!
   Populate all the variables
   \param Control :: DataBase to used
 */
{
  ELog::RegMethod RegA("CylReflector","populate");
  
  attachSystem::FixedOffset::populate(Control);
 
  nLayer=Control.EvalVar<size_t>(keyName+"NLayer");
  if (!nLayer) return;
  radius.resize(nLayer);
  height.resize(nLayer);
  depth.resize(nLayer);
  Mat.resize(nLayer);
  COffset.resize(nLayer);
  for(size_t i=0;i<nLayer;i++)
    {
      radius[i]=Control.EvalVar<double>
	(keyName+StrFunc::makeString("Radius",i+1));
      height[i]=Control.EvalVar<double>
	(keyName+StrFunc::makeString("Height",i+1));
      depth[i]=Control.EvalVar<double>
	(keyName+StrFunc::makeString("Depth",i+1));
      Mat[i]=ModelSupport::EvalMat<int>(Control,
	keyName+StrFunc::makeString("Mat",i+1));
      if (i)
	COffset[i]=Control.EvalDefVar<Geometry::Vec3D>
	  (keyName+StrFunc::makeString("Offset",i+1),
	   Geometry::Vec3D(0,0,0));
    }
  return;
}
  
void
CylReflector::createUnitVector(const attachSystem::FixedComp& FC,
			       const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Linked object
    \param sideIndex :: link pint
  */
{
  ELog::RegMethod RegA("CylReflector","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
  
  for(size_t i=0;i<nLayer;i++)
    COffset[i]=X*COffset[i].X()+Y*COffset[i].Y();

  return;
}
  
void
CylReflector::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("CylReflector","createSurface");

  // rotation of axis:

  int RI(refIndex);
  for(size_t i=0;i<nLayer;i++)
    {
      ModelSupport::buildPlane(SMap,RI+5,Origin-Z*depth[i],Z);
      ModelSupport::buildPlane(SMap,RI+6,Origin+Z*height[i],Z);
      ModelSupport::buildCylinder(SMap,RI+7,Origin+COffset[i]
				  ,Z,radius[i]);
      RI+=10;
    }

  return;
}

void
CylReflector::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("CylReflector","createObjects");

  std::string Out,OutX;
  int RI(refIndex);
  for(size_t i=0;i<nLayer;i++)
    {
      Out=ModelSupport::getComposite(SMap,RI,"5 -6 -7 ");
      if (i)
	OutX=ModelSupport::getComposite(SMap,RI-10,"(-5:6:7)");
      System.addCell(MonteCarlo::Qhull(cellIndex++,Mat[i],0.0,Out+OutX));
      RI+=10;
      addCell("Reflector",cellIndex-1);
      addCell("RefLayer"+std::to_string(i),cellIndex-1);
    }
  
  addOuterSurf(Out);
  return;
}

std::string
CylReflector::getComposite(const std::string& surfList) const
  /*!
    Exposes local version of getComposite
    \param surfList :: surface list
    \return Composite string
  */
{
  return ModelSupport::getComposite(SMap,refIndex,surfList);
}

void
CylReflector::createLinks()
  /*!
    Create all the linkes [OutGoing]
  */
{
  ELog::RegMethod RegA("Reflector","createLinks");

  if (nLayer)
    {
      const size_t i(nLayer-1);
      FixedComp::setConnect(0,Origin+COffset[i]-Z*depth[i],-Z);  // base
      FixedComp::setConnect(1,Origin+COffset[i]+Z*height[i],Z);  // base
      FixedComp::setConnect(2,Origin+COffset[i]+Y*radius[i],Y);   // outer point
      
      const int RI(static_cast<int>(nLayer-1)*10+refIndex);
      FixedComp::setLinkSurf(0,-SMap.realSurf(RI+5));
      FixedComp::setLinkSurf(1,SMap.realSurf(RI+6));
      FixedComp::setLinkSurf(2,SMap.realSurf(RI+7));
    }
  return;
}

std::vector<int>
CylReflector::getCells() const
  /*!
    Return all the main reflector cells
    \return Vector of cells
  */
{
  std::vector<int> Out;
  for(int i=refIndex+1;i<cellIndex;i++)
    Out.push_back(i);
  return Out;
}

void
CylReflector::createAll(Simulation& System,
			const attachSystem::FixedComp& FC,
			const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("CylReflector","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createLinks();
  createObjects(System);
  insertObjects(System);              

  return;
}

}  // NAMESPACE ts1System
