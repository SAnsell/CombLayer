/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   essBuild/GuideBay.cxx
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#include "ReadFunctions.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "SecondTrack.h"
#include "TwinComp.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "World.h"
#include "GuideItem.h"
#include "GuideBay.h"

namespace essSystem
{

 GuideBay::GuideBay(const std::string& Key,const size_t BN)  :
  attachSystem::ContainedGroup("Inner","Outer"),
  attachSystem::FixedComp(StrFunc::makeString(Key,BN),6),
  baseKey(Key),bayNumber(BN),
  bayIndex(ModelSupport::objectRegister::Instance().cell(keyName)),
  cellIndex(bayIndex+1),nItems(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

GuideBay::GuideBay(const GuideBay& A) : 
  attachSystem::ContainedGroup(A),attachSystem::FixedComp(A),
  baseKey(A.baseKey),bayNumber(A.bayNumber),bayIndex(A.bayIndex),
  cellIndex(A.cellIndex),xStep(A.xStep),yStep(A.yStep),
  zStep(A.zStep),xyAngle(A.xyAngle),zAngle(A.zAngle),
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
      attachSystem::FixedComp::operator=(A);
      cellIndex=A.cellIndex;
      xStep=A.xStep;
      yStep=A.yStep;
      zStep=A.zStep;
      xyAngle=A.xyAngle;
      zAngle=A.zAngle;
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
GuideBay::populate(const Simulation& System)
 /*!
   Populate all the variables
   \param System :: Simulation to use
 */
{
  ELog::RegMethod RegA("GuideBay","populate");
  
  const FuncDataBase& Control=System.getDataBase();

  xStep=Control.EvalPair<double>(keyName,baseKey,"XStep");
  yStep=Control.EvalPair<double>(keyName,baseKey,"YStep");
  zStep=Control.EvalPair<double>(keyName,baseKey,"ZStep");
  xyAngle=Control.EvalPair<double>(keyName,baseKey,"XYangle");
  zAngle=Control.EvalPair<double>(keyName,baseKey,"Zangle");
  
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
GuideBay::createUnitVector(const attachSystem::FixedComp& FC)
  /*!
    Create the unit vectors
    \param FC :: Linked object
  */
{
  ELog::RegMethod RegA("GuideBay","createUnitVector");

  FixedComp::createUnitVector(FC);
  applyShift(xStep,yStep,zStep);
  applyAngleRotate(xyAngle,zAngle);

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
  addOuterSurf("Inner",Out);
  System.addCell(MonteCarlo::Qhull(cellIndex++,mat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,bayIndex,"1 17 -27 3 -4 15 -16 ");
  addOuterSurf("Outer",Out);
  System.addCell(MonteCarlo::Qhull(cellIndex++,mat,0.0,Out));
  
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
GuideBay::createGuideItems(Simulation& System)
  /*!
    Create the guide items
    \param System :: Simulation to link
  */
{
  ELog::RegMethod RegA("GuideBay","createGuideItems");

  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  const std::string BL=StrFunc::makeString("G",bayNumber)+"BLine";

  const int dPlane=SMap.realSurf(bayIndex+1);
  for(size_t i=0;i<nItems;i++)
    {
      std::shared_ptr<GuideItem> GA(new GuideItem(BL,i+1));
      GA->setCylBoundary(dPlane,innerCyl,outerCyl);
      GA->addInsertCell("Inner",bayIndex+1);
      GA->addInsertCell("Outer",bayIndex+2);
      if (i)
	GA->createAll(System,*this,0,GUnit[i-1].get());
      else
	GA->createAll(System,*this,0,0);

      GUnit.push_back(GA);
      OR.addObject(GUnit.back());

      
    }
  return;
}
  

void
GuideBay::createAll(Simulation& System,
		    const attachSystem::FixedComp& FC)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
  */
{
  ELog::RegMethod RegA("GuideBay","createAll");

  populate(System);
  createUnitVector(FC);
  createSurfaces();
  createObjects(System);
  createLinks();
  createGuideItems(System);
  insertObjects(System);              

  return;
}

}  // NAMESPACE ts1System
