/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   muon/cShieldLayer.cxx
 *
 * Copyright (c) 2004-2018 by Stuart Ansell/Goran Skoro
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
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Quadratic.h"
#include "Plane.h"
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
#include "ContainedComp.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "cShieldLayer.h"

namespace muSystem
{

cShieldLayer::cShieldLayer(const std::string& Key)  : 
  attachSystem::FixedOffset(Key,6),attachSystem::ContainedComp(),
  csLayerIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(csLayerIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Key to use
  */
{}

cShieldLayer::cShieldLayer(const cShieldLayer& A) : 
  attachSystem::FixedOffset(A),attachSystem::ContainedComp(A),
  csLayerIndex(A.csLayerIndex),cellIndex(A.cellIndex),
  height(A.height),depth(A.depth),width(A.width),
  steelMat(A.steelMat),nLay(A.nLay)
  /*!
    Copy constructor
    \param A :: cShieldLayer to copy
  */
{}

cShieldLayer&
cShieldLayer::operator=(const cShieldLayer& A)
  /*!
    Assignment operator
    \param A :: cShieldLayer to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedOffset::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      cellIndex=A.cellIndex;
      height=A.height;
      depth=A.depth;
      width=A.width;
      steelMat=A.steelMat;
      nLay=A.nLay;
    }
  return *this;
}

cShieldLayer::~cShieldLayer() 
  /*!
    Destructor
  */
{}

void
cShieldLayer::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Database to used
  */
{
  ELog::RegMethod RegA("cShieldLayer","populate");

  FixedOffset::populate(Control);
  height=Control.EvalVar<double>(keyName+"Height");
  depth=Control.EvalVar<double>(keyName+"Depth");
  width=Control.EvalVar<double>(keyName+"Width");
  
  steelMat=ModelSupport::EvalMat<int>(Control,keyName+"SteelMat");    

  nLay=Control.EvalVar<size_t>(keyName+"NLayers");
       
  return;
}

void
cShieldLayer::createUnitVector(const attachSystem::FixedComp& FC,
                               const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Object for origin
    \param sideIndex :: link point [signed]
  */
{
  ELog::RegMethod RegA("cShieldLayer","createUnitVector");

  attachSystem::FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
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
      const double layThick(depth/static_cast<double>(nLay));
      double yPos(-depth/2.0);
      int plateIndex(csLayerIndex+100);
      for(size_t i=0;i<nLay-1;i++)
	{
	  yPos+=layThick;
	  ModelSupport::buildPlane(SMap,plateIndex+1,Origin+Y*yPos,Y);
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

  if (nLay>1)
    {
      int plateIndex(csLayerIndex+100);
      // inital
      Out=ModelSupport::getComposite(SMap,csLayerIndex,"1 -101 3 -4 5 -6");
      System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,0.0,Out));
      for(size_t i=1;i<nLay-1;i++)
	{
	  Out=ModelSupport::getComposite(SMap,csLayerIndex,
					 plateIndex,"1M -11M 3 -4 5 -6");					 
	  System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,0.0,Out));
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
  
  FixedComp::setConnect(0,Origin-Y*depth/2.0,-Y);
  FixedComp::setConnect(1,Origin+Y*depth/2.0,Y);
  FixedComp::setConnect(2,Origin-X*width/2.0,-X);
  FixedComp::setConnect(3,Origin+X*width/2.0,X);
  FixedComp::setConnect(4,Origin-Z*height/2.0,-Z);
  FixedComp::setConnect(5,Origin+Z*height/2.0,Z);

  return;
}

void
cShieldLayer::createAll(Simulation& System,
			const attachSystem::FixedComp& FC,
                        const long int sideIndex)

  /*!
    Global creation of the hutch
    \param System :: Simulation to add vessel to
    \param FC :: Fixed Component to place object within
    \param sideIndex :: Link point/direction
  */
{
  ELog::RegMethod RegA("cShieldLayer","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);
  return;
}
  
}  // NAMESPACE muSystem
