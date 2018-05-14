/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/GuideItem.cxx
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#include "localRotate.h"
#include "masterRotate.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfEqual.h"
#include "SurInter.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "LineIntersectVisit.h"
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
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "FixedGroup.h"
#include "FixedOffsetGroup.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "World.h"
#include "GuideItem.h"

namespace essSystem
{

GuideItem::GuideItem(const std::string& Key,const size_t Index)  :
  attachSystem::ContainedGroup("Inner","Outer"),
  attachSystem::FixedOffsetGroup(Key+StrFunc::makeString(Index),
                                 "Main",6,"Beam",6),
  attachSystem::CellMap(),baseName(Key),
  guideIndex(ModelSupport::objectRegister::Instance().cell(keyName)),
  cellIndex(guideIndex+1),active(1),innerCyl(0),outerCyl(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
    \param Index :: Index of guide unit
  */
{}

GuideItem::GuideItem(const GuideItem& A) : 
  attachSystem::ContainedGroup(A),
  attachSystem::FixedOffsetGroup(A),
  attachSystem::CellMap(A),
  baseName(A.baseName),guideIndex(A.guideIndex),
  cellIndex(A.cellIndex),active(A.active),beamXStep(A.beamXStep),
  beamZStep(A.beamZStep),beamXYAngle(A.beamXYAngle),
  beamZAngle(A.beamZAngle),beamWidth(A.beamWidth),
  beamHeight(A.beamHeight),nSegment(A.nSegment),
  height(A.height),depth(A.depth),width(A.width),length(A.length),
  mat(A.mat),innerCyl(A.innerCyl),outerCyl(A.outerCyl),
  RInner(A.RInner),ROuter(A.ROuter)
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
      attachSystem::ContainedGroup::operator=(A);
      attachSystem::FixedOffsetGroup::operator=(A);
      attachSystem::CellMap::operator=(A);
      cellIndex=A.cellIndex;
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
      innerCyl=A.innerCyl;
      outerCyl=A.outerCyl;
      RInner=A.RInner;
      ROuter=A.ROuter;
    }
  return *this;
}


GuideItem::~GuideItem() 
  /*!
    Destructor
  */
{}

void
GuideItem::setCylBoundary(const int dPlane,const int A,const int B)
  /*!
    Set the boundary cylinders
    \param dPlane :: divide surface
    \param A :: Inner Cylinder
    \param B :: Outer Cylinder
   */
{
  ELog::RegMethod RegA("GuideItem","setCylBoundary");

  dividePlane=dPlane;
  innerCyl=abs(A);
  outerCyl=abs(B);

  const Geometry::Cylinder* CPtr=
    SMap.realPtr<Geometry::Cylinder>(innerCyl);
  RInner=CPtr->getRadius();
  const Geometry::Cylinder* DPtr=
    SMap.realPtr<Geometry::Cylinder>(outerCyl);
  ROuter=DPtr->getRadius();
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

  FixedOffsetGroup::populate(Control);
  active=Control.EvalPair<int>(keyName,baseName,"Active");

  beamXYAngle=Control.EvalPair<double>(keyName,baseName,"BeamXYAngle");
  beamZAngle=Control.EvalPair<double>(keyName,baseName,"BeamZAngle");
  beamXStep=Control.EvalPair<double>(keyName,baseName,"BeamXStep");
  beamZStep=Control.EvalPair<double>(keyName,baseName,"BeamZStep");

  beamHeight=Control.EvalPair<double>(keyName,baseName,"BeamHeight");
  beamWidth=Control.EvalPair<double>(keyName,baseName,"BeamWidth");

  sideGap=Control.EvalPair<double>(keyName,baseName,"SideGap");
  topGap=Control.EvalPair<double>(keyName,baseName,"TopGap");
  baseGap=Control.EvalPair<double>(keyName,baseName,"BaseGap");

  double D,W,H,L(RInner);
  nSegment=Control.EvalPair<size_t>(keyName,baseName,"NSegment");
  for(size_t i=0;i<nSegment;i++)
    {
      W=Control.EvalPair<double>(keyName,baseName,
				  StrFunc::makeString("Width",i+1));
      H=Control.EvalPair<double>(keyName,baseName,
				 StrFunc::makeString("Height",i+1));
      D=Control.EvalPair<double>(keyName,baseName,
				 StrFunc::makeString("Depth",i+1));
      if (i!=nSegment-1)
	{
	  L+=Control.EvalPair<double>(keyName,baseName,
				      StrFunc::makeString("Length",i+1));
	  length.push_back(L);
	}
      height.push_back(H);
      depth.push_back(D);
      width.push_back(W);
    }
  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat",baseName+"Mat");
  filled=Control.EvalPair<int>(keyName,baseName,"Filled");


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
  //  mainFC.createUnitVector(FC.getCentre(),-LU.getAxis(),FC.getZ());

  //   beamFC=mainFC;

  
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

  if (dividePlane)
    {
      const Geometry::Plane* PPtr=SMap.realPtr<Geometry::Plane>(dividePlane);
      if (dividePlane * PPtr->getNormal().dotProd(Y)<0.0)
	dividePlane*=-1;
      SMap.addMatch(guideIndex+1,dividePlane);
    }
  else
    ModelSupport::buildPlane(SMap,guideIndex+1,Origin,Y);    // Divider plane

  
  const attachSystem::FixedComp& mainFC=
    FixedGroup::getKey("Main");
  const attachSystem::FixedComp& beamFC=
    FixedGroup::getKey("Beam");

  const Geometry::Vec3D mainOrigin=mainFC.getCentre();
  const Geometry::Vec3D beamOrigin=beamFC.getCentre();

  
  const Geometry::Vec3D& bX=beamFC.getX();
  const Geometry::Vec3D& bZ=beamFC.getZ();
 

  SMap.addMatch(guideIndex+7,innerCyl);

  int GI(guideIndex);

  for(size_t i=0;i<nSegment;i++)
    {
      if (i!=nSegment-1)
	ModelSupport::buildCylinder(SMap,GI+57,mainOrigin,Z,length[i]);

      ModelSupport::buildPlane(SMap,GI+3,beamOrigin-bX*(width[i]/2.0),bX);
      ModelSupport::buildPlane(SMap,GI+4,beamOrigin+bX*(width[i]/2.0),bX);
      ModelSupport::buildPlane(SMap,GI+5,beamOrigin-bZ*depth[i],bZ);
      ModelSupport::buildPlane(SMap,GI+6,beamOrigin+bZ*height[i],bZ);

      ModelSupport::buildPlane
        (SMap,GI+13,beamOrigin-bX*(-sideGap+width[i]/2.0),bX);
      ModelSupport::buildPlane
        (SMap,GI+14,beamOrigin+bX*(-sideGap+width[i]/2.0),bX);
      ModelSupport::buildPlane(SMap,GI+15,beamOrigin-bZ*(-baseGap+depth[i]),bZ);
      ModelSupport::buildPlane(SMap,GI+16,beamOrigin+bZ*(-topGap+height[i]),bZ);

      GI+=50;
    }
  SMap.addMatch(GI+7,outerCyl);

  // Beamline :: 
  ModelSupport::buildPlane(SMap,guideIndex+1103,
			   beamOrigin-bX*(beamWidth/2.0),bX);
  ModelSupport::buildPlane(SMap,guideIndex+1104,
			   beamOrigin+bX*(beamWidth/2.0),bX);
  ModelSupport::buildPlane(SMap,guideIndex+1105,
			   beamOrigin-bZ*(beamHeight/2.0),bZ);
  ModelSupport::buildPlane(SMap,guideIndex+1106,
			   beamOrigin+bZ*(beamHeight/2.0),bZ);
  
  
  return;
}

const Geometry::Plane*
GuideItem::getPlane(const int SN) const
  /*!
    Access to specific plane
    \param SN :: Surface offset [without guideIndex]
    \return Plane Number
  */
{
  ELog::RegMethod RegA("GuideItem","getPlane");
  return SMap.realPtr<Geometry::Plane>(guideIndex+SN);
}


std::string
GuideItem::getEdgeStr(const GuideItem* GPtr) const
  /*!
    Given another GuideItem determine the end point collision string
    \param GPtr :: Other object Ptr [0 for none]
    \return Edge string (3 side)
   */
{
  ELog::RegMethod RegA("GuideItem","getEdgeStr");
  if (!GPtr) return "";
  const attachSystem::FixedComp& beamFC=FixedGroup::getKey("Beam");
  const Geometry::Vec3D beamOrigin=beamFC.getCentre();

  const Geometry::Plane* gP3=GPtr->getPlane(3);
  const Geometry::Plane* gP4=GPtr->getPlane(4);
  const Geometry::Plane* P3=getPlane(3);
  const Geometry::Plane* P4=getPlane(4);
  const Geometry::Plane* ZPlane=getPlane(6);
  
  const Geometry::Vec3D RptA=SurInter::getPoint(gP3,P4,ZPlane,beamOrigin);
  const Geometry::Vec3D RptB=SurInter::getPoint(gP4,P3,ZPlane,beamOrigin);
  const Geometry::Cylinder* CPtr=
    SMap.realPtr<Geometry::Cylinder>(innerCyl);

  if (CPtr->side(RptA))
    return GPtr->sideExclude(1);
  else if (CPtr->side(RptB))
    return GPtr->sideExclude(0);
  return "";
}

void
GuideItem::createObjects(Simulation& System,const GuideItem* GPtr)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
    \param GPtr :: Near neigbour
  */
{
  ELog::RegMethod RegA("GuideItem","createObjects");
  if (!active) return;
  
  const std::string edgeStr=getEdgeStr(GPtr);
  std::string Out;  

  int GI(guideIndex);
  for(size_t i=0;i<nSegment;i++)
    {
      // Outer layer
      if (i==0)
	{
	  Out=ModelSupport::getComposite(SMap,guideIndex,"1 7 3 -4 5 -6 -57");
	  Out+=edgeStr;
	}
      else 
	Out=ModelSupport::getComposite(SMap,GI,guideIndex,"1M 7 3 -4 5 -6 -57");

      if (!i)
	addOuterSurf("Inner",Out);
      else 
	addOuterUnionSurf("Outer",Out);



      // Add inner boundary
      Out+=ModelSupport::getComposite(SMap,GI," (-13:14:-15:16) ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
      addCell("Body",cellIndex-1);

      if (i==0)
	{
	  Out=ModelSupport::getComposite
	    (SMap,guideIndex,"1 7 13 -14 15 -16 -57");
	  Out+=edgeStr;
	}
      else 
	Out=ModelSupport::getComposite
	  (SMap,GI,guideIndex,"1M 7 13 -14 15 -16 -57");

      // Inner metal:
      if (!filled)
	Out+=ModelSupport::getComposite
	  (SMap,guideIndex,"(-1103:1104:-1105:1106) ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,mat,0.0,Out));
      
      if (filled) addCell("Void",cellIndex-1);
      addCell("Body",cellIndex-1);
      addCell("BodyMetal",cellIndex-1);
      
      GI+=50;
    }      
  // Inner void
  if (!filled)
    {
      Out=ModelSupport::getComposite(SMap,guideIndex,GI,
				     "1 7 -7M 1103 -1104 1105 -1106 ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
      
      setCell("Void",cellIndex-1);
    }

  return;
}

std::string
GuideItem::sideExclude(const size_t sideIndex) const
  /*!
    Determine the string to exclude on a front point touch
    \param sideIndex :: 0 / 1
    \return string
  */
{
  if (sideIndex==0)
    return ModelSupport::getComposite(SMap,guideIndex,"(-3 : -5 : 6)");
  
  return ModelSupport::getComposite(SMap,guideIndex,"(4 : -5 : 6)");
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

  MonteCarlo::LineIntersectVisit LI(beamOrigin,beamFC.getY());

  const Geometry::Cylinder* CPtr=
    SMap.realPtr<Geometry::Cylinder>(innerCyl);
  const Geometry::Cylinder* DPtr=
     SMap.realPtr<Geometry::Cylinder>(outerCyl);

  const Geometry::Vec3D beamEnter=
    LI.getPoint(CPtr,beamOrigin+bY*RInner);
  const Geometry::Vec3D beamExit=
    LI.getPoint(DPtr,beamOrigin+bY*ROuter);

  beamFC.setConnect(0,beamEnter,-bY);
  beamFC.setLinkSurf(0,-SMap.realSurf(guideIndex+7));
  beamFC.addBridgeSurf(0,SMap.realSurf(guideIndex+1));

  const int GI=50*static_cast<int>(nSegment)+guideIndex;
  beamFC.setConnect(1,beamExit,bY);
  beamFC.setLinkSurf(1,SMap.realSurf(GI+7));
  beamFC.addBridgeSurf(1,SMap.realSurf(guideIndex+1));
  if (!filled)
    {
      const Geometry::Vec3D MidPt((beamOrigin+bY*RInner+beamExit)/2.0);
      beamFC.setConnect(2,MidPt-bX*(beamWidth/2.0),-bX);
      beamFC.setConnect(3,MidPt+bX*(beamWidth/2.0),bX);
      beamFC.setConnect(4,MidPt-bZ*(beamWidth/2.0),-bZ);
      beamFC.setConnect(5,MidPt+bZ*(beamWidth/2.0),bZ);

      beamFC.setLinkSurf(2,-SMap.realSurf(guideIndex+1103));
      beamFC.setLinkSurf(3,SMap.realSurf(guideIndex+1104));
      beamFC.setLinkSurf(4,-SMap.realSurf(guideIndex+1105));
      beamFC.setLinkSurf(5,SMap.realSurf(guideIndex+1106));
    }

  /// TARGET CENTRE TRACKING:
  mainFC.setConnect(0,beamEnter-Geometry::Vec3D(0,0,beamEnter[2]),-bY);
  mainFC.setLinkSurf(0,-SMap.realSurf(guideIndex+7));
  mainFC.addBridgeSurf(0,SMap.realSurf(guideIndex+1));

  mainFC.setConnect(1,beamExit-Geometry::Vec3D(0,0,beamExit[2]),bY);
  mainFC.setLinkSurf(1,SMap.realSurf(GI+7));
  mainFC.addBridgeSurf(1,SMap.realSurf(guideIndex+1));
    

  return;
}

void
GuideItem::createAll(Simulation& System,
		     const attachSystem::FixedComp& FC,
		     const long int sideIndex,
		     const GuideItem* GPtr)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: side to used
    \param GPtr :: Previous Guide item for edge intersect
  */
{
  ELog::RegMethod RegA("GuideItem","createAll");
	    
  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System,GPtr);
  createLinks();
  insertObjects(System);              

  return;
}

}  // NAMESPACE essSystem
