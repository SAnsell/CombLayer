/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   muon/cShieldLayer.cxx
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
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Tensor.h"
#include "Vec3D.h"
#include "PointOperation.h"
#include "Triple.h"
#include "NRange.h"
#include "NList.h"
#include "Quaternion.h"
#include "localRotate.h"
#include "masterRotate.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfDIter.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfEqual.h"
#include "surfDivide.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "LineIntersectVisit.h"
#include "Rules.h"
#include "Convex.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "KGroup.h"
#include "SimProcess.h"
#include "SurInter.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "generateSurf.h"
#include "ContainedComp.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "World.h"
#include "cShieldLayer.h"

namespace muSystem
{

cShieldLayer::cShieldLayer(const std::string& Key)  : 
  attachSystem::FixedComp(Key,6),attachSystem::ContainedComp(),
  csLayerIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(csLayerIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Key to use
  */
{}


cShieldLayer::~cShieldLayer() 
  /*!
    Destructor
  */
{}

void
cShieldLayer::populate(const Simulation& System)
  /*!
    Populate all the variables
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("cShieldLayer","populate");

  const FuncDataBase& Control=System.getDataBase();

  xStep=Control.EvalVar<double>(keyName+"XStep");
  yStep=Control.EvalVar<double>(keyName+"YStep");
  zStep=Control.EvalVar<double>(keyName+"ZStep");
  xyAngle=Control.EvalVar<double>(keyName+"XYAngle");

  height=Control.EvalVar<double>(keyName+"Height");
  depth=Control.EvalVar<double>(keyName+"Depth");
  width=Control.EvalVar<double>(keyName+"Width");
  
  steelMat=Control.EvalVar<int>(keyName+"SteelMat");    

  nLay=Control.EvalVar<int>(keyName+"NLayers");
       
  return;
}

void
cShieldLayer::createUnitVector()
  /*!
    Create the unit vectors
  */
{
  ELog::RegMethod RegA("cShieldLayer","createUnitVector");

  attachSystem::FixedComp::createUnitVector(World::masterOrigin());
  applyShift(xStep,yStep,zStep);
  applyAngleRotate(xyAngle,0);    


  
  return;
}

void
cShieldLayer::createSurfaces()
  /*!
    Create all the surfaces
  */
{
  ELog::RegMethod RegA("cShieldLayer","createSurface");

  if (nLay>0)
    {
      const double layThick(depth/nLay);
      double yPos(-depth/2.0);
      int plateIndex(csLayerIndex+100);
      for(int i=0;i<nLay-1;i++)
	{
	  yPos+=layThick;
	  ModelSupport::buildPlane(SMap,plateIndex+1,Origin+Y*yPos,Y);
//	  yPos+=layThick;
//	  ModelSupport::buildPlane(SMap,plateIndex+2,Origin+Y*yPos,Y);
	  plateIndex+=10;
	}
    }


  // outer layer
  ModelSupport::buildPlane(SMap,csLayerIndex+1,Origin-Y*depth/2.0,Y);
  ModelSupport::buildPlane(SMap,csLayerIndex+2,Origin+Y*depth/2.0,Y);
  ModelSupport::buildPlane(SMap,csLayerIndex+3,Origin-X*width/2.0,X);
  ModelSupport::buildPlane(SMap,csLayerIndex+4,Origin+X*width/2.0,X);
  ModelSupport::buildPlane(SMap,csLayerIndex+5,Origin-Z*height/2.0,Z);
  ModelSupport::buildPlane(SMap,csLayerIndex+6,Origin+Z*height/2.0,Z);
  


  return;
}

void
cShieldLayer::addToInsertChain(attachSystem::ContainedComp& CC) const
  /*!
    Adds this object to the containedComp to be inserted.
    \param CC :: ContainedComp object to add to this
  */
{
  for(int i=csLayerIndex+1;i<cellIndex;i++)
    CC.addInsertCell(i);
  return;
}

void
cShieldLayer::createObjects(Simulation& System)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("cShieldLayer","createObjects");
  
  std::string Out;
  std::string Out1;

    // Steel
  Out=ModelSupport::getComposite(SMap,csLayerIndex,"1 -2 3 -4 5 -6 ");
  addOuterSurf(Out);
  addBoundarySurf(Out);

  if (nLay>0)
    {
      int plateIndex(csLayerIndex+100);
      // inital
      Out=ModelSupport::getComposite(SMap,csLayerIndex,"1 -101 3 -4 5 -6");
      System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,0.0,Out));
      for(int i=0;i<nLay-2;i++)
	{
	  Out=ModelSupport::getComposite(SMap,csLayerIndex,
					 plateIndex,"1M -11M 3 -4 5 -6");					 
	  System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,0.0,Out));
//	  if (i=nLay-5)
//	    {
//	  Out=ModelSupport::getComposite(SMap,csLayerIndex,
//					 plateIndex,"1M -11M 3 -4 5 -6");					 
//	  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
//	    }
	  plateIndex+=10;
	}
      // final
      Out=ModelSupport::getComposite(SMap,csLayerIndex,plateIndex-10,
				     "11M -2 3 -4 5 -6");
      System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,0.0,Out));
    }
  else
    {
      Out=ModelSupport::getComposite(SMap,csLayerIndex,"1 -2 3 -4 5 -6");
      System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,0.0,Out));
    }
  return;
}


void
cShieldLayer::createLinks()
  /*!
    Create links
   */
{
  ELog::RegMethod RegA("cShieldLayer","createLinks");

  FixedComp::setLinkSurf(0,-SMap.realSurf(csLayerIndex+1));
  FixedComp::setLinkSurf(1,SMap.realSurf(csLayerIndex+2));
  FixedComp::setLinkSurf(2,-SMap.realSurf(csLayerIndex+3));
  FixedComp::setLinkSurf(3,SMap.realSurf(csLayerIndex+4));
  FixedComp::setLinkSurf(4,-SMap.realSurf(csLayerIndex+5));
  FixedComp::setLinkSurf(5,SMap.realSurf(csLayerIndex+6));

  return;
}

void
cShieldLayer::createAll(Simulation& System)

  /*!
    Global creation of the hutch
    \param System :: Simulation to add vessel to
    \param FC :: Fixed Component to place object within
  */
{
  ELog::RegMethod RegA("cShieldLayer","createAll");
  populate(System);
  createUnitVector();
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);
  return;
}
  
}  // NAMESPACE shutterSystem
