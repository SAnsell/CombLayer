/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/GuideBay.cxx
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
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "inputParam.h"
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
#include "SurInter.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "World.h"
#include "GuideItem.h"
#include "GuideBay.h"

#include "AttachSupport.h"

namespace essSystem
{

GuideBay::GuideBay(const std::string& Key,const size_t BN)  :
  attachSystem::ContainedGroup("Inner","Outer"),
  attachSystem::FixedOffset(StrFunc::makeString(Key,BN),6),
  attachSystem::CellMap(),
  baseKey(Key),bayNumber(BN),
  bayIndex(ModelSupport::objectRegister::Instance().cell(keyName)),
  cellIndex(bayIndex+1),nItems(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

GuideBay::GuideBay(const GuideBay& A) : 
  attachSystem::ContainedGroup(A),attachSystem::FixedOffset(A),
  attachSystem::CellMap(A),
  baseKey(A.baseKey),bayNumber(A.bayNumber),bayIndex(A.bayIndex),
  cellIndex(A.cellIndex),
  viewAngle(A.viewAngle),innerHeight(A.innerHeight),
  innerDepth(A.innerDepth),height(A.height),depth(A.depth),
  midRadius(A.midRadius),mat(A.mat),nItems(A.nItems),
  innerCyl(A.innerCyl),outerCyl(A.outerCyl),GUnit(A.GUnit)
  /*!
    Copy constructor
    \param A :: GuideBay to copy
  */
{}

GuideBay&
GuideBay::operator=(const GuideBay& A)
  /*!
    Assignment operator
    \param A :: GuideBay to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedGroup::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      attachSystem::CellMap::operator=(A);
      cellIndex=A.cellIndex;
      viewAngle=A.viewAngle;
      innerHeight=A.innerHeight;
      innerDepth=A.innerDepth;
      height=A.height;
      depth=A.depth;
      midRadius=A.midRadius;
      mat=A.mat;
      nItems=A.nItems;
      innerCyl=A.innerCyl;
      outerCyl=A.outerCyl;
      GUnit=A.GUnit;
    }
  return *this;
}


GuideBay::~GuideBay() 
  /*!
    Destructor
  */
{}

void
GuideBay::setCylBoundary(const int A,const int B)
  /*!
    Set inside and outside cylinder cuts
    \todo Move to a headrule system
    \param A :: Inner cylinder 
    \param B :: Outer cylinder
   */
{
  innerCyl=std::abs(A);
  outerCyl=std::abs(B);
  return;
}
  
void
GuideBay::populate(const FuncDataBase& Control)
 /*!
   Populate all the variables
   \param Control :: Database to use
 */
{
  ELog::RegMethod RegA("GuideBay","populate");

  FixedOffset::populate(baseKey,Control);
  
  height=Control.EvalPair<double>(keyName,baseKey,"Height");
  depth=Control.EvalPair<double>(keyName,baseKey,"Depth");
  midRadius=Control.EvalPair<double>(keyName,baseKey,"MidRadius");
  viewAngle=Control.EvalPair<double>(keyName,baseKey,"ViewAngle")*M_PI/180.0;
  innerHeight=Control.EvalPair<double>(keyName,baseKey,"InnerHeight");
  innerDepth=Control.EvalPair<double>(keyName,baseKey,"InnerDepth");

  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat",baseKey+"Mat");

  nItems=Control.EvalPair<size_t>(keyName,baseKey,"NItems");
  return;
}
  
  
void
GuideBay::createUnitVector(const attachSystem::FixedComp& FC,
                           const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Linked object
    \param sideIndex :: linkPoint index
  */
{
  ELog::RegMethod RegA("GuideBay","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();

  return;
}
  
void
GuideBay::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("GuideBay","createSurface");

  ModelSupport::buildPlane(SMap,bayIndex+1,Origin,Y);    // Divider plane

  SMap.addMatch(bayIndex+7,innerCyl);
  SMap.addMatch(bayIndex+27,outerCyl);
  const Geometry::Cylinder* CPtr=
    SMap.realPtr<Geometry::Cylinder>(bayIndex+7);
  const double RInner=CPtr->getRadius();
  ModelSupport::buildCylinder(SMap,bayIndex+17,Origin,Z,RInner+midRadius);
  
  ModelSupport::buildPlane(SMap,bayIndex+5,Origin-Z*innerDepth,Z);
  ModelSupport::buildPlane(SMap,bayIndex+6,Origin+Z*innerHeight,Z);
  ModelSupport::buildPlane(SMap,bayIndex+15,Origin-Z*depth,Z);
  ModelSupport::buildPlane(SMap,bayIndex+16,Origin+Z*height,Z);
  
  // Calculate the point the cylinder intersects:
  Geometry::Vec3D LVec(X);
  Geometry::Vec3D RVec(X);
  const Geometry::Quaternion Qxy=
    Geometry::Quaternion::calcQRot(viewAngle/2.0,Z);
  Qxy.rotate(LVec);
  Qxy.invRotate(RVec);

  const Geometry::Vec3D LEdge=Z*LVec;
  const Geometry::Vec3D REdge=Z*RVec;

  const Geometry::Vec3D LPoint=Origin-X*(RInner*sin(viewAngle/2.0))+
    Y*(RInner*cos(viewAngle/2.0));
  const Geometry::Vec3D RPoint=Origin+X*(RInner*sin(viewAngle/2.0))+
    Y*(RInner*cos(viewAngle/2.0));

  ModelSupport::buildPlane(SMap,bayIndex+3,LPoint,LVec);  
  ModelSupport::buildPlane(SMap,bayIndex+4,RPoint,RVec);  

  // Link Points created here as access to variables
  const Geometry::Cylinder* DPtr=
    SMap.realPtr<Geometry::Cylinder>(bayIndex+27);
  const double ROuter=DPtr->getRadius();

  const double midRad=(RInner+ROuter)/2.0;
  // Inner point 
  FixedComp::setConnect(0,Origin+Y*RInner,-Y);
  FixedComp::setLinkSurf(0,-SMap.realSurf(bayIndex+7));
  FixedComp::setConnect(1,Origin+Y*ROuter,Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(bayIndex+27));
  FixedComp::setConnect(2,LPoint+LEdge*midRad,-LVec);
  FixedComp::setLinkSurf(2,-SMap.realSurf(bayIndex+3));
  FixedComp::setConnect(3,RPoint+REdge*midRad,RVec);
  FixedComp::setLinkSurf(3,SMap.realSurf(bayIndex+4));
  FixedComp::setConnect(4,Origin+Y*midRad-Z*depth,-Z);
  FixedComp::setLinkSurf(4,-SMap.realSurf(bayIndex+5));
  FixedComp::setConnect(5,Origin+Y*midRad+Z*height,-Z);
  FixedComp::setLinkSurf(5,SMap.realSurf(bayIndex+6));
  
  return;
}

void
GuideBay::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("GuideBay","createObjects");

  std::string Out;
  Out=ModelSupport::getComposite(SMap,bayIndex,"1 7 -17 3 -4 5 -6 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,mat,0.0,Out));
  CellMap::addCell("Inner",cellIndex-1);
  addOuterSurf("Inner",Out);
  
  Out=ModelSupport::getComposite(SMap,bayIndex,"1 17 -27 3 -4 15 -16 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,mat,0.0,Out));
  CellMap::addCell("Outer",cellIndex-1);
  addOuterSurf("Outer",Out);

  return;
}

void
GuideBay::createLinks()
  /*!
    Create all the linkes [OutGoing]
  */
{
  ELog::RegMethod RegA("GuideBay","createLinks");
  
  return;
}

void
GuideBay::outerMerge(Simulation& System,
		     GuideBay& otherBay)
  /*!
    Merge the outer component of the guidebay to the neighbouring 
    bay.
    \param System :: simulaiton to use
    \param otherBay :: Other guidebase (neighbour)
  */
{
  ELog::RegMethod RegA("GuideBay","outerMerge");

  MonteCarlo::Qhull* AB=System.findQhull(CellMap::getCell("Outer"));
  MonteCarlo::Qhull* BB=System.findQhull(otherBay.CellMap::getCell("Outer"));
  
  if (!AB)
    throw ColErr::InContainerError<int>
      (CellMap::getCell("Outer"),"Outer cell not found");
  if (!BB)
    throw ColErr::InContainerError<int>
      (otherBay.CellMap::getCell("Outer"),"Outer cell not found");
  // Get radii:
  const Geometry::Cylinder* CInner=
    SMap.realPtr<Geometry::Cylinder>(bayIndex+7);
  const Geometry::Cylinder* COuter=
    SMap.realPtr<Geometry::Cylinder>(bayIndex+27);
  const double midRadius=
    (CInner->getRadius()+COuter->getRadius())/2.0;

  // Keep positive direction of sense
  Geometry::Vec3D CPoint = (Origin+otherBay.getCentre())/2.0
    +Y*midRadius;


  HeadRule ARule=AB->getHeadRule();
  HeadRule BRule=BB->getHeadRule();
  const std::pair<Geometry::Vec3D,int> APoint=
    SurInter::interceptRule(ARule,CPoint,Z);
  const std::pair<Geometry::Vec3D,int> BPoint=
    SurInter::interceptRule(BRule,CPoint,Z);

  if (APoint.second && BPoint.second)
    {
      ARule.removeUnsignedItems(APoint.second);
      BRule.removeUnsignedItems(BPoint.second);
      ARule.addIntersection(BRule);
      //      ARule.removeCommon();      
    }

  // Delete common stuff and build object
  const int mat=AB->getMat();
  const double temp=AB->getTemp();
  System.removeCell(AB->getName());
  System.removeCell(BB->getName());
  System.addCell(MonteCarlo::Qhull(cellIndex++,mat,temp,
				   ARule.display()));
  // NOW RESET names in cellMap:
  setCell("Outer",cellIndex-1);
  otherBay.setCell("Outer",cellIndex-1);
  return;
}
  
  
void
GuideBay::createGuideItems(Simulation& System,
                           const std::string& modName,
			   const std::string& wheelName)
  /*!
    Create the guide items
    \param System :: Simulation to link
    \param modName :: Moderator name (top/low)
    \param wheelName :: Wheel object [if used]
  */
{
  ELog::RegMethod RegA("GuideBay","createGuideItems");

  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  const attachSystem::FixedComp* ModFC=
    OR.getObjectThrow<attachSystem::FixedComp>(modName+"Focus",
                                               "Focus unit not found");
  const std::string BL=StrFunc::makeString("G",bayNumber)+"BLine"+modName;
  
  const long int lFocusIndex=(bayNumber==1) ? 2 : 3;
  const long int rFocusIndex=(bayNumber==1) ? 1 : 4;
  
  const int dPlane=SMap.realSurf(bayIndex+1);

  const GuideItem* GB(0);  // guides can intersect

  
  attachSystem::ContainedGroup* CG=
    (!wheelName.empty()) ?
    OR.getObjectThrow<attachSystem::ContainedGroup>
    (wheelName,"Wheel unit not found") : 0;
    
  for(size_t i=0;i<nItems;i++)
    {
      const long int FI((i>=nItems/2) ? rFocusIndex : lFocusIndex);
      std::shared_ptr<GuideItem> GA(new GuideItem(BL,i+1));
      GA->setCylBoundary(dPlane,innerCyl,outerCyl);

      GA->addInsertCell("Inner",getCell("Inner"));
      GA->addInsertCell("Outer",getCell("Outer"));

      GA->createAll(System,*ModFC,FI,GB);
      GUnit.push_back(GA);
      OR.addObject(GUnit.back());

      // Add wheel to inner cell if required
      if (GA->hasItem("BodyMetal"))
	{
	  GA->insertComponent(System,"Body",CG->getCC("Wheel"));
     //	  attachSystem::addToInsertSurfCtrl(System,*GA,"BodyMetal",
     //					    CG->getCC("Wheel"));
	}
      
      GB=GA.get();
    }
  
  return;
}
  

void
GuideBay::createAll(Simulation& System,
		    const attachSystem::FixedComp& FC,
                    const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC ::  Central origin
    \param sideIndex ::  side link point
  */
{
  ELog::RegMethod RegA("GuideBay","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);              
  return;
}

}  // NAMESPACE essSystem
