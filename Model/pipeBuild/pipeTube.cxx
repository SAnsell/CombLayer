/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   pipeBuild/pipeTube.cxx
 *
 * Copyright (c) 2004-2016 by Stuart Ansell
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
#include "surfRegister.h"
#include "objectRegister.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfDIter.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfEqual.h"
#include "surfDivide.h"
#include "Quadratic.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "support.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "LayerComp.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "surfDBase.h"
#include "mergeTemplate.h"
#include "pipeTube.h"

namespace pipeSystem
{

pipeTube::pipeTube(const std::string& Key) :
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(Key,6)
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{}

pipeTube::pipeTube(const pipeTube& A) : 
  attachSystem::ContainedComp(A),
  attachSystem::FixedOffset(A),attachSystem::CellMap(A),
  length(A.length),height(A.height),width(A.width),
  innerHeight(A.innerHeight),innerWidth(A.innerWidth),
  wallMat(A.wallMat),nWallLayers(A.nWallLayers),
  wallFracList(A.wallFracList),wallMatList(A.wallMatList)
  /*!
    Copy constructor
    \param A :: pipeTube to copy
  */
{}

pipeTube&
pipeTube::operator=(const pipeTube& A)
  /*!
    Assignment operator
    \param A :: pipeTube to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      attachSystem::CellMap::operator=(A);
      length=A.length;
      height=A.height;
      width=A.width;
      innerHeight=A.innerHeight;
      innerWidth=A.innerWidth;
      wallMat=A.wallMat;
      nWallLayers=A.nWallLayers;
      wallFracList=A.wallFracList;
      wallMatList=A.wallMatList;
    }
  return *this;
}

pipeTube::~pipeTube()
  /*!
    Destructor
  */
{}
  

void
pipeTube::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("pipeTube","populate");

  FixedOffset::populate(Control);

  length=Control.EvalVar<double>(keyName+"Length");
  height=Control.EvalVar<double>(keyName+"Height");
  width=Control.EvalVar<double>(keyName+"Width");
  innerHeight=Control.EvalVar<double>(keyName+"InnerHeight");
  innerWidth=Control.EvalVar<double>(keyName+"InnerWidth");

  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");

  nWallLayers=Control.EvalVar<size_t>(keyName+"NWallLayers");
  ModelSupport::populateDivide(Control,nWallLayers,
			       keyName+"WLayerMat",
			       wallMat,wallMatList);
  ModelSupport::populateDivideLen(Control,nWallLayers,
				  keyName+"WLayerThick",
				  (height-innerHeight)/2.0,
				  wallFracList);

  return;
}

void
pipeTube::createUnitVector(const attachSystem::FixedComp& FC,
			   const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed Component
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("pipeTube","createUnitVector");
  attachSystem::FixedComp::createUnitVector(FC,sideIndex);
  yStep+=length/2.0;
  FixedOffset::applyOffset();

  return;
}

void
pipeTube::createSurfaces()
  /*!
    Create planes for system
  */
{
  ELog::RegMethod RegA("pipeTube","createSurfaces");

  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length/2.0),Y);  
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(width/2.0),X);  
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(height/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(height/2.0),Z);  

  ModelSupport::buildPlane(SMap,buildIndex+13,Origin-X*(innerWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+14,Origin+X*(innerWidth/2.0),X);  
  ModelSupport::buildPlane(SMap,buildIndex+15,Origin-Z*(innerHeight/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,Origin+Z*(innerHeight/2.0),Z);  

  return; 
}

void
pipeTube::createObjects(Simulation& System)
  /*!
    Create the vaned moderator
    \param System :: Simulation to add results
  */
{
  ELog::RegMethod RegA("pipeTube","createObjects");

  std::string Out;

  // Inner 
  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -2 13 -14 15 -16 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

  CellMap::setCell("Inner",cellIndex-1);
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 " 1 -2 3 -4 5 -6 (-13:14:-15:16) ");
  
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));
  CellMap::setCell("Outer",cellIndex-1);
  
  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -2 3 -4 5 -6 ");
  addOuterSurf(Out);
  return; 
}

void
pipeTube::createLinks()
  /*!
    Creates a full attachment set
    First two are in the -/+Y direction and have a divider
    Last two are in the -/+X direction and have a divider
    The mid two are -/+Z direction
  */
{  
  ELog::RegMethod RegA("pipeTube","createLinks");

  FixedComp::setConnect(0,Origin-Y*(length/2.0),-Y);
  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+1));

  FixedComp::setConnect(1,Origin+Y*(length/2.0),Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+2));
  
  FixedComp::setConnect(2,Origin-X*(length/2.0),-X);
  FixedComp::setLinkSurf(2,-SMap.realSurf(buildIndex+3));
  
  FixedComp::setConnect(3,Origin+X*(width/2.0),X);
  FixedComp::setLinkSurf(3,-SMap.realSurf(buildIndex+4));
  
  FixedComp::setConnect(4,Origin-Z*(height/2.0),-Z);
  FixedComp::setLinkSurf(4,-SMap.realSurf(buildIndex+5));
  
  FixedComp::setConnect(5,Origin+Z*(height/2.0),Z);
  FixedComp::setLinkSurf(5,-SMap.realSurf(buildIndex+6));

  return;
}

void 
pipeTube::layerProcess(Simulation& System)
  /*!
    Processes the splitting of the surfaces into a multilayer system
    \param System :: Simulation to work on
  */
{
  ELog::RegMethod RegA("pipeTube","layerProcess");
  // Steel layers
  //  layerSpecial(System);

  if (nWallLayers>1)
    {
      std::string OutA,OutB;
      ModelSupport::surfDivide DA;
            
      for(size_t i=1;i<nWallLayers;i++)
	{
	  DA.addFrac(wallFracList[i-1]);
	  DA.addMaterial(wallMatList[i-1]);
	}
      DA.addMaterial(wallMatList.back());
      // Cell Specific:
      DA.setCellN(CellMap::getCell("Outer"));
      DA.setOutNum(cellIndex,buildIndex+1001);

      ModelSupport::mergeTemplate<Geometry::Plane,
				  Geometry::Plane> surroundRule;

      surroundRule.setSurfPair(SMap.realSurf(buildIndex+13),
			       SMap.realSurf(buildIndex+3));
      surroundRule.setSurfPair(SMap.realSurf(buildIndex+14),
			       SMap.realSurf(buildIndex+4));
      surroundRule.setSurfPair(SMap.realSurf(buildIndex+15),
			       SMap.realSurf(buildIndex+5));
      surroundRule.setSurfPair(SMap.realSurf(buildIndex+16),
			       SMap.realSurf(buildIndex+6));
      OutA=ModelSupport::getComposite(SMap,buildIndex," (-13:14:-15:16) ");
      OutB=ModelSupport::getComposite(SMap,buildIndex," 3 -4 5 -6 ");

      surroundRule.setInnerRule(OutA);
      surroundRule.setOuterRule(OutB);
      DA.addRule(&surroundRule);
      DA.activeDivideTemplate(System);

      cellIndex=DA.getCellNum();
    }
  return;
}


  
void
pipeTube::createAll(Simulation& System,
		    const attachSystem::FixedComp& FC,
		    const long int sideIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: Attachment point
    \param sideIndex :: sideIndex for link point
   */
{
  ELog::RegMethod RegA("pipeTube","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);

  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);
  layerProcess(System);
  return;
}

}  // NAMESPACE pipeSystem
