/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   essBuild/WedgeItem.cxx
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
#include "SimProcess.h"
#include "ModelSupport.h"
#include "generateSurf.h"
#include "chipDataStore.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "World.h"
#include "WedgeItem.h"

namespace essSystem
{

WedgeItem::WedgeItem(const int BN,const std::string& Key)  :
  attachSystem::ContainedComp(),attachSystem::FixedComp(Key,6),
  beamNumber(BN),
  wedgeIndex(ModelSupport::objectRegister::Instance().cell(Key,BN)),
  cellIndex(wedgeIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

WedgeItem::~WedgeItem() 
  /*!
    Destructor
  */
{}

void
WedgeItem::populate(const FuncDataBase& Control)
 /*!
   Populate all the variables
   \param Control :: FuncDataBase to use
 */
{
  ELog::RegMethod RegA("WedgeItem","populate");
  
  xStep=SimProcess::getIndexVar<double>
    (Control,keyName,"XStep",beamNumber);
  yStep=SimProcess::getIndexVar<double>
    (Control,keyName,"YStep",beamNumber);
  zStep=SimProcess::getIndexVar<double>
    (Control,keyName,"ZStep",beamNumber);
  xyAngle=SimProcess::getIndexVar<double>
    (Control,keyName,"XYangle",beamNumber);
  zAngle=SimProcess::getIndexVar<double>
    (Control,keyName,"Zangle",beamNumber);

  nLayer=SimProcess::getIndexVar<size_t>
    (Control,keyName,"NLayer",beamNumber);

  if (!nLayer) return;
  
  for(size_t i=0;i<nLayer;i++)
    {
      const std::string AStr=StrFunc::makeString(i);
      const std::string BStr=StrFunc::makeString(i+1);
      if (i) 
	radius.push_back
	  (SimProcess::getIndexVar<double>
	   (Control,keyName,"Radius"+AStr,beamNumber));

      width.push_back
	(SimProcess::getIndexVar<double>
	 (Control,keyName,"Width"+BStr,beamNumber));

      height.push_back
	(SimProcess::getIndexVar<double>
	 (Control,keyName,"Height"+BStr,beamNumber));
    }
  return;
}
  
void
WedgeItem::createUnitVector(const attachSystem::FixedComp& FC)
  /*!
    Create the unit vectors
    \param FC :: Linked object
  */
{
  ELog::RegMethod RegA("WedgeItem","createUnitVector");

  FixedComp::createUnitVector(FC);
  applyShift(xStep,yStep,zStep);
  applyAngleRotate(xyAngle,zAngle);

  return;
}
  
void
WedgeItem::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("WedgeItem","createSurface");

  // rotation of axis:

  int RI(wedgeIndex);
  ModelSupport::buildPlane(SMap,wedgeIndex,Origin,Y);  
  for(size_t i=0;i<nLayer;i++)
    {
      if (i)
	ModelSupport::buildCylinder(SMap,RI+7,Origin,Z,radius[i-1]);
      ModelSupport::buildPlane(SMap,RI+3,Origin-X*(width[i]/2.0),X);
      ModelSupport::buildPlane(SMap,RI+4,Origin+X*(width[i]/2.0),X);
      ModelSupport::buildPlane(SMap,RI+5,Origin-Z*(height[i]/2.0),Z);
      ModelSupport::buildPlane(SMap,RI+6,Origin+Z*(height[i]/2.0),Z);
      RI+=10;
    }

  return;
}

void
WedgeItem::createObjects(Simulation& System,
			 const attachSystem::FixedComp& FC,
			 const size_t innerIndex,
			 const size_t outerIndex)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
    \param FC :: Main body 
    \param innerIndex :: Index of inner surf [typically 0]
    \param outerIndex :: Index of outer surf [typically 1]
  */
{
  ELog::RegMethod RegA("WedgeItem","createObjects");

  std::string Out;
  int RI(wedgeIndex);
  for(size_t i=0;i<nLayer;i++)
    {
      Out=ModelSupport::getComposite(SMap,RI,wedgeIndex,"1M 3 -4 5 -6 ");
      Out+=(i) ? 
	ModelSupport::getComposite(SMap,RI," 7 ") :
	FC.getLinkString(innerIndex);

      Out+=(i!=nLayer-1) ? 
	ModelSupport::getComposite(SMap,RI+10," -7 ") :
	FC.getLinkString(outerIndex);
      
      System.addCell(MonteCarlo::Qhull(cellIndex++,mat,0.0,Out));
      RI+=10;
    }
  
  addOuterSurf(Out);
  return;
}

void
WedgeItem::createLinks()
  /*!
    Create all the linkes [OutGoing]
  */
{
  ELog::RegMethod RegA("WedgeItem","createLinks");

  if (nLayer>1)
    {
      size_t index(0);
      for(size_t j=0;j<2;j++)
        {
	  const size_t i(nLayer-(j+1));
	  
	  FixedComp::setConnect(index,Origin-Z*height[i]/2.0,-Z);  // base
	  FixedComp::setConnect(index+1,Origin+Z*height[i]/2.0,Z);  // base
	  FixedComp::setConnect(index+2,Origin+Y*radius[i],Y);   // outer point
	  
	  const int RI(static_cast<int>(i)*10+wedgeIndex);
	  FixedComp::setLinkSurf(index,-SMap.realSurf(RI+5));
	  FixedComp::setLinkSurf(index+1,SMap.realSurf(RI+6));
	  FixedComp::setLinkSurf(index+2,SMap.realSurf(RI+7));
	  index+=3;
	}
    }
  return;
}


void
WedgeItem::createAll(Simulation& System,
		     const attachSystem::FixedComp& FC,
		     const size_t innerSide, 
		     const size_t outerSide)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param innerSide :: Inner link surface 
    \param outerSide :: Outer link surface 
  */
{
  ELog::RegMethod RegA("WedgeItem","createAll");
  

  populate(System.getDataBase());
  createUnitVector(FC);
  createSurfaces();
  createLinks();
  createObjects(System,FC,innerSide,outerSide);
  insertObjects(System);              

  return;
}

}  // NAMESPACE ts1System
