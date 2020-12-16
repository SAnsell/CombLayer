/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   zoom/ZoomPrimary.cxx
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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
#include "Rules.h"
#include "HeadRule.h"
#include "SurInter.h"
#include "surfDBase.h"
#include "mergeTemplate.h"
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
#include "ExternalCut.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "ContainedComp.h"
#include "BulkShield.h"
#include "ZoomPrimary.h"

namespace zoomSystem
{

ZoomPrimary::ZoomPrimary(const std::string& Key) : 
  attachSystem::FixedGroup(Key,"Main",6,"Beam",2),
  attachSystem::ContainedComp(),
  attachSystem::ExternalCut(),
  nLayers(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

ZoomPrimary::ZoomPrimary(const ZoomPrimary& A) : 
  attachSystem::FixedGroup(A),attachSystem::ContainedComp(A),
  attachSystem::ExternalCut(A),
  length(A.length),height(A.height),
  depth(A.depth),leftWidth(A.leftWidth),rightWidth(A.rightWidth),
  cutX(A.cutX),cutZ(A.cutZ),cutWidth(A.cutWidth),
  cutHeight(A.cutHeight),nLayers(A.nLayers),feMat(A.feMat),
  innerVoid(A.innerVoid)
  /*!
    Copy constructor
    \param A :: ZoomPrimary to copy
  */
{}

ZoomPrimary&
ZoomPrimary::operator=(const ZoomPrimary& A)
  /*!
    Assignment operator
    \param A :: ZoomPrimary to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedGroup::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      attachSystem::ExternalCut::operator=(A);
      length=A.length;
      height=A.height;
      depth=A.depth;
      leftWidth=A.leftWidth;
      rightWidth=A.rightWidth;
      cutX=A.cutX;
      cutZ=A.cutZ;
      cutWidth=A.cutWidth;
      cutHeight=A.cutHeight;
      nLayers=A.nLayers;
      feMat=A.feMat;
      innerVoid=A.innerVoid;
    }
  return *this;
}

ZoomPrimary::~ZoomPrimary() 
  /*!
    Destructor
  */
{}

void
ZoomPrimary::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("ZoomPrimary","populate");

  length=Control.EvalVar<double>(keyName+"Length");
  depth=Control.EvalVar<double>(keyName+"Depth");
  height=Control.EvalVar<double>(keyName+"Height");
  leftWidth=Control.EvalVar<double>(keyName+"LeftWidth");
  rightWidth=Control.EvalVar<double>(keyName+"RightWidth");

  cutX=Control.EvalVar<double>(keyName+"CutX");
  cutZ=Control.EvalVar<double>(keyName+"CutZ");
  cutWidth=Control.EvalVar<double>(keyName+"CutWidth");
  cutHeight=Control.EvalVar<double>(keyName+"CutHeight");

  feMat=ModelSupport::EvalMat<int>(Control,keyName+"FeMat");

  nLayers=Control.EvalVar<size_t>(keyName+"NLayers");

  // Need width/heigth form the divide [if used]
  const double minDist(std::min(leftWidth,rightWidth));
  ModelSupport::populateDivide(Control,nLayers,keyName+"Mat_",
   			       feMat,cMat);
  ModelSupport::populateDivideLen(Control,nLayers,
				  keyName+"Frac_",minDist,cFrac);

  return;
}

void
ZoomPrimary::createSurfaces()
  /*!
    Create All the surfaces
    \param LC :: Linear Component [to get outer surface]
  */
{
  ELog::RegMethod RegA("ZoomPrimary","createSurface");

  setDefault("Main","Beam");


  
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*length,Y);
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*leftWidth,X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*rightWidth,X);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*depth,Z);


  //  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*height,Z);

  // inner void
  ModelSupport::buildPlane(SMap,buildIndex+13,
			   bOrigin+X*(cutX-cutWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+14,
			   bOrigin+X*(cutX+cutWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+15,
			   bOrigin+Z*(cutZ-cutHeight/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,
			   bOrigin+Z*(cutZ+cutHeight/2.0),Z);
    
  return;
}

void
ZoomPrimary::createObjects(Simulation& System)
  /*!
    Adds the  guide components
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("ZoomPrimary","createObjects");

  const std::string frontStr=
    ExternalCut::getRuleStr("front");
  const HeadRule cutHR=
    ExternalCut::getRule("side");
  const int SN=cutHR.getPrimarySurface();
  //  SMap.addMatch(buildIndex+1,LC.getLinkSurf(2));   // back plane
  SMap.addMatch(buildIndex+6,SN);   // right plane

  std::string Out;
  // Outer steel
  Out=ModelSupport::getComposite(SMap,buildIndex," -2 3 -4 5 ");
  addOuterSurf(Out+frontStr+cutHR.display());      
  Out+=ModelSupport::getComposite(SMap,buildIndex," (-13:14:-15:16) ");
  System.addCell(MonteCarlo::Object
		 (cellIndex++,feMat,0.0,Out+frontStr+cutHR.display()));
  CDivideList.push_back(cellIndex-1);

  // Inner void:
  Out=ModelSupport::getComposite(SMap,buildIndex," -2 13 -14 15 -16");
  System.addCell(MonteCarlo::Object(cellIndex++,0,0.0,Out+frontStr));
  innerVoid=cellIndex-1;

  return;
}

void 
ZoomPrimary::createLinks()
  /*!
    Create the linked units
   */
{
  ELog::RegMethod RegA("ZoomPrimary","createLinks");

    
  attachSystem::FixedComp& mainFC=FixedGroup::getKey("Main");
  attachSystem::FixedComp& beamFC=FixedGroup::getKey("Beam");
  
  mainFC.setConnect(0,Origin,-Y);
  mainFC.setConnect(1,Origin+Y*length,Y);     
  mainFC.setConnect(2,Origin-X*leftWidth/2.0,-X);     
  mainFC.setConnect(3,Origin+X*rightWidth/2.0,X);     
  mainFC.setConnect(4,Origin-Z*depth,-Z);     
  mainFC.setConnect(5,Origin+Z*height,Z);     

  mainFC.setLinkSurf(0,ExternalCut::getRule("front"));
  mainFC.setLinkSurf(5,ExternalCut::getRule("side"));
		       
  for(int i=1;i<6;i++)
    mainFC.setLinkSurf(static_cast<size_t>(i),
			   SMap.realSurf(buildIndex+i+1));

  beamFC.setConnect(0,bOrigin,-bY);
  beamFC.setExit(buildIndex+2,bOrigin,bY);

  return;
}

void 
ZoomPrimary::layerProcess(Simulation& System)
  /*!
    Processes the splitting of the surfaces into a multilayer system
    This has to deal with the three layers that split cells:
    \param System :: Simulation to work on
  */
{
  ELog::RegMethod RegA("ZoomPrimary","layerProcess");

  if (!nLayers) return;

  ModelSupport::surfDivide DA;
  // Generic
  for(size_t i=1;i<nLayers;i++)
    {
      DA.addFrac(cFrac[i-1]);
      DA.addMaterial(cMat[i-1]);
    }
  DA.addMaterial(cMat.back());

  const size_t CSize=CDivideList.size();
  for(size_t i=0;i<CSize;i++)
    {
      DA.init(); 
      // Cell Specific:
      DA.setCellN(CDivideList[i]);
      DA.setOutNum(cellIndex,
		   buildIndex+201+100*static_cast<int>(i));

      // Modern divider system:
      ModelSupport::mergeTemplate<Geometry::Plane,
				  Geometry::Plane> wallRule;
      wallRule.setSurfPair(SMap.realSurf(buildIndex+3),
			   SMap.realSurf(buildIndex+13));
      wallRule.setSurfPair(SMap.realSurf(buildIndex+4),
			   SMap.realSurf(buildIndex+14));
      wallRule.setSurfPair(SMap.realSurf(buildIndex+5),
			   SMap.realSurf(buildIndex+15));
      wallRule.setSurfPair(SMap.realSurf(buildIndex+6),
			   SMap.realSurf(buildIndex+16));

      const std::string OutA=
	ModelSupport::getComposite(SMap,buildIndex," 3 -4 5 -6 ");
      const std::string OutB=
	ModelSupport::getComposite(SMap,buildIndex," (-13:14:-15:16) ");
      wallRule.setInnerRule(OutA);
      wallRule.setOuterRule(OutB);

      DA.addRule(&wallRule);
      DA.activeDivideTemplate(System);
      cellIndex=DA.getCellNum();
    }
  return;
}

int
ZoomPrimary::exitWindow(const double Dist,
			   std::vector<int>& window,
			   Geometry::Vec3D& Pt) const
  /*!
    Outputs a window
    \param Dist :: Distance from window
    \param window :: window vector of paired planes
    \param Pt :: Point for 
    \return Point at exit + Dist
  */
{
  window.clear();
  // Not valid numbers:
  window.push_back(SMap.realSurf(buildIndex+23));
  window.push_back(SMap.realSurf(buildIndex+24));
  window.push_back(SMap.realSurf(buildIndex+25));
  window.push_back(SMap.realSurf(buildIndex+26));
  // Note cant rely on exit point because that is the 
  // virtual 46 degree exit point.
  Pt=Origin+Y*(length+Dist); 
  return SMap.realSurf(buildIndex+2);
}

void
ZoomPrimary::createAll(Simulation& System,
		       const attachSystem::FixedComp& ZC,
		       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param ZC :: Zoom chopper
  */
{
  ELog::RegMethod RegA("ZoomPrimary","createAll");
  
  populate(System.getDataBase());
  createUnitVector(ZC,sideIndex);
  setDefault("Main","Beam");
  createSurfaces();
  createObjects(System);
  createLinks();
  layerProcess(System);
  insertObjects(System);   
  
  return;
}
  
}  // NAMESPACE shutterSystem


