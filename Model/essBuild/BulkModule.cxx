/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/BulkModule.cxx
 *
 * Copyright (c) 2004-2025 by Stuart Ansell
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
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "BulkModule.h"

namespace essSystem
{

BulkModule::BulkModule(const std::string& Key)  :
  attachSystem::FixedRotate(Key,9),
  attachSystem::ContainedComp(),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  attachSystem::ExternalCut()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

BulkModule::BulkModule(const BulkModule& A) : 
  attachSystem::FixedRotate(A),
  attachSystem::ContainedComp(A),
  attachSystem::CellMap(A),
  attachSystem::SurfMap(A),
  attachSystem::ExternalCut(A),
  nLayer(A.nLayer),radius(A.radius),height(A.height),depth(A.depth),
  COffset(A.COffset),Mat(A.Mat)
  /*!
    Copy constructor
    \param A :: BulkModule to copy
  */
{}

BulkModule&
BulkModule::operator=(const BulkModule& A)
  /*!
    Assignment operator
    \param A :: BulkModule to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedRotate::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      attachSystem::SurfMap::operator=(A);
      attachSystem::ExternalCut::operator=(A);
      nLayer=A.nLayer;
      radius=A.radius;
      height=A.height;
      depth=A.depth;
      COffset=A.COffset;
      Mat=A.Mat;
    }
  return *this;
}

BulkModule::~BulkModule() 
  /*!
    Destructor
  */
{}

void
BulkModule::populate(const FuncDataBase& Control)
 /*!
   Populate all the variables
   \param Control :: DataBase to use
 */
{
  ELog::RegMethod RegA("BulkModule","populate");
  
  FixedRotate::populate(Control);
  
  nLayer=Control.EvalVar<size_t>(keyName+"NLayer");
  if (!nLayer) return;
  radius.resize(nLayer);
  height.resize(nLayer);
  depth.resize(nLayer);
  Mat.resize(nLayer);
  COffset.resize(nLayer);
  for(size_t i=0;i<nLayer;i++)
    {
      const std::string nStr(std::to_string(i+1));
      radius[i]=Control.EvalVar<double>(keyName+"Radius"+nStr);
      height[i]=Control.EvalVar<double>(keyName+"Height"+nStr);
      depth[i]=Control.EvalVar<double>(keyName+"Depth"+nStr);      
      Mat[i]=ModelSupport::EvalMat<int>(Control,keyName+"Mat"+nStr);
      if (i)
	COffset[i]=Control.EvalDefVar<Geometry::Vec3D>
	  (keyName+"Offset"+nStr,Geometry::Vec3D(0,0,0));
    }
  return;
}
  
void
BulkModule::createUnitVector(const attachSystem::FixedComp& FC,
			     const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Linked object
    \param sideIndex :: Link point 
  */
{
  ELog::RegMethod RegA("BulkModule","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
    
  for(size_t i=0;i<nLayer;i++)
    COffset[i]=X*COffset[i].X()+Y*COffset[i].Y();

  return;
}
  
void
BulkModule::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("BulkModule","createSurface");

  // rotation of axis:

  // divider
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
  ModelSupport::buildPlane(SMap,buildIndex+100,Origin,Z);

  int RI(buildIndex);    
  for(size_t i=0;i<nLayer;i++)
    {
      ModelSupport::buildPlane(SMap,RI+5,Origin-Z*depth[i],Z);
      ModelSupport::buildPlane(SMap,RI+6,Origin+Z*height[i],Z);
      SurfMap::makeCylinder("Radius"+std::to_string(i),
			    SMap,RI+7,Origin+COffset[i]
			    ,Z,radius[i]);
      RI+=10;
    }

  return;
}

void
BulkModule::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
    \param CC :: Inner reflector
  */
{
  ELog::RegMethod RegA("BulkModule","createObjects");

  const HeadRule boundaryHR=
    ExternalCut::getRule("Reflector");

  HeadRule HR;

  HeadRule HRX=boundaryHR;
  int RI(buildIndex);
  size_t completeLayer(0);

  
  // create a split innner if base/top are active
  if (isActive("TargetBase") &&
      isActive("TargetTop") &&
      isActive("TargetRadius") &&
      isActive("ReflectorRadius"))
    {
      const HeadRule topHR=getRule("TargetTop");
      const HeadRule baseHR=getRule("TargetBase");
      const HeadRule radiusHR=getRule("TargetRadius");
      const HeadRule beHR=getRule("ReflectorRadius");
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"5 -7");
      makeCell("Layer0",System,cellIndex++,Mat[0],0.0,HR*HRX*baseHR);

      HR=ModelSupport::getHeadRule(SMap,buildIndex,"-6 -7");
      makeCell("Layer0",System,cellIndex++,Mat[0],0.0,HR*HRX*topHR);

      // inner section
      HR=HeadRule(SMap,buildIndex,-7);
      HR*=baseHR.complement()*topHR.complement()*radiusHR*beHR;
      makeCell("Layer0",System,cellIndex++,Mat[0],0.0,HR);
      
      
      HRX=ModelSupport::getHeadRule(SMap,buildIndex,"-5:6:7");
      completeLayer=1;
      RI+=10;
    }

  for(size_t i=completeLayer;i<nLayer;i++)
    {
      const std::string lName("Layer"+std::to_string(i));

      HR=ModelSupport::getHeadRule
	(SMap,RI,buildIndex,"5 -6 -7 -100M");
      makeCell(lName+"Base",System,cellIndex++,Mat[i],0.0,HR*HRX);
      HR=ModelSupport::getHeadRule
	(SMap,RI,buildIndex,"5 -6 -7 100M");
      makeCell(lName+"Top",System,cellIndex++,Mat[i],0.0,HR*HRX);
      HRX=ModelSupport::getHeadRule(SMap,RI,"-5:6:7");
      RI+=10;
    }
  HR=ModelSupport::getHeadRule(SMap,RI-10,"5 -6 -7");
  addOuterSurf(HR);
  return;
}


void
BulkModule::createLinks()
  /*!
    Create all the linkes [OutGoing]
  */
{
  ELog::RegMethod RegA("BulkModule","createLinks");

  if (nLayer>1)
    {
      size_t index(0);
      for(size_t j=0;j<2;j++)
        {
	  const size_t i(nLayer-(j+1));
	  
	  FixedComp::setConnect
	    (index,Origin+COffset[i]-Z*depth[i],-Z);  // base
	  FixedComp::setConnect
	    (index+1,Origin+COffset[i]+Z*height[i],Z);  // top
	  FixedComp::setConnect
	    (index+2,Origin+COffset[i]+Y*radius[i],Y);   // outer point
	  FixedComp::setConnect
	    (index+3,Origin+COffset[i]-Y*radius[i],-Y);   // outer point
	  
	  const int RI(static_cast<int>(i)*10+buildIndex);
	  FixedComp::setLinkSurf(index,-SMap.realSurf(RI+5));
	  FixedComp::setLinkSurf(index+1,SMap.realSurf(RI+6));
	  FixedComp::setLinkSurf(index+2,SMap.realSurf(RI+7));
	  FixedComp::setLinkSurf(index+3,SMap.realSurf(RI+7));
	  FixedComp::setBridgeSurf(index+3,-SMap.realSurf(buildIndex+1));

	  index+=4;
	}
      nameSideIndex(2,"OuterCyl");
    }
  return;
}

void 
BulkModule::addFlightUnit(Simulation& System,
			  const attachSystem::FixedComp& FC)
  /*!
    Adds a flight unit based on an existing flight line
    it is effectively an extension. This method only works
    with a >=2 layer system.
    \param System :: simulation
    \param FC :: Flight component [uses link surf 2-5] 
  */
{
  ELog::RegMethod RegA("BulkModule","addFlightUnit");


  HeadRule HR,HRlayer;

  for(long int index=3;index<=6;index++)
    HRlayer+=FC.getFullRule(index);

  // AVOID INNER
  for(int i=1;i<static_cast<int>(nLayer);i++)
    {
      MonteCarlo::Object* OPtr=System.findObject(buildIndex+i+1);
      if (!OPtr)
	throw ColErr::InContainerError<int>(buildIndex+i+1,"layerCells");
      OPtr->addIntersection(HRlayer);
    }

  HRlayer.makeComplement();

  HR=ModelSupport::getHeadRule(SMap,buildIndex,
			       buildIndex+10*static_cast<int>(nLayer-1),
			       "7 -7M");
  // Dividing surface ?
  System.addCell(cellIndex++,0,0.0,HR*HRlayer);
  return;
}

void
BulkModule::createAll(Simulation& System,
		      const attachSystem::FixedComp& FC,
		      const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex
  */
{
  ELog::RegMethod RegA("BulkModule","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createLinks();
  createObjects(System);
  insertObjects(System);              

  return;
}

}  // NAMESPACE essSystem
