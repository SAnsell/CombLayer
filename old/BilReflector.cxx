/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   old/BilReflector.cxx
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Triple.h"
#include "NRange.h"
#include "NList.h"
#include "Tally.h"
#include "Quaternion.h"
#include "localRotate.h"
#include "masterRotate.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfEqual.h"
#include "surfDivide.h"
#include "surfDIter.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "inputParam.h"
#include "ReadFunctions.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "KGroup.h"
#include "Source.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "generateSurf.h"
#include "chipDataStore.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "TargetBase.h"
#include "Groove.h"
#include "Hydrogen.h"
#include "OrthoInsert.h"
#include "VacVessel.h"
#include "FlightLine.h"
#include "World.h"
#include "BilReflector.h"

namespace bibSystem
{

BilReflector::BilReflector(const std::string& Key)  :
  attachSystem::ContainedComp(),attachSystem::FixedComp(Key,10),
  refIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(refIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}


BilReflector::~BilReflector() 
  /*!
    Destructor
  */
{}

void
BilReflector::populate(const Simulation& System)
 /*!
   Populate all the variables
   \param System :: Simulation to use
 */
{
  ELog::RegMethod RegA("BilReflector","populate");
  
  const FuncDataBase& Control=System.getDataBase();
  
  xStep=Control.EvalVar<double>(keyName+"XStep");
  yStep=Control.EvalVar<double>(keyName+"YStep");
  zStep=Control.EvalVar<double>(keyName+"ZStep");
  xyAngle=Control.EvalVar<double>(keyName+"XYAngle");
  zAngle=Control.EvalVar<double>(keyName+"ZAngle");

  // Depth is singular
  depth=Control.EvalVar<double>(keyName+"Depth");
  baseDepth=Control.EvalVar<double>(keyName+"BaseDepth");

  nLayer=Control.EvalVar<size_t>(keyName+"NLayer");
  if (!nLayer) return;
  radius.resize(nLayer);
  height.resize(nLayer);
  Mat.resize(nLayer);
  COffset.resize(nLayer);
  for(size_t i=0;i<nLayer;i++)
    {
      radius[i]=Control.EvalVar<double>
	(keyName+StrFunc::makeString("Radius",i+1));
      height[i]=Control.EvalVar<double>
	(keyName+StrFunc::makeString("Height",i+1));
      Mat[i]=Control.EvalVar<int>
	(keyName+StrFunc::makeString("Mat",i+1));
      if (i)
	COffset[i]=Control.EvalDefVar<Geometry::Vec3D>
	  (keyName+StrFunc::makeString("Offset",i+1),
	   Geometry::Vec3D(0,0,0));
    }
  return;
}
  
void
BilReflector::createUnitVector(const attachSystem::FixedComp& FC)
  /*!
    Create the unit vectors
    \param FC :: Linked object
  */
{
  ELog::RegMethod RegA("BilReflector","createUnitVector");

  FixedComp::createUnitVector(FC);
  applyShift(xStep,yStep,zStep);
  applyAngleRotate(xyAngle,zAngle);
    
  for(size_t i=0;i<nLayer;i++)
    COffset[i]=X*COffset[i].X()+Y*COffset[i].Y();

  return;
}
  
void
BilReflector::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("BilReflector","createSurface");

  // rotation of axis:

  ModelSupport::buildPlane(SMap,refIndex+5,Origin-Z*depth,Z);
  ModelSupport::buildPlane(SMap,refIndex+15,Origin-Z*(baseDepth+depth),Z);

  int RI(refIndex);
  for(size_t i=0;i<nLayer;i++)
    {
      ModelSupport::buildPlane(SMap,RI+6,Origin+Z*height[i],Z);
      ModelSupport::buildCylinder(SMap,RI+7,Origin+COffset[i]
				  ,Z,radius[i]);
      RI+=10;
    }

  return;
}

void
BilReflector::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("BilReflector","createObjects");

  std::string Out,OutX;
  int RI(refIndex);
  for(size_t i=0;i<nLayer;i++)
    {
      Out=ModelSupport::getComposite(SMap,RI,refIndex,"5M -6 -7 ");
      if (i)
	OutX=ModelSupport::getComposite(SMap,RI-10,"(6:7)");
      System.addCell(MonteCarlo::Qhull(cellIndex++,Mat[i],0.0,Out+OutX));
      RI+=10;
    }
  // Base plate:
  
  addOuterSurf(Out);
  return;
}

std::string
BilReflector::getComposite(const std::string& surfList) const
  /*!
    Exposes local version of getComposite
    \param surfList :: surface list
    \return Composite string
  */
{
  return ModelSupport::getComposite(SMap,refIndex,surfList);
}

void
BilReflector::addToInsertChain(attachSystem::ContainedComp& CC) const
  /*!
    Adds this object to the containedComp to be inserted.
    \param CC :: ContainedComp object to add to this
  */
{
  ELog::RegMethod RegA("BilReflector","addToInsertChain");

  for(int i=refIndex+1;i<cellIndex;i++)
    CC.addInsertCell(i);
    
  return;
}

void
BilReflector::addToInsertControl(Simulation& System,
				 const attachSystem::FixedComp& FC,
				 attachSystem::ContainedComp& CC) const
  /*!
    Adds this object to the containedComp to be inserted.
    FC is the fixed object that is to be inserted -- linkpoints
    must be set. It is tested against all the ojbect with
    this object .
    
    \todo This can easily be moved to FixedComp and out of this class
       -- it requies that cellNumber is checked in ObjectRegister
       -- That all objects can be effectively calculated [no open virtual
       holers]
       -- THAT IS IT!!!

    \param FC :: FixedComp with the points
    \param CC :: ContainedComp object to add to this
  */
{
  ELog::RegMethod RegA("BilReflector","addToInsertControl");

  const size_t NPoint=FC.NConnect();
  for(int i=refIndex+1;i<cellIndex;i++)
    {
      MonteCarlo::Qhull* CRPtr=System.findQhull(i);
      if (!CRPtr)
	throw ColErr::InContainerError<int>(i,"Object not build");

      CRPtr->populate();
      for(size_t j=0;j<NPoint;j++)
	{
	  const Geometry::Vec3D& Pt=FC.getLinkPt(j);
	  if (CRPtr->isValid(Pt))
	    {
	      CC.addInsertCell(i);
	      break;
	    }
	}
    }
  CC.insertObjects(System);
  return;
}

void
BilReflector::createLinks()
  /*!
    Create all the linkes [OutGoing]
  */
{
  ELog::RegMethod RegA("Reflector","createLinks");

  FixedComp::setConnect(0,Origin-Z*(depth+baseDepth),-Z);  // base
  FixedComp::setLinkSurf(0,-SMap.realSurf(refIndex+15));

  if (nLayer)
    {
      const size_t i(nLayer-1);
      FixedComp::setConnect(1,Origin+COffset[i]+Z*height[i],Z);  // base
      FixedComp::setConnect(2,Origin+COffset[i]+Y*radius[i],Y);   // outer point
      
      const int RI(static_cast<int>(nLayer-1)*10+refIndex);
      FixedComp::setLinkSurf(1,SMap.realSurf(RI+6));
      FixedComp::setLinkSurf(2,SMap.realSurf(RI+7));
    }
  return;
}

std::vector<int>
BilReflector::getCells() const
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
BilReflector::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
  */
{
  ELog::RegMethod RegA("BilReflector","createAll");

  populate(System);
  createUnitVector(FC);
  createSurfaces();
  createLinks();
  createObjects(System);
  insertObjects(System);              

  return;
}

}  // NAMESPACE ts1System
