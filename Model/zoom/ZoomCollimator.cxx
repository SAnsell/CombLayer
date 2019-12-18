/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   zoom/ZoomCollimator.cxx
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
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
#include "Rules.h"
#include "SurInter.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "SimProcess.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"  
#include "FixedComp.h"
#include "FixedGroup.h"
#include "FixedOffset.h"
#include "FixedOffsetGroup.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "ExternalCut.h"
#include "ZoomChopper.h"
#include "ZoomStack.h"
#include "ZoomOpenStack.h"
#include "ZoomCollimator.h"

namespace zoomSystem
{

ZoomCollimator::ZoomCollimator(const std::string& Key) : 
  attachSystem::FixedOffsetGroup(Key,"Main",6,"Beam",2),
  attachSystem::ContainedComp(),
  attachSystem::ExternalCut(),
  cStack("zoomColStack"),nLayers(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

ZoomCollimator::ZoomCollimator(const ZoomCollimator& A) : 
  attachSystem::FixedOffsetGroup(A),attachSystem::ContainedComp(A),
  attachSystem::ExternalCut(A),
  cStack(A.cStack),
  length(A.length),height(A.height),depth(A.depth),
  leftWidth(A.leftWidth),rightWidth(A.rightWidth),
  stackWidth(A.stackWidth),stackHeight(A.stackHeight),
  stackXShift(A.stackXShift),stackZShift(A.stackZShift),
  nLayers(A.nLayers),feMat(A.feMat),innerVoid(A.innerVoid),
  cFrac(A.cFrac),cMat(A.cMat),CDivideList(A.CDivideList)
  /*!
    Copy constructor
    \param A :: ZoomCollimator to copy
  */
{}

ZoomCollimator&
ZoomCollimator::operator=(const ZoomCollimator& A)
  /*!
    Assignment operator
    \param A :: ZoomCollimator to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedOffsetGroup::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      attachSystem::ExternalCut::operator=(A);
      cStack=A.cStack;
      xStep=A.xStep;
      zStep=A.zStep;
      length=A.length;
      height=A.height;
      depth=A.depth;
      leftWidth=A.leftWidth;
      rightWidth=A.rightWidth;
      stackWidth=A.stackWidth;
      stackHeight=A.stackHeight;
      stackXShift=A.stackXShift;
      stackZShift=A.stackZShift;
      nLayers=A.nLayers;
      feMat=A.feMat;
      innerVoid=A.innerVoid;
      cFrac=A.cFrac;
      cMat=A.cMat;
      CDivideList=A.CDivideList;
    }
  return *this;
}

ZoomCollimator::~ZoomCollimator() 
  /*!
    Destructor
  */
{}

void
ZoomCollimator::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("ZoomCollimator","populate");

  stackXShift=Control.EvalVar<double>(keyName+"StackXShift");
  stackZShift=Control.EvalVar<double>(keyName+"StackZShift");

  length=Control.EvalVar<double>(keyName+"Length");
  depth=Control.EvalVar<double>(keyName+"Depth");
  height=Control.EvalVar<double>(keyName+"Height");
  leftWidth=Control.EvalVar<double>(keyName+"LeftWidth");
  rightWidth=Control.EvalVar<double>(keyName+"RightWidth");
  leftInnerWidth=Control.EvalVar<double>(keyName+"LeftInnerWidth");
  rightInnerWidth=Control.EvalVar<double>(keyName+"RightInnerWidth");
  leftInnerAngle=Control.EvalVar<double>(keyName+"LeftInnerAngle");
  rightInnerAngle=Control.EvalVar<double>(keyName+"RightInnerAngle");
  leftWaxSkin=Control.EvalVar<double>(keyName+"LeftWaxSkin");
  rightWaxSkin=Control.EvalVar<double>(keyName+"RightWaxSkin");

  stackHeight=Control.EvalVar<double>(keyName+"StackHeight");
  stackWidth=Control.EvalVar<double>(keyName+"StackWidth");

  feMat=ModelSupport::EvalMat<int>(Control,keyName+"FeMat");
  waxMat=ModelSupport::EvalMat<int>(Control,keyName+"WaxMat");

  nLayers=Control.EvalVar<size_t>(keyName+"NLayers");

  // Need widthh/eight from the divide [if used]
  const double minDist(std::min(leftWidth,rightWidth));
  ModelSupport::populateDivide(Control,nLayers,keyName+"Mat_",
   			       feMat,cMat);
  ModelSupport::populateDivideLen(Control,nLayers,
				  keyName+"Frac_",minDist,cFrac);

  return;
}


void
ZoomCollimator::createSurfaces()
  /*!
    Create All the surfaces
    \param LC :: Linear Component [to get outer surface]
  */
{
  ELog::RegMethod RegA("ZoomCollimator","createSurface");
  

  //  SMap.addMatch(buildIndex+1,LC.getLinkSurf(2));   // back plane
  //  SMap.addMatch(buildIndex+14,LC.getLinkSurf(4));   // right plane

  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*length,Y);
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*leftWidth,X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*rightWidth,X);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*depth,Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*height,Z);


  ModelSupport::buildPlane(SMap,buildIndex+203,
			   Origin-X*(leftWidth+leftWaxSkin),X);
  ModelSupport::buildPlane(SMap,buildIndex+204,
			   Origin+X*(rightWidth+rightWaxSkin),X);

  ModelSupport::buildPlaneRotAxis(SMap,buildIndex+113,
				  Origin-X*leftInnerWidth,X,
				  Z,-leftInnerAngle);     
  ModelSupport::buildPlaneRotAxis(SMap,buildIndex+114,
				  Origin+X*rightInnerWidth,X,
				  Z,rightInnerAngle);     

  ModelSupport::buildPlaneRotAxis(SMap,buildIndex+213,
				  Origin-X*(leftInnerWidth+leftWaxSkin),
				  X,Z,-leftInnerAngle);     
  ModelSupport::buildPlaneRotAxis(SMap,buildIndex+214,
				  Origin+X*(rightInnerWidth+rightWaxSkin),
				  X,Z,rightInnerAngle);     

  
  const Geometry::Vec3D StackCent=Origin+X*stackXShift+Z*stackZShift;
  ModelSupport::buildPlane(SMap,buildIndex+13,StackCent-X*(stackWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+14,StackCent+X*(stackWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+15,StackCent-Z*(stackHeight/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,StackCent+Z*(stackHeight/2.0),Z);

    
  // inner void
  ModelSupport::buildPlane(SMap,buildIndex+23,
			   bOrigin-X*stackWidth/2.0,X);
  ModelSupport::buildPlane(SMap,buildIndex+24,
			   bOrigin+X*stackWidth/2.0,X);
    
  return;
}

void
ZoomCollimator::createObjects(Simulation& System)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("ZoomCollimator","createObjects");

  const std::string frontStr=
    ExternalCut::getRuleStr("front");

  std::string Out;
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 " -2 213 203 -214 -204 5 -6 ");
  addOuterSurf(Out+frontStr);

  // Outer steel
  Out=ModelSupport::getComposite(SMap,buildIndex," -2 113 3 -114 "
				 " -4 5 -6  (-13:14:-15:16) ");
  System.addCell(MonteCarlo::Object(cellIndex++,feMat,0.0,Out+frontStr));

  // Outer Wax
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 " -2 (-113:-3) 213 203  5 -6 ");
  System.addCell(MonteCarlo::Object(cellIndex++,waxMat,0.0,Out+frontStr));
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 " -2 (114:4) -214 -204  5 -6 ");
  System.addCell(MonteCarlo::Object(cellIndex++,waxMat,0.0,Out+frontStr));

  // Inner void:
  Out=ModelSupport::getComposite(SMap,buildIndex," -2 13 -14 15 -16");
  System.addCell(MonteCarlo::Object(cellIndex++,0,0.0,Out+frontStr));
  innerVoid=cellIndex-1;

  return;
}

void 
ZoomCollimator::createLinks()
  /*!
    Create the linked units
   */
{
  ELog::RegMethod RegA("ZoomCollimator","createLinks");
  
  attachSystem::FixedComp& mainFC=FixedGroup::getKey("Main");
  attachSystem::FixedComp& beamFC=FixedGroup::getKey("Beam");

  // front not set
  mainFC.setConnect(0,Origin,-Y);       
  mainFC.setConnect(1,Origin+Y*length,Y);     
  mainFC.setConnect(2,Origin-X*leftWidth/2.0,-X);     
  mainFC.setConnect(3,Origin+X*rightWidth/2.0,X);     
  mainFC.setConnect(4,Origin-Z*depth,-Z);     
  mainFC.setConnect(5,Origin+Z*height,Z);     
  
  for(size_t i=1;i<6;i++)
    mainFC.setLinkSurf
      (i,SMap.realSurf(buildIndex+static_cast<int>(i)+1));

  beamFC.setExit(buildIndex+2,bOrigin,bY);

  return;
}

void 
ZoomCollimator::layerProcess(Simulation& System)
  /*!
    Processes the splitting of the surfaces into a multilayer system
    This has to deal with the three layers that split cells:
    \param System :: Simulation to work on
  */
{
  ELog::RegMethod RegA("ZoomCollimator","LayerProcess");

  if (!nLayers) return;
  ModelSupport::surfDivide DA;
  // Generic [nLayers > 0 ]
  for(size_t i=0;i<nLayers-1;i++)
    {
      DA.addFrac(cFrac[i]);
      DA.addMaterial(cMat[i]);
    }
  DA.addMaterial(cMat[nLayers-1]);

  const size_t CSize=CDivideList.size();
  for(size_t i=0;i<CSize;i++)
    {
      DA.init(); 
      // Cell Specific:
      DA.setCellN(CDivideList[i]);
      DA.setOutNum(cellIndex,buildIndex+201+100*static_cast<int>(i));

      DA.makePair<Geometry::Plane>(SMap.realSurf(buildIndex+23),
				   -SMap.realSurf(buildIndex+3));
      DA.makePair<Geometry::Plane>(SMap.realSurf(buildIndex+24),
       				   SMap.realSurf(buildIndex+4));
      DA.activeDivide(System);
      cellIndex=DA.getCellNum();
    }
  return;
}

void
ZoomCollimator::createAll(Simulation& System,
			  const attachSystem::FixedComp& ZC,
			  const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param ZC :: Zoom chopper
  */
{
  ELog::RegMethod RegA("ZoomCollimator","createAll");
  
  populate(System.getDataBase());
  createUnitVector(ZC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  layerProcess(System);
  insertObjects(System);   
  
  return;
}
  
}  // NAMESPACE shutterSystem
