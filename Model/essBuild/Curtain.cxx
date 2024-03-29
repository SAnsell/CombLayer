/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/Curtain.cxx
 *
 * Copyright (c) 2004-2023 by Stuart Ansell
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

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "Quaternion.h"
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
#include "FixedGroup.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "ExternalCut.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "surfDBase.h"
#include "surfDIter.h"
#include "surfDivide.h"
#include "mergeTemplate.h"

#include "Curtain.h"

namespace essSystem
{

Curtain::Curtain(const std::string& Key)  :
  attachSystem::FixedGroup(Key,"Top",6,"Mid",14,"Lower",16),
  attachSystem::ContainedGroup("Top","Mid","Lower","RoofCut"),
  attachSystem::CellMap(),
  attachSystem::SurfMap()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

Curtain::Curtain(const Curtain& A) : 
  attachSystem::FixedGroup(A),
  attachSystem::ContainedGroup(A),
  attachSystem::CellMap(A),
  attachSystem::SurfMap(A),
  wallRadius(A.wallRadius),leftPhase(A.leftPhase),
  rightPhase(A.rightPhase),innerStep(A.innerStep),
  wallThick(A.wallThick),baseGap(A.baseGap),
  outerGap(A.outerGap),topRaise(A.topRaise),depth(A.depth),
  height(A.height),nTopLayers(A.nTopLayers),
  nMidLayers(A.nMidLayers),nBaseLayers(A.nBaseLayers),
  topFrac(A.topFrac),midFrac(A.midFrac),baseFrac(A.baseFrac),
  topMat(A.topMat),midMat(A.midMat),baseMat(A.baseMat),
  wallMat(A.wallMat)
  /*!
    Copy constructor
    \param A :: Curtain to copy
  */
{}

Curtain&
Curtain::operator=(const Curtain& A)
  /*!
    Assignment operator
    \param A :: Curtain to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedGroup::operator=(A);
      attachSystem::ContainedGroup::operator=(A);
      attachSystem::CellMap::operator=(A);
      attachSystem::SurfMap::operator=(A);
      wallRadius=A.wallRadius;
      leftPhase=A.leftPhase;
      rightPhase=A.rightPhase;
      innerStep=A.innerStep;
      wallThick=A.wallThick;
      baseGap=A.baseGap;
      outerGap=A.outerGap;
      topRaise=A.topRaise;
      depth=A.depth;
      height=A.height;
      nTopLayers=A.nTopLayers;
      nMidLayers=A.nMidLayers;
      nBaseLayers=A.nBaseLayers;
      topFrac=A.topFrac;
      midFrac=A.midFrac;
      baseFrac=A.baseFrac;
      topMat=A.topMat;
      midMat=A.midMat;
      baseMat=A.baseMat;
      wallMat=A.wallMat;
    }
  return *this;
}


Curtain::~Curtain() 
  /*!
    Destructor
  */
{}

void
Curtain::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("Curtain","populate");

  leftPhase=Control.EvalVar<double>(keyName+"LeftPhase");
  rightPhase=Control.EvalVar<double>(keyName+"RightPhase");

  innerStep=Control.EvalVar<double>(keyName+"InnerStep");
  wallThick=Control.EvalVar<double>(keyName+"WallThick");
  baseGap=Control.EvalDefVar<double>(keyName+"BaseGap",0.0);
  outerGap=Control.EvalDefVar<double>(keyName+"OuterGap",0.0);
  topRaise=Control.EvalVar<double>(keyName+"TopRaise");
  height=Control.EvalVar<double>(keyName+"Height");
  depth=Control.EvalVar<double>(keyName+"Depth");
    
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");

  nTopLayers=Control.EvalVar<size_t>(keyName+"NTopLayers");
  nMidLayers=Control.EvalVar<size_t>(keyName+"NMidLayers");
  nBaseLayers=Control.EvalVar<size_t>(keyName+"NBaseLayers");

  //  ModelSupport::populateDivide(Control,nLayers,keyName+"WallMat",
  //			       wallMat,wallMatVec);
  ModelSupport::populateDivideLen(Control,nTopLayers,keyName+"TopLen",
				  height-topRaise,topFrac);
  ModelSupport::populateDivideLen(Control,nMidLayers,keyName+"MidLen",
				  topRaise,midFrac);
  ModelSupport::populateDivideLen(Control,nBaseLayers,keyName+"BaseLen",
				  depth,baseFrac);

  ModelSupport::populateDivide(Control,nBaseLayers,keyName+"BaseMat",
                               ModelSupport::EvalMatName("Void"),
                               baseMat);	  
  return;
}
  
void
Curtain::createLocalUnitVector(const attachSystem::FixedComp& FC,
			  const long int topIndex,
			  const long int sideIndex)
/*!
    Create the unit vectors
    \param MainCentre :: Main rotation centre
    \param FC :: object for origin/radius
    \param topIndex :: top of monolith
    \param sideIndex :: Side for monolith
  */
{
  ELog::RegMethod RegA("Curtain","createUnitVector");

  attachSystem::FixedComp& topFC=FixedGroup::getKey("Top");
  attachSystem::FixedComp& midFC=FixedGroup::getKey("Mid");
  attachSystem::FixedComp& baseFC=FixedGroup::getKey("Lower");

  topFC.createUnitVector(FC,sideIndex);
  topFC.setCentre(FC.getLinkPt(topIndex));
  midFC.createUnitVector(FC,sideIndex);
  midFC.setCentre(FC.getLinkPt(topIndex));
  baseFC.createUnitVector(FC,sideIndex);
  baseFC.setCentre(FC.getLinkPt(topIndex));
  
  //  Origin=FC.getLinkPt(topIndex);

  wallRadius=FC.getCentre().Distance(FC.getLinkPt(sideIndex));
  setDefault("Lower");


  return;
}
  
void
Curtain::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("Curtain","createSurface");

  // THIS NEEDS A GENERIC:
  Geometry::Vec3D AWallDir(X);
  Geometry::Vec3D BWallDir(X);
  // rotation of axis:
  Geometry::Quaternion::calcQRotDeg(leftPhase,-Z).rotate(AWallDir);
  Geometry::Quaternion::calcQRotDeg(rightPhase,-Z).rotate(BWallDir);
  // rotation of phase points:

  // Points on wall
  Geometry::Vec3D AWall(Y*wallRadius);
  Geometry::Vec3D BWall(Y*wallRadius);
  Geometry::Quaternion::calcQRotDeg(-leftPhase,Z).rotate(AWall);
  Geometry::Quaternion::calcQRotDeg(-rightPhase,Z).rotate(BWall);
  AWall+=Origin;
  BWall+=Origin;
  // Divider
  ModelSupport::buildCylinder(SMap,buildIndex+7,
			      Origin,Z,wallRadius-innerStep);
  ModelSupport::buildCylinder(SMap,buildIndex+17,
			      Origin,Z,wallRadius-innerStep+wallThick);
  ModelSupport::buildCylinder(SMap,buildIndex+27,
			      Origin,Z,wallRadius+wallThick);
  ModelSupport::buildCylinder(SMap,buildIndex+127,
			      Origin,Z,wallRadius+wallThick+outerGap);

  
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*depth,Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*height,Z);
  ModelSupport::buildPlane(SMap,buildIndex+15,Origin+Z*topRaise,Z);

  ModelSupport::buildPlane(SMap,buildIndex+105,Origin-Z*(depth+baseGap),Z);
  
  ModelSupport::buildPlane(SMap,buildIndex+3,AWall,AWallDir);
  ModelSupport::buildPlane(SMap,buildIndex+4,BWall,BWallDir);

  setSurf("OuterRadius",SMap.realSurf(buildIndex+127));
  setSurf("OuterZStep",SMap.realSurf(buildIndex+15));
  return;
}

  
void
Curtain::createObjects(Simulation& System,
		       const attachSystem::FixedComp& FC,  
		       const long int topIndex,
		       const long int sideIndex)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
    \param FC :: Side of bulk shield + divider(?)
    \param topIndex :: Index to top of roof
    \param sideIndex :: side of link point 
  */
{
  const ELog::RegMethod RegA("Curtain","createObjects");

  HeadRule HR;

  const HeadRule topSurfHR=FC.getFullRule(topIndex);
  const HeadRule topBaseHR=FC.getFullRule(-topIndex);
  const HeadRule sideSurfHR=FC.getFullRule(sideIndex);

  // Top section
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"7 -17 3 -4 15 -6");
  makeCell("topWall",System,cellIndex++,wallMat,0.0,HR);
  addOuterSurf("Top",HR);

  // Mid section
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"7 -27 3 -4 -15");
  makeCell("midWall",System,cellIndex++,wallMat,0.0,HR*topSurfHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"27 -127 3 -4 -15");
  makeCell("midGap",System,cellIndex++,0,0.0,HR*topSurfHR);

  // Lower section
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-27 3 -4 5");
  makeCell("baseWall",System,cellIndex++,wallMat,0.0,
	   HR*topBaseHR*sideSurfHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-27 3 -4 -5 105");
  makeCell("baseGap",System,cellIndex++,0,0.0,HR*sideSurfHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"27 -127 3 -4 105");
  makeCell("baseGap",System,cellIndex++,0,0.0,HR*topBaseHR*sideSurfHR);


  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-127 3 -4 105");
  addOuterSurf("Lower",HR*topBaseHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"7 -127 3 -4 -15");
  addOuterSurf("Mid",HR*topSurfHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"105 -127 3 -4");
  addOuterSurf("RoofCut",HR);
  return;
}

void 
Curtain::layerProcess(Simulation& System,
		      const attachSystem::FixedComp& FC,  
		      const long int topIndex)
  /*!
    Processes the splitting of the surfaces into a multilayer system
    \param System :: Simulation to work on
    \param FC :: Side of bulk shield + divider(?)
    \param topIndex :: top index

  */
{
  ELog::RegMethod RegA("Curtain","layerProcess");
  std::string OutA,OutB;
  
  if (nTopLayers>1)
    {
      std::string OutA,OutB;
      ModelSupport::surfDivide DA;
            
      for(size_t i=1;i<nTopLayers;i++)
	{
	  DA.addFrac(topFrac[i-1]);
	  DA.addMaterial(wallMat);
	}
      DA.addMaterial(wallMat);
      
      DA.setCellN(getCell("topWall"));
      DA.setOutNum(cellIndex,buildIndex+1001);
      ModelSupport::mergeTemplate<Geometry::Plane,
				  Geometry::Plane> surroundRule;
      surroundRule.setSurfPair(SMap.realSurf(buildIndex+15),
			       SMap.realSurf(buildIndex+6));

      const HeadRule HRA(SMap,buildIndex,15);
      const HeadRule HRB(SMap,buildIndex,-6);
      
      surroundRule.setInnerRule(HRA);
      surroundRule.setOuterRule(HRB);
      
      DA.addRule(&surroundRule);
      DA.activeDivideTemplate(System);
      cellIndex=DA.getCellNum();
      //      removeCell("topWall");
      //      setCells(cellName,firstCell,cellIndex-1);

    }
  if (nBaseLayers>1)
    {
      const int topSurf=FC.getLinkSurf(topIndex);
      ModelSupport::surfDivide DA;
            
      for(size_t i=1;i<nBaseLayers;i++)
	{
	  DA.addFrac(baseFrac[i-1]);
	  DA.addMaterial(baseMat[i-1]);
	}
      DA.addMaterial(baseMat.back());
      
      DA.setCellN(getCell("baseWall"));
      DA.setOutNum(cellIndex,buildIndex+1101);
      ModelSupport::mergeTemplate<Geometry::Plane,
				  Geometry::Plane> surroundRule;
      surroundRule.setSurfPair(SMap.realSurf(buildIndex+5),
                               SMap.realSurf(topSurf));

      const HeadRule HRA=FC.getFullRule(-topIndex);
      const HeadRule HRB(SMap,buildIndex,5);
      
      surroundRule.setInnerRule(HRB);
      surroundRule.setOuterRule(HRA);
      
      DA.addRule(&surroundRule);
      DA.activeDivideTemplate(System);
      cellIndex=DA.getCellNum();
      //      removeCell("topWall");
      //      setCells(cellName,firstCell,cellIndex-1);

    }

  return;
}

  
void
Curtain::createLinks()
  /*!
    Create all the linkes [OutGoing]
  */
{
  ELog::RegMethod RegA("Curtain","createLinks");

  //  attachSystem::FixedComp& topFC=FixedGroup::getKey("Top");
  attachSystem::FixedComp& baseFC=FixedGroup::getKey("Lower");

  // Lower first:
  //  const double angleStep((M_PI/180.0)*(rightPhase-leftPhase)/3.0);
  double angle(M_PI*leftPhase/180.0);
  for(size_t i=0;i<4;i++)
    {
      const Geometry::Vec3D Axis(Y*cos(angle)+X*sin(angle));
      const Geometry::Vec3D OutPt=Origin+Axis*(wallRadius+wallThick);
      const Geometry::Vec3D InPt=Origin+Axis*wallRadius;
      baseFC.setConnect(i,OutPt-Z*depth,Axis);
      baseFC.setConnect(i+4,InPt-Z*depth,-Axis);

      baseFC.setConnect(i+8,OutPt+Z*height,Axis);
      baseFC.setConnect(i+12,InPt+Z*height,-Axis);

      baseFC.setConnect(i,OutPt,Axis);
      baseFC.setConnect(i+4,InPt,-Axis);
      baseFC.setLinkSurf(i,SMap.realSurf(buildIndex+27));
      baseFC.setLinkSurf(i+8,SMap.realSurf(buildIndex+27));   
    }

  return;
}

void
Curtain::buildAll(Simulation& System,
		   const attachSystem::FixedComp& FC,
		   const long int topIndex,
		   const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param topIndex :: Top of monolith
    \param sideIndex :: Side of monolith
  */
{
  ELog::RegMethod RegA("Curtain","buildAll");

  populate(System.getDataBase());
  createLocalUnitVector(FC,topIndex,sideIndex);
  createSurfaces();
  createLinks();
  createObjects(System,FC,topIndex,sideIndex);
  layerProcess(System,FC,topIndex);
  insertObjects(System);              

  return;
}

}  // NAMESPACE essSystem
