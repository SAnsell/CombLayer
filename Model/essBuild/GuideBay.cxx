/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/GuideBay.cxx
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

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Quadratic.h"
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
#include "FixedRotate.h"
#include "FixedGroup.h"
#include "FixedRotateGroup.h"
#include "SurInter.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "ExternalCut.h"
#include "BaseMap.h"
#include "SurfMap.h"
#include "CellMap.h"
#include "GuideItem.h"
#include "GuideBay.h"


namespace essSystem
{

GuideBay::GuideBay(const std::string& Key,const size_t BN)  :
  attachSystem::FixedRotate(Key+std::to_string(BN),6),
  attachSystem::ContainedGroup("Inner","Outer"),
  attachSystem::CellMap(),
  baseKey(Key),bayNumber(BN),
  nItems(0),RInner(0.0),ROuter(0.0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

GuideBay::GuideBay(const GuideBay& A) : 
  attachSystem::FixedRotate(A),
  attachSystem::ContainedGroup(A),
  attachSystem::CellMap(A),
  baseKey(A.baseKey),bayNumber(A.bayNumber),
  viewAngle(A.viewAngle),innerHeight(A.innerHeight),
  innerDepth(A.innerDepth),height(A.height),depth(A.depth),
  midRadius(A.midRadius),mat(A.mat),nItems(A.nItems),
  RInner(A.RInner),ROuter(A.ROuter),GUnit(A.GUnit)
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
      attachSystem::FixedRotate::operator=(A);
      attachSystem::ContainedGroup::operator=(A);
      attachSystem::CellMap::operator=(A);
      viewAngle=A.viewAngle;
      innerHeight=A.innerHeight;
      innerDepth=A.innerDepth;
      height=A.height;
      depth=A.depth;
      midRadius=A.midRadius;
      mat=A.mat;
      nItems=A.nItems;
      RInner=A.RInner;
      ROuter=A.ROuter;
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
GuideBay::populate(const FuncDataBase& Control)
 /*!
   Populate all the variables
   \param Control :: Database to use
 */
{
  ELog::RegMethod RegA("GuideBay","populate");

  FixedRotate::populate(baseKey,Control);
  
  height=Control.EvalTail<double>(keyName,baseKey,"Height");
  depth=Control.EvalTail<double>(keyName,baseKey,"Depth");
  midRadius=Control.EvalTail<double>(keyName,baseKey,"MidRadius");
  viewAngle=Control.EvalTail<double>(keyName,baseKey,"ViewAngle")*M_PI/180.0;
  innerHeight=Control.EvalTail<double>(keyName,baseKey,"InnerHeight");
  innerDepth=Control.EvalTail<double>(keyName,baseKey,"InnerDepth");

  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat",baseKey+"Mat");

  nItems=Control.EvalTail<size_t>(keyName,baseKey,"NItems");
  return;
}

void
GuideBay::calcRadii()
  /*!
    Calc inner/oute radii
   */
{
  ELog::RegMethod RegA("GuideBay","calcRadii");
  
  // construct
  const HeadRule innerHR=getRule("innerCyl");
  const HeadRule outerHR=getRule("outerCyl");
  const Geometry::Cylinder* CInner=
    dynamic_cast<const Geometry::Cylinder*>(innerHR.primarySurface());
  const Geometry::Cylinder* COuter=
    dynamic_cast<const Geometry::Cylinder*>(outerHR.primarySurface());

  if (!CInner || !COuter)
    throw ColErr::InContainerError<std::string>
      ("Inner/Outer","inner/outer Cyl not cylinder");

  RInner=CInner->getRadius();
  ROuter=COuter->getRadius();

  return;
}
  
  
void
GuideBay::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("GuideBay","createSurface");
  
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);    // Divider plane


  // note Origin here is axpanded centre and only used for planes
  // it deals with the case of a bridge surface in expanded map
  makeExpandedSurf(SMap,"innerCyl",buildIndex+17,Origin,midRadius);

  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*innerDepth,Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*innerHeight,Z);
  ModelSupport::buildPlane(SMap,buildIndex+15,Origin-Z*depth,Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,Origin+Z*height,Z);
  
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

  ModelSupport::buildPlane(SMap,buildIndex+3,LPoint,LVec);  
  ModelSupport::buildPlane(SMap,buildIndex+4,RPoint,RVec);  

  // Link Points created here as access to variables

  const double midRad=(RInner+ROuter)/2.0;
  // Inner point 
  FixedComp::setConnect(0,Origin+Y*RInner,-Y);
  FixedComp::setLinkSurf(0,getRule("#innerCyl"));
  FixedComp::setConnect(1,Origin+Y*ROuter,Y);
  FixedComp::setLinkSurf(1,getRule("outerCyl"));
  FixedComp::setConnect(2,LPoint+LEdge*midRad,-LVec);
  FixedComp::setLinkSurf(2,-SMap.realSurf(buildIndex+3));
  FixedComp::setConnect(3,RPoint+REdge*midRad,RVec);
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+4));
  FixedComp::setConnect(4,Origin+Y*midRad-Z*depth,-Z);
  FixedComp::setLinkSurf(4,-SMap.realSurf(buildIndex+5));
  FixedComp::setConnect(5,Origin+Y*midRad+Z*height,-Z);
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+6));
  
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

  const HeadRule innerHR=getRule("innerCyl");
  const HeadRule outerHR=getComplementRule("outerCyl");

  HeadRule HR;
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -17 3 -4 5 -6");
  makeCell("Inner",System,cellIndex++,mat,0.0,HR*innerHR);
  addOuterSurf("Inner",HR);

  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 17 3 -4 15 -16");
  makeCell("Outer",System,cellIndex++,mat,0.0,HR*outerHR);
  addOuterSurf("Outer",HR);

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

  MonteCarlo::Object* AB=System.findObject(CellMap::getCell("Outer"));
  MonteCarlo::Object* BB=System.findObject(otherBay.CellMap::getCell("Outer"));
  
  if (!AB)
    throw ColErr::InContainerError<int>
      (CellMap::getCell("Outer"),"Outer cell not found");
  if (!BB)
    throw ColErr::InContainerError<int>
      (otherBay.CellMap::getCell("Outer"),"Outer cell not found");

  // Get radii:
    
  const double halfRadius=(RInner+ROuter)/2.0;
  // Keep positive direction of sense
  Geometry::Vec3D CPoint = (Origin+otherBay.getCentre())/2.0
    +Y*halfRadius;


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
  const int mat=AB->getMatID();
  const double temp=AB->getTemp();
  System.removeCell(AB->getName());
  System.removeCell(BB->getName());
  makeCell("Outer",System,cellIndex++,mat,temp,ARule);
  // NOW RESET names in cellMap:
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
    System.getObjectThrow<attachSystem::FixedComp>(modName+"Focus",
                                               "Focus unit not found");
  const std::string BL="G"+std::to_string(bayNumber)+"BLine"+modName;
  
  const long int lFocusIndex=(bayNumber==1) ? 2 : 3;
  const long int rFocusIndex=(bayNumber==1) ? 1 : 4;
  
  const GuideItem* GB(0);  // guides can intersect

  
  attachSystem::ContainedGroup* CG=
    (!wheelName.empty()) ?
    System.getObjectThrow<attachSystem::ContainedGroup>
    (wheelName,"Wheel unit not found") : 0;
    
  for(size_t i=0;i<nItems;i++)
    {
      const long int FI((i>=nItems/2) ? rFocusIndex : lFocusIndex);

      std::shared_ptr<GuideItem> GA(new GuideItem(BL,i+1));
      OR.addObject(GA);
      GA->setCutSurf("divider",SMap.realSurf(buildIndex+1));
      GA->copyCutSurf("innerCyl",*this,"innerCyl");
      GA->copyCutSurf("outerCyl",*this,"outerCyl");
      GA->copyCutSurf("wheel",*this,"wheel");

      GA->addInsertCell("Inner",getCell("Inner"));
      GA->addInsertCell("Outer",getCell("Outer"));

      if (GB && GB->isBuilt())
	GA->setNeighbour(GB);
      GA->createAll(System,*ModFC,FI);
      GUnit.push_back(GA);

      // Add wheel to inner cell if required
      if (GA->hasCell("BodyMetal"))
	GA->insertComponent(System,"Body",CG->getCC("Wheel"));
      
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
  calcRadii();
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);              
  return;
}

}  // NAMESPACE essSystem
