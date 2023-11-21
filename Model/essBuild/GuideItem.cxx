/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/GuideItem.cxx
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
#include "Exception.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "Surface.h"
#include "surfRegister.h"
#include "SurInter.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
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
#include "FixedRotateGroup.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "ExternalCut.h"
#include "BaseMap.h"
#include "SurfMap.h"
#include "CellMap.h"
#include "GuideItem.h"

namespace essSystem
{

GuideItem::GuideItem(const std::string& Key,const size_t Index)  :
  attachSystem::FixedRotateGroup(Key+std::to_string(Index),
                                 "Main",6,"Beam",6),
  attachSystem::ContainedGroup("Inner","Outer"),
  attachSystem::ExternalCut(),
  attachSystem::SurfMap(),
  attachSystem::CellMap(),
  baseName(Key),active(1),RInner(0),ROuter(0),GPtr(nullptr)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
    \param Index :: Index of guide unit
  */
{}

GuideItem::GuideItem(const GuideItem& A) : 
  attachSystem::FixedRotateGroup(A),
  attachSystem::ContainedGroup(A),
  attachSystem::ExternalCut(A),
  attachSystem::SurfMap(A),
  attachSystem::CellMap(A),
  baseName(A.baseName),active(A.active),beamXStep(A.beamXStep),
  beamZStep(A.beamZStep),beamXYAngle(A.beamXYAngle),
  beamZAngle(A.beamZAngle),beamWidth(A.beamWidth),
  beamHeight(A.beamHeight),nSegment(A.nSegment),
  height(A.height),depth(A.depth),width(A.width),length(A.length),
  mat(A.mat),
  RInner(A.RInner),ROuter(A.ROuter),GPtr(A.GPtr)
  /*!
    Copy constructor
    \param A :: GuideItem to copy
  */
{}

GuideItem&
GuideItem::operator=(const GuideItem& A)
  /*!
    Assignment operator
    \param A :: GuideItem to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedRotateGroup::operator=(A);
      attachSystem::ContainedGroup::operator=(A);
      attachSystem::ExternalCut::operator=(A);
      attachSystem::SurfMap::operator=(A);
      attachSystem::CellMap::operator=(A);
      active=A.active;
      beamXStep=A.beamXStep;
      beamZStep=A.beamZStep;
      beamXYAngle=A.beamXYAngle;
      beamZAngle=A.beamZAngle;
      beamWidth=A.beamWidth;
      beamHeight=A.beamHeight;
      nSegment=A.nSegment;
      height=A.height;
      depth=A.depth;
      width=A.width;
      length=A.length;
      mat=A.mat;
      RInner=A.RInner;
      ROuter=A.ROuter;
      GPtr=A.GPtr;
    }
  return *this;
}


GuideItem::~GuideItem() 
  /*!
    Destructor
  */
{}

void
GuideItem::calcRadii()
  /*!
    Calc inner/oute radii
    [ copied from GuideBay -- this is junk so fix ]
   */
{
  ELog::RegMethod RegA("GuideItem","calcRadii");
  
  // construct
  const HeadRule innerHR=getRule("innerCyl");
  const HeadRule outerHR=getRule("outerCyl");

  const Geometry::Cylinder* CInner=
    dynamic_cast<const Geometry::Cylinder*>(innerHR.primarySurface());
  const Geometry::Cylinder* COuter=
    dynamic_cast<const Geometry::Cylinder*>(innerHR.primarySurface());

  if (!CInner || !COuter)
    throw ColErr::InContainerError<std::string>
      ("Inner/Outer","inner/outer Cyl not cylinder");

  RInner=CInner->getRadius();
  ROuter=COuter->getRadius();
  return;
}

void
GuideItem::populate(const FuncDataBase& Control)
 /*!
   Populate all the variables
   \param Control :: Func Data Base to use
 */
{
  ELog::RegMethod RegA("GuideItem","populate");


  FixedRotateGroup::populate(Control);
  active=Control.EvalTail<int>(keyName,baseName,"Active");

  beamXYAngle=Control.EvalTail<double>(keyName,baseName,"BeamXYAngle");
  beamZAngle=Control.EvalTail<double>(keyName,baseName,"BeamZAngle");
  beamXStep=Control.EvalTail<double>(keyName,baseName,"BeamXStep");
  beamZStep=Control.EvalTail<double>(keyName,baseName,"BeamZStep");

  beamHeight=Control.EvalTail<double>(keyName,baseName,"BeamHeight");
  beamWidth=Control.EvalTail<double>(keyName,baseName,"BeamWidth");

  sideGap=Control.EvalTail<double>(keyName,baseName,"SideGap");
  topGap=Control.EvalTail<double>(keyName,baseName,"TopGap");
  baseGap=Control.EvalTail<double>(keyName,baseName,"BaseGap");

  double D,W,H,L(RInner);
  nSegment=Control.EvalTail<size_t>(keyName,baseName,"NSegment");
  for(size_t i=0;i<nSegment;i++)
    {
      W=Control.EvalTail<double>(keyName,baseName,
				 "Width"+std::to_string(i+1));
      H=Control.EvalTail<double>(keyName,baseName,
				 "Height"+std::to_string(i+1));
      D=Control.EvalTail<double>(keyName,baseName,
				 "Depth"+std::to_string(i+1));
      if (i!=nSegment-1)
	{
	  L+=Control.EvalTail<double>(keyName,baseName,
				      "Length"+std::to_string(i+1));
	  length.push_back(L);
	}
      height.push_back(H);
      depth.push_back(D);
      width.push_back(W);
    }
  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat",baseName+"Mat");
  filled=Control.EvalTail<int>(keyName,baseName,"Filled");

  return;
}
  
void
GuideItem::createUnitVector(const attachSystem::FixedComp& FC,
			    const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Linked object
    \param sideIndex :: Mid-point of inner bay 
  */
{
  ELog::RegMethod RegA("GuideItem","createUnitVector");

  attachSystem::FixedComp& mainFC=FixedGroup::getKey("Main");
  attachSystem::FixedComp& beamFC=FixedGroup::getKey("Beam");
  
  mainFC.createUnitVector(FC,0);  
  beamFC.createUnitVector(FC,sideIndex);
  applyOffset();
  // Need to calculate impact point of beamline:
  const double yShift=sqrt(RInner*RInner-beamXStep*beamXStep)-RInner;
  beamFC.applyShift(beamXStep,yShift,beamZStep);
  beamFC.applyAngleRotate(beamXYAngle,beamZAngle);
  setDefault("Beam");
  
  return;
}

void
GuideItem::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("GuideItem","createSurface");

  if (!ExternalCut::isActive("divider"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);    // Divider plane
      setCutSurf("divider",SMap.realSurf(buildIndex+1));
    }
  else
    ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);    // Divider plane
  
  const attachSystem::FixedComp& mainFC=
    FixedGroup::getKey("Main");
  const attachSystem::FixedComp& beamFC=
    FixedGroup::getKey("Beam");

  const Geometry::Vec3D mainOrigin=mainFC.getCentre();
  const Geometry::Vec3D beamOrigin=beamFC.getCentre();
  
  const Geometry::Vec3D& bX=beamFC.getX();
  const Geometry::Vec3D& bZ=beamFC.getZ();

  int GI(buildIndex);
  for(size_t i=0;i<nSegment;i++)
    {
      if (i!=nSegment-1)
	ModelSupport::buildCylinder(SMap,GI+57,mainOrigin,Z,length[i]);

      SurfMap::makePlane("Left",SMap,GI+3,beamOrigin-bX*(width[i]/2.0),bX);
      SurfMap::makePlane("Right",SMap,GI+4,beamOrigin+bX*(width[i]/2.0),bX);
      SurfMap::makePlane("Base",SMap,GI+5,beamOrigin-bZ*depth[i],bZ);
      SurfMap::makePlane("Top",SMap,GI+6,beamOrigin+bZ*height[i],bZ);

      ModelSupport::buildPlane
        (SMap,GI+13,beamOrigin-bX*(-sideGap+width[i]/2.0),bX);
      ModelSupport::buildPlane
        (SMap,GI+14,beamOrigin+bX*(-sideGap+width[i]/2.0),bX);
      ModelSupport::buildPlane(SMap,GI+15,beamOrigin-bZ*(-baseGap+depth[i]),bZ);
      ModelSupport::buildPlane(SMap,GI+16,beamOrigin+bZ*(-topGap+height[i]),bZ);
      GI+=50;
    }

  // Beamline :: 
  ModelSupport::buildPlane(SMap,buildIndex+1103,
			   beamOrigin-bX*(beamWidth/2.0),bX);
  ModelSupport::buildPlane(SMap,buildIndex+1104,
			   beamOrigin+bX*(beamWidth/2.0),bX);
  ModelSupport::buildPlane(SMap,buildIndex+1105,
			   beamOrigin-bZ*(beamHeight/2.0),bZ);
  ModelSupport::buildPlane(SMap,buildIndex+1106,
			   beamOrigin+bZ*(beamHeight/2.0),bZ);
  
  
  return;
}

const Geometry::Plane*
GuideItem::getPlane(const int SN) const
  /*!
    Access to specific plane
    \param SN :: Surface offset [without buildIndex]
    \return Plane Number
  */
{
  ELog::RegMethod RegA("GuideItem","getPlane");
  return SMap.realPtr<Geometry::Plane>(buildIndex+SN);
}


HeadRule
GuideItem::getEdge(const GuideItem* GPtr) const
  /*!
    Given another GuideItem determine the end point collision
    \param GPtr :: Other object Ptr [0 for none]
    \return Edge  (3/4 side)
   */
{
  ELog::RegMethod RegA("GuideItem","getEdge");

  HeadRule HR;
  if (!GPtr) return HR;

  const attachSystem::FixedComp& beamFC=FixedGroup::getKey("Beam");
  const Geometry::Vec3D beamOrigin=beamFC.getCentre();
  

  
  const Geometry::Plane* gP3=GPtr->getPlane(3);
  const Geometry::Plane* gP4=GPtr->getPlane(4);
  const Geometry::Plane* P3=getPlane(3);
  const Geometry::Plane* P4=getPlane(4);
  const Geometry::Plane* ZPlane=getPlane(6);
  
  const Geometry::Vec3D RptA=SurInter::getPoint(gP3,P4,ZPlane,beamOrigin);
  const Geometry::Vec3D RptB=SurInter::getPoint(gP4,P3,ZPlane,beamOrigin);

  const HeadRule cylHR=getRule("innerCyl");
  const HeadRule divHR=getRule("divider");

  if (cylHR.isValid(RptA) && divHR.isValid(RptA))
    HR=GPtr->sideExclude(0);
  else if (cylHR.isValid(RptB) && divHR.isValid(RptB))
    HR=GPtr->sideExclude(1);
  return HR;
}

HeadRule
GuideItem::wheelHeadRule() const
  /*!
    Decides is wheel headrule cuts into the guide item
    and returns a HeadRule if that is the case.
   */
{
  ELog::RegMethod RegA("GiudeItem","wheelHeadRule");

  const HeadRule wheelHR=getRule("wheel");

  const attachSystem::FixedComp& beamFC=FixedGroup::getKey("Beam");
  const Geometry::Vec3D& beamOrigin=beamFC.getCentre();
  const Geometry::Vec3D& bY=beamFC.getY();

  const HeadRule innerHR=getRule("innerCyl");
  Geometry::Vec3D beamEnter=
    SurInter::getLinePoint(beamOrigin,bY,innerHR,
			   beamOrigin+bY*RInner);
  // need to project to origin:
  beamEnter=beamEnter.cutComponent(Z);    
  return (wheelHR.isValid(beamEnter)) ?
    wheelHR.complement() : HeadRule();
    
}

void
GuideItem::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("GuideItem","createObjects");

  if (!active) return;
  const HeadRule wheelHR=wheelHeadRule();   // null if not intersecting
  const HeadRule innerHR=getRule("innerCyl")*wheelHR;
  const HeadRule outerHR=getComplementRule("outerCyl"); 
  HeadRule divideHR=getRule("divider"); // populate ??
  if (!divideHR.isValid(Origin+Y*1000.0))
    divideHR.makeComplement();
  
  const HeadRule edgeHR=getEdge(GPtr);

  HeadRule HR;
  int GI(buildIndex);
  for(size_t i=0;i<nSegment;i++)
    {
      // Outer layer
      if (i==0)
	{
	  HR=ModelSupport::getHeadRule(SMap,buildIndex,"3 -4 5 -6 -57");
	  HR*=edgeHR*innerHR*divideHR;
	}
      else 
	HR=ModelSupport::getSetHeadRule(SMap,GI,"7 3 -4 5 -6 -57");
      HR*=divideHR;
      if (i+1==nSegment) HR*=outerHR;

      // This is a mess:
      if (!i)
	addOuterSurf("Inner",HR);
      else 
	addOuterUnionSurf("Outer",HR);

      // Add inner boundary
      HR*=ModelSupport::getHeadRule(SMap,GI,"(-13:14:-15:16)");
      makeCell("Body",System,cellIndex++,0,0.0,HR);

      if (i==0)
	{
	  HR=ModelSupport::getHeadRule
	    (SMap,buildIndex,"13 -14 15 -16 -57");
	  HR*=edgeHR*innerHR;
	}
      else 
	HR=ModelSupport::getSetHeadRule
	  (SMap,GI,buildIndex,"7 13 -14 15 -16 -57");
      HR*=divideHR;
      
      // Inner metal:
      if (!filled)
	HR*=ModelSupport::getHeadRule
	  (SMap,buildIndex,"(-1103:1104:-1105:1106)");
      if (i+1==nSegment) HR*=outerHR;
      
      makeCell("Body",System,cellIndex++,mat,0.0,HR);
      if (filled) addCell("Void",cellIndex-1);
      GI+=50;
    }      
  // Inner void
  if (!filled)
    {
      HR=ModelSupport::getHeadRule
	(SMap,buildIndex,"1103 -1104 1105 -1106 ");
      makeCell("Void",System,cellIndex++,0,0.0,HR*divideHR*innerHR*outerHR);
    }

  return;
}

HeadRule
GuideItem::sideExclude(const size_t sideIndex) const
  /*!
    Determine the string to exclude on a front point touch
    \param sideIndex :: 0 / 1
    \return string
  */
{
  return (!sideIndex) ?
    ModelSupport::getHeadRule(SMap,buildIndex,"(-3 : -5 : 6)") :
    ModelSupport::getHeadRule(SMap,buildIndex,"(4 : -5 : 6)");
}

void
GuideItem::createLinks()
  /*!
    Create all the linkes [OutGoing]
    - Beam links are for moderator centre
    - Main links are for target centre
  */
{
  ELog::RegMethod RegA("GuideItem","createLinks");

  attachSystem::FixedComp& mainFC=FixedGroup::getKey("Main");
  attachSystem::FixedComp& beamFC=FixedGroup::getKey("Beam");
  const Geometry::Vec3D& bX=-beamFC.getX();
  const Geometry::Vec3D& bY=beamFC.getY();
  const Geometry::Vec3D& bZ=beamFC.getZ();

  const Geometry::Vec3D beamOrigin=beamFC.getCentre();

  HeadRule divideHR=getRule("divider"); // populate ??
  if (!divideHR.isValid(Origin+Y*1000.0))
    divideHR.makeComplement();

  const HeadRule innerHR=getRule("innerCyl");
  const HeadRule outerHR=getRule("outerCyl");
  //  ELog::EM<<"Inner == "<<innerHR<<" "<<outerHR<<ELog::endDiag;
  
  const Geometry::Vec3D beamEnter=
    SurInter::getLinePoint(beamOrigin,bY,innerHR,
			   beamOrigin+bY*RInner);
  const Geometry::Vec3D beamExit=
    SurInter::getLinePoint(beamOrigin,bY,outerHR,
			   beamOrigin+bY*ROuter);
  

  beamFC.setConnect(0,beamEnter,-bY);

  beamFC.setLinkSurf(0,innerHR.complement());
  beamFC.setBridgeSurf(0,divideHR);

  const int GI=50*static_cast<int>(nSegment)+buildIndex;
  beamFC.setConnect(1,beamExit,bY);
  beamFC.setLinkSurf(1,outerHR);
  beamFC.setBridgeSurf(1,divideHR);
  if (!filled)
    {
      const Geometry::Vec3D MidPt((beamOrigin+bY*RInner+beamExit)/2.0);
      beamFC.setConnect(2,MidPt-bX*(beamWidth/2.0),-bX);
      beamFC.setConnect(3,MidPt+bX*(beamWidth/2.0),bX);
      beamFC.setConnect(4,MidPt-bZ*(beamWidth/2.0),-bZ);
      beamFC.setConnect(5,MidPt+bZ*(beamWidth/2.0),bZ);

      beamFC.setLinkSurf(2,-SMap.realSurf(buildIndex+1103));
      beamFC.setLinkSurf(3,SMap.realSurf(buildIndex+1104));
      beamFC.setLinkSurf(4,-SMap.realSurf(buildIndex+1105));
      beamFC.setLinkSurf(5,SMap.realSurf(buildIndex+1106));
    }

  /// TARGET CENTRE TRACKING:
  mainFC.setConnect(0,beamEnter-Geometry::Vec3D(0,0,beamEnter[2]),-bY);
  mainFC.setLinkSurf(0,innerHR.complement());
  mainFC.setBridgeSurf(0,divideHR);

  mainFC.setConnect(1,beamExit-Geometry::Vec3D(0,0,beamExit[2]),bY);
  mainFC.setLinkSurf(1,outerHR);
  mainFC.setBridgeSurf(1,divideHR);
    

  return;
}

void
GuideItem::createAll(Simulation& System,
		     const attachSystem::FixedComp& FC,
		     const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: side to used

  */
{
  ELog::RegMethod RegA("GuideItem","createAll");

  calcRadii();
  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);              

  return;
}

}  // NAMESPACE essSystem
