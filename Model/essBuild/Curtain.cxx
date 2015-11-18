/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/Curtain.cxx
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#include "Line.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "inputParam.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "Simulation.h"
#include "ReadFunctions.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "CellMap.h"
#include "surfDBase.h"
#include "surfDIter.h"
#include "surfDivide.h"
#include "SurInter.h"
#include "mergeTemplate.h"

#include "World.h"
#include "Curtain.h"

namespace essSystem
{

Curtain::Curtain(const std::string& Key)  :
  attachSystem::ContainedGroup("Top","Mid","Lower"),
  attachSystem::FixedComp(Key,6),
  attachSystem::CellMap(),
  curIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(curIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

Curtain::Curtain(const Curtain& A) : 
  attachSystem::ContainedGroup(A),attachSystem::FixedComp(A),
  attachSystem::CellMap(A),
  curIndex(A.curIndex),cellIndex(A.cellIndex),wallRadius(A.wallRadius),
  leftPhase(A.leftPhase),rightPhase(A.rightPhase),
  innerStep(A.innerStep),wallThick(A.wallThick),
  topRaise(A.topRaise),depth(A.depth),height(A.height),
  nTopLayers(A.nTopLayers),nMidLayers(A.nMidLayers),
  nBaseLayers(A.nBaseLayers),topFrac(A.topFrac),
  midFrac(A.midFrac),baseFrac(A.baseFrac),wallMat(A.wallMat)
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
      attachSystem::ContainedGroup::operator=(A);
      attachSystem::FixedComp::operator=(A);
      attachSystem::CellMap::operator=(A);
      cellIndex=A.cellIndex;
      wallRadius=A.wallRadius;
      leftPhase=A.leftPhase;
      rightPhase=A.rightPhase;
      innerStep=A.innerStep;
      wallThick=A.wallThick;
      topRaise=A.topRaise;
      depth=A.depth;
      height=A.height;
      nTopLayers=A.nTopLayers;
      nMidLayers=A.nMidLayers;
      nBaseLayers=A.nBaseLayers;
      topFrac=A.topFrac;
      midFrac=A.midFrac;
      baseFrac=A.baseFrac;
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

  return;
}
  
void
Curtain::createUnitVector(const attachSystem::FixedComp& FC,
			  const long int topIndex,
			  const long int sideIndex,
			  const attachSystem::FixedComp& dirFC,
			  const long int dirIndex,
			  const bool reverseZ)
  /*!
    Create the unit vectors
    \param MainCentre :: Main rotation centre
    \param FC :: object for origin/radius
    \param dirFC :: object for Y-X-Z direction
    \param dirIndex :: direction of XYZ
    \param topIndex :: top of monolith
    \param sideIndex :: Side for monolith
  */
{
  ELog::RegMethod RegA("Curtain","createUnitVector");

  FixedComp::createUnitVector(dirFC,dirIndex);
  Origin=FC.getSignedLinkPt(topIndex);
  wallRadius=FC.getCentre().Distance(FC.getSignedLinkPt(sideIndex));
  if (reverseZ)
    {
      Z*=-1;
      X*=-1;
    }
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

  ModelSupport::buildCylinder(SMap,curIndex+7,
			      Origin,Z,wallRadius-innerStep);
  ModelSupport::buildCylinder(SMap,curIndex+17,
			      Origin,Z,wallRadius-innerStep+wallThick);
  ModelSupport::buildCylinder(SMap,curIndex+27,
			      Origin,Z,wallRadius+wallThick);
  
  ModelSupport::buildPlane(SMap,curIndex+5,Origin-Z*depth,Z);
  ModelSupport::buildPlane(SMap,curIndex+6,Origin+Z*height,Z);
  ModelSupport::buildPlane(SMap,curIndex+15,Origin+Z*topRaise,Z);

  ModelSupport::buildPlane(SMap,curIndex+3,AWall,AWallDir);
  ModelSupport::buildPlane(SMap,curIndex+4,BWall,BWallDir);
  
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
    \param sideIndex :: side of link point
  */
{
  ELog::RegMethod RegA("Curtain","createObjects");

  const std::string topSurf=FC.getSignedLinkString(topIndex);
  const std::string topBase=FC.getSignedLinkString(-topIndex);
  const std::string sideSurf=FC.getSignedLinkString(sideIndex);
  std::string Out;
  // Top section
  Out=ModelSupport::getComposite(SMap,curIndex," 7 -17 3 -4 15 -6 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));
  addOuterSurf("Top",Out);
  setCell("topWall",cellIndex-1);
  // Top section void
  //  Out=ModelSupport::getComposite(SMap,curIndex," 17 -27 3 -4 15 -6 ");
  //  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  //  addCell("topVoid",cellIndex-1);

  // Mid section
  Out=ModelSupport::getComposite(SMap,curIndex," 7 -27 3 -4 -15 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out+topSurf));
  addOuterSurf("Mid",Out+topSurf);
  setCell("midWall",cellIndex-1);

  // Lower section
  Out=ModelSupport::getComposite(SMap,curIndex," -27 3 -4 5 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,
				   Out+topBase+sideSurf));
  setCell("baseWall",cellIndex-1);

  Out=ModelSupport::getComposite(SMap,curIndex,"-27 3 -4 5 ");
  addOuterSurf("Lower",Out+topBase);

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
      DA.setOutNum(cellIndex,curIndex+1001);
      ModelSupport::mergeTemplate<Geometry::Plane,
				  Geometry::Plane> surroundRule;
      surroundRule.setSurfPair(SMap.realSurf(curIndex+15),
			       SMap.realSurf(curIndex+6));
      
      OutA=ModelSupport::getComposite(SMap,curIndex," 15 ");
      OutB=ModelSupport::getComposite(SMap,curIndex," -6 ");
      
      surroundRule.setInnerRule(OutA);
      surroundRule.setOuterRule(OutB);
      
      DA.addRule(&surroundRule);
      DA.activeDivideTemplate(System);
      cellIndex=DA.getCellNum();
      //      removeCell("topWall");
      //      setCells(cellName,firstCell,cellIndex-1);

    }
  if (nBaseLayers>1)
    {
      const int topSurf=FC.getSignedLU(topIndex).getLinkSurf();
      ELog::EM<<"Top surf == "<<topSurf<<ELog::endDiag;
      ModelSupport::surfDivide DA;
            
      for(size_t i=1;i<nTopLayers;i++)
	{
	  DA.addFrac(topFrac[i-1]);
	  DA.addMaterial(wallMat);
	}
      DA.addMaterial(wallMat);
      
      DA.setCellN(getCell("baseWall"));
      DA.setOutNum(cellIndex,curIndex+1101);
      ModelSupport::mergeTemplate<Geometry::Plane,
				  Geometry::Plane> surroundRule;
      surroundRule.setSurfPair(SMap.realSurf(curIndex+5),
			       SMap.realSurf(topSurf));
      
      OutA=ModelSupport::getComposite(SMap,curIndex," 5 ");
      OutB=FC.getSignedLinkString(-topIndex);
	//      OutB=ModelSupport::getComposite(SMap,curIndex,
	//			      );
      
      surroundRule.setInnerRule(OutA);
      surroundRule.setOuterRule(OutB);
      
      DA.addRule(&surroundRule);
      DA.activeDivideTemplate(System);
      cellIndex=DA.getCellNum();
      //      removeCell("topWall");
      //      setCells(cellName,firstCell,cellIndex-1);

    }
  // Steel layers
  //  layerSpecial(System);

  return;
}

  
void
Curtain::createLinks()
  /*!
    Create all the linkes [OutGoing]
  */
{
  ELog::RegMethod RegA("Curtain","createLinks");


  return;
}
  
  

  
void
Curtain::createAll(Simulation& System,
		   const attachSystem::FixedComp& FC,
		   const long int topIndex,
		   const long int sideIndex,
		   const attachSystem::FixedComp& dirFC,
		   const long int dirIndex,const bool reverseZ)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param topIndex :: Top of monolith
    \param sideIndex :: Side of monolith
    \param dirFC :: direction for Y,Z/X comp
    \param dirIndex :: direction for Y,Z/X
  */
{
  ELog::RegMethod RegA("Curtain","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,topIndex,sideIndex,dirFC,dirIndex,reverseZ);
  createSurfaces();
  createLinks();
  createObjects(System,FC,topIndex,sideIndex);
  layerProcess(System,FC,topIndex);
  insertObjects(System);              

  return;
}

}  // NAMESPACE essSystem
