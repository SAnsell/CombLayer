/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   essBuild/GuideItem.cxx
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
#include "Vec3D.h"
#include "Triple.h"
#include "NRange.h"
#include "NList.h"
#include "Tally.h"
#include "Quaternion.h"
#include "localRotate.h"
#include "masterRotate.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfEqual.h"
#include "surfDivide.h"
#include "surfDIter.h"
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
#include "inputParam.h"
#include "ReadFunctions.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "KGroup.h"
#include "Source.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "generateSurf.h"
#include "chipDataStore.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "SecondTrack.h"
#include "TwinComp.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "World.h"
#include "GuideItem.h"

namespace essSystem
{

GuideItem::GuideItem(const std::string& Key,const size_t Index)  :
  attachSystem::ContainedGroup("Inner","Outer"),
  attachSystem::TwinComp(StrFunc::makeString(Key,Index),6),
  baseName(Key),
  guideIndex(ModelSupport::objectRegister::Instance().cell(keyName)),
  cellIndex(guideIndex+1),innerCyl(0),outerCyl(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
    \param Index :: Index of guide unit
  */
{}

GuideItem::GuideItem(const GuideItem& A) : 
  attachSystem::ContainedGroup(A),attachSystem::TwinComp(A),
  baseName(A.baseName),guideIndex(A.guideIndex),
  cellIndex(A.cellIndex),xStep(A.xStep),yStep(A.yStep),
  zStep(A.zStep),xyAngle(A.xyAngle),zAngle(A.zAngle),
  beamXStep(A.beamXStep),beamZStep(A.beamZStep),
  beamXYAngle(A.beamXYAngle),beamZAngle(A.beamZAngle),
  beamWidth(A.beamWidth),beamHeight(A.beamHeight),
  nSegment(A.nSegment),height(A.height),width(A.width),
  length(A.length),mat(A.mat),innerCyl(A.innerCyl),
  outerCyl(A.outerCyl),RInner(A.RInner),ROuter(A.ROuter)
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
      attachSystem::TwinComp::operator=(A);
      cellIndex=A.cellIndex;
      xStep=A.xStep;
      yStep=A.yStep;
      zStep=A.zStep;
      xyAngle=A.xyAngle;
      zAngle=A.zAngle;
      beamXStep=A.beamXStep;
      beamZStep=A.beamZStep;
      beamXYAngle=A.beamXYAngle;
      beamZAngle=A.beamZAngle;
      beamWidth=A.beamWidth;
      beamHeight=A.beamHeight;
      nSegment=A.nSegment;
      height=A.height;
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
GuideItem::populate(const Simulation& System)
 /*!
   Populate all the variables
   \param System :: Simulation to use
 */
{
  ELog::RegMethod RegA("GuideItem","populate");
  
  const FuncDataBase& Control=System.getDataBase();
  
  xStep=Control.EvalPair<double>(keyName,baseName,"XStep");
  yStep=Control.EvalPair<double>(keyName,baseName,"YStep");
  zStep=Control.EvalPair<double>(keyName,baseName,"ZStep");
  xyAngle=Control.EvalPair<double>(keyName,baseName,"XYangle");
  zAngle=Control.EvalPair<double>(keyName,baseName,"Zangle");

  beamXYAngle=Control.EvalPair<double>(keyName,baseName,"BeamXYAngle");
  beamZAngle=Control.EvalPair<double>(keyName,baseName,"BeamZAngle");
  beamXStep=Control.EvalPair<double>(keyName,baseName,"BeamXStep");
  beamZStep=Control.EvalPair<double>(keyName,baseName,"BeamZStep");

  beamHeight=Control.EvalPair<double>(keyName,baseName,"BeamHeight");
  beamWidth=Control.EvalPair<double>(keyName,baseName,"BeamWidth");

  double W,H,L(RInner);
  nSegment=Control.EvalPair<size_t>(keyName,baseName,"NSegment");
  for(size_t i=0;i<nSegment;i++)
    {
      W=Control.EvalPair<double>(keyName,baseName,
				  StrFunc::makeString("Width",i+1));
      H=Control.EvalPair<double>(keyName,baseName,
				 StrFunc::makeString("Height",i+1));
      if (i!=nSegment-1)
	{
	  L+=Control.EvalPair<double>(keyName,baseName,
				      StrFunc::makeString("Length",i+1));
	  length.push_back(L);
	}
      height.push_back(H);
      width.push_back(W);
    }
  mat=Control.EvalPair<int>(keyName,baseName,"Mat");

  return;
}
  
void
GuideItem::createUnitVector(const attachSystem::FixedComp& FC,
			    const size_t sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Linked object
    \param sideIndex :: Mid-point of inner bay 
  */
{
  ELog::RegMethod RegA("GuideItem","createUnitVector");
  
  const attachSystem::LinkUnit& LU=FC.getLU(sideIndex);
  FixedComp::createUnitVector(FC.getCentre(),-LU.getAxis(),FC.getZ());
  applyShift(xStep,yStep,zStep);
  applyAngleRotate(xyAngle,zAngle);
  return;
}

void
GuideItem::calcBeamLineTrack(const attachSystem::FixedComp& FC)
  /*!
    Calculate the beamline values
    \param FC :: Fixed component for object centre
   */
{
  ELog::RegMethod RegA("GuideItem","calcBeamLineTrack");

  bX=X;
  bY=Y;
  bZ=Z;
  // Need to calculate impact point of beamline:
  const double yShift=sqrt(RInner*RInner-beamXStep*beamXStep)-RInner;
  bEnter=FC.getCentre()+X*beamXStep+Y*yShift+Z*beamZStep;
  applyBeamAngleRotate(beamXYAngle,beamZAngle);
  MonteCarlo::LineIntersectVisit LI(bEnter,bY);

  const Geometry::Cylinder* DPtr=
    SMap.realPtr<Geometry::Cylinder>(outerCyl);
  bExit=LI.getPoint(DPtr,bEnter);
  return;
}
  
void
GuideItem::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("GuideItem","createSurface");

  ModelSupport::buildPlane(SMap,guideIndex+1,Origin,Y);    // Divider plane

  SMap.addMatch(guideIndex+1,dividePlane);
  SMap.addMatch(guideIndex+7,innerCyl);

  int GI(guideIndex);

  for(size_t i=0;i<nSegment;i++)
    {
      if (i!=nSegment-1)
	ModelSupport::buildCylinder(SMap,GI+17,Origin,Z,length[i]);

      ModelSupport::buildPlane(SMap,GI+3,Origin-X*(width[i]/2.0),X);
      ModelSupport::buildPlane(SMap,GI+4,Origin+X*(width[i]/2.0),X);
      ModelSupport::buildPlane(SMap,GI+5,Origin-Z*(height[i]/2.0),Z);
      ModelSupport::buildPlane(SMap,GI+6,Origin+Z*(height[i]/2.0),Z);
      GI+=10;
    }
  SMap.addMatch(GI+7,outerCyl);

  // Beamline :: 
  ModelSupport::buildPlane(SMap,guideIndex+103,bEnter-bX*(beamWidth/2.0),bX);
  ModelSupport::buildPlane(SMap,guideIndex+104,bEnter+bX*(beamWidth/2.0),bX);
  ModelSupport::buildPlane(SMap,guideIndex+105,bEnter-bZ*(beamHeight/2.0),bZ);
  ModelSupport::buildPlane(SMap,guideIndex+106,bEnter+bZ*(beamHeight/2.0),bZ);
  
  
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
    Given another GuideItem determine the end point collision stirng
    \param GPtr :: Other object Ptr [0 for none]
    \return Edge string (3 side)
   */
{
  ELog::RegMethod RegA("GuideItem","getEdgeStr");
  if (!GPtr) return "";

  const Geometry::Plane* gP3=GPtr->getPlane(3);
  const Geometry::Plane* gP4=GPtr->getPlane(4);
  const Geometry::Plane* P3=getPlane(3);
  const Geometry::Plane* P4=getPlane(4);
  const Geometry::Plane* ZPlane=getPlane(6);
  
  const Geometry::Vec3D RptA=SurInter::getPoint(gP3,P4,ZPlane,bEnter);
  const Geometry::Vec3D RptB=SurInter::getPoint(gP4,P3,ZPlane,bEnter);
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
    \param  GPtr :: Near neigbour
  */
{
  ELog::RegMethod RegA("GuideItem","createObjects");

  const std::string edgeStr=
    getEdgeStr(GPtr);
  std::string Out;  


  int GI(guideIndex);
  for(size_t i=0;i<nSegment;i++)
    {
      if (i==0)
	{
	  Out=ModelSupport::getComposite(SMap,guideIndex,"1 7 3 -4 5 -6 -17");
	  Out+=edgeStr;
	}
      else if (i!=nSegment-1)
	Out=ModelSupport::getComposite(SMap,GI-10,guideIndex,
				       "1M 17 13 -14 15 -16 -27");
      else 
	Out=ModelSupport::getComposite(SMap,GI-10,guideIndex,
				       "1M 17 13 -14 15 -16 -27");
      if (!i)
	addOuterSurf("Inner",Out);
      else 
	addOuterUnionSurf("Outer",Out);

      Out+=ModelSupport::getComposite(SMap,guideIndex,"(-103:104:-105:106) ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,mat,0.0,Out));
      GI+=10;
    }      
  // Inner void  
  Out=ModelSupport::getComposite(SMap,guideIndex,GI,
				 "1 7 -7M 103 -104 105 -106 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  //  addBoundarySurf(Out);

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
  */
{
  ELog::RegMethod RegA("GuideItem","createLinks");
  // Beamline :: 
  FixedComp::setConnect(0,bEnter,-bY);
  FixedComp::setLinkSurf(0,-SMap.realSurf(guideIndex+7));
  const int GI=10*static_cast<int>(nSegment);
  FixedComp::setConnect(1,bExit,bY);
  FixedComp::setLinkSurf(1,SMap.realSurf(GI+7));
  FixedComp::addLinkSurf(1,SMap.realSurf(GI+1));

  FixedComp::setConnect(2,bEnter-bX*(beamWidth/2.0)+
			bY*((RInner+ROuter)/2.0),-bX);
  FixedComp::setLinkSurf(2,-SMap.realSurf(guideIndex+103));
  FixedComp::setConnect(3,bEnter+bX*(beamWidth/2.0)+
			bY*((RInner+ROuter)/2.0),bX);
  FixedComp::setLinkSurf(3,SMap.realSurf(guideIndex+104));
  FixedComp::setConnect(4,bEnter-bZ*(beamHeight/2.0)+
			bY*((RInner+ROuter)/2.0),-bZ);
  FixedComp::setLinkSurf(4,-SMap.realSurf(guideIndex+105));
  FixedComp::setConnect(5,bEnter+bZ*(beamHeight/2.0)+
			bY*((RInner+ROuter)/2.0),bZ);
  FixedComp::setLinkSurf(5,SMap.realSurf(guideIndex+106));

  return;
}

void
GuideItem::createAll(Simulation& System,
		     const attachSystem::FixedComp& FC,
		     const size_t sideIndex,
		     const GuideItem* GPtr)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: side to used
  */
{
  ELog::RegMethod RegA("GuideItem","createAll");

  populate(System);
  createUnitVector(FC,sideIndex);
  calcBeamLineTrack(FC);
  createSurfaces();
  createObjects(System,GPtr);
  createLinks();
  insertObjects(System);              

  return;
}

}  // NAMESPACE ts1System
