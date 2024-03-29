/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/WedgeItem.cxx
 *
 * Copyright (c) 2004-2023 by Konstantin Batkov
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
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "Exception.h"
#include "Surface.h"
#include "surfRegister.h"
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
#include "FixedRotate.h"
#include "FixedRotateUnit.h"
#include "ContainedComp.h"
#include "ExternalCut.h"
#include "SurInter.h"
#include "WedgeItem.h"

namespace essSystem
{

WedgeItem::WedgeItem(const std::string& Key,const size_t Index)  :
  attachSystem::FixedRotateUnit(Key+std::to_string(Index),6),
  attachSystem::ContainedComp(),
  attachSystem::ExternalCut()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
    \param Index :: Offset index
  */
{}

WedgeItem::WedgeItem(const WedgeItem& A) : 
  attachSystem::FixedRotateUnit(A),
  attachSystem::ContainedComp(A),
  attachSystem::ExternalCut(A),
  length(A.length),baseWidth(A.baseWidth),
  mat(A.mat)
  /*!
    Copy constructor
    \param A :: WedgeItem to copy
  */
{}

WedgeItem&
WedgeItem::operator=(const WedgeItem& A)
  /*!
    Assignment operator
    \param A :: WedgeItem to copy
    \return *this
  */
{
  if (this!=&A)
    {

      attachSystem::FixedRotate::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      attachSystem::ExternalCut::operator=(A);
      length=A.length;
      baseWidth=A.baseWidth;
      mat=A.mat;
    }
  return *this;
}

WedgeItem::~WedgeItem() 
  /*!
    Destructor
  */
{}

double
WedgeItem::getFixedXYAngle(const double angle) const
  /*!
    Recalculate the XYAngle used by engineers (measured from Oy towards Ox
    with respect to (0,0)) to the wedge XYAngle (measured from Oy
    towards -Ox with respect to the wedge's focal point (XStep, YStep)
    \param angle :: engineering angle [deg]
    Return the wedge XYAngle [deg]
  */
{
  ELog::RegMethod RegA("WedgeItem","getFixedXYAngle");

  const double arad = angle*M_PI/180;
  const double R = 200;

  double beta = std::atan2(R*std::cos(arad)-xStep,  R*std::sin(arad)-yStep);
  beta *= -180.0/M_PI;

  return beta;
}
  
void
WedgeItem::populate(const FuncDataBase& Control)
 /*!
   Populate all the variables
   \param Control :: FuncDataBase to use
 */
{
  ELog::RegMethod RegA("WedgeItem","populate");

  FixedRotate::populate(Control);

  length=Control.EvalVar<double>(keyName+"Length");
  baseWidth=Control.EvalVar<double>(keyName+"BaseWidth");
  tipWidth=Control.EvalVar<double>(keyName+"TipWidth");

  theta=Control.EvalVar<double>(keyName+"Theta");
  zAngle = getFixedXYAngle(theta);

  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");

  return;
}
  
  
void
WedgeItem::createSurfaces(const attachSystem::FixedComp& FC,
                          const long int baseLinkPt)
  /*!
    Create All the surfaces
    \param FC :: FixedComp for outer surface
    \param baseLinkPt :: Outer surface
  */
{
  ELog::RegMethod RegA("WedgeItem","createSurface");

  ELog::EM<<"CREATEA"<<ELog::endDiag;
  const HeadRule cylHR=getNonDivideRule("OuterCyl");
  outerCyl=dynamic_cast<const Geometry::Cylinder*>(cylHR.primarySurface());
  if (!outerCyl)
    throw ColErr::InContainerError<std::string>
      ("OuterCyl","OuterCyl not found");
  
  // we assume that baseSurf is cylinder
  //  outerCyl = SMap.realPtr<Geometry::Cylinder>
  //    (FC.getLinkSurf(baseLinkPt));

  // divider:
  const Geometry::Plane *pZ =
    ModelSupport::buildPlane(SMap,buildIndex+5,Origin,Z);

  // aux planes:
  const Geometry::Vec3D nearPt(Origin+Y*outerCyl->getRadius());
  const Geometry::Plane *p103 = ModelSupport::buildPlane(SMap,buildIndex+103,
							 Origin-X*baseWidth/2.0,X);
  const Geometry::Plane *p104 = ModelSupport::buildPlane(SMap,buildIndex+104,
							 Origin+X*baseWidth/2.0,X);

  // inclination of side plane with respect to Y-axis
  const double alpha = std::atan((baseWidth-tipWidth)/2.0/length) * 180.0/M_PI;

  // points on the outerCyl surface
  const Geometry::Vec3D A = SurInter::getPoint(p103,outerCyl,pZ,nearPt);
  const Geometry::Vec3D B = SurInter::getPoint(p104,outerCyl,pZ,nearPt);

  ModelSupport::buildPlaneRotAxis(SMap,buildIndex+3,A,X,Z,alpha);
  ModelSupport::buildPlaneRotAxis(SMap,buildIndex+4,B,X,Z,-alpha);

  // aux plane between points AB
  const Geometry::Plane *pAB=
    ModelSupport::buildPlane(SMap,buildIndex+101,(A+B)/2,Y);

  ModelSupport::buildShiftedPlane(SMap,buildIndex+1,pAB,-length);
  
  return;
}

void
WedgeItem::createObjects(Simulation& System,
			 const attachSystem::FixedComp& FC,
			 const long int baseLinkPt,
			 const attachSystem::FixedComp& FL,
			 const long int topLinkPt,
                         const long int bottomLinkPt)
  /*!
    Adds the all the components
    \param System :: Simulation item
    \param FC :: Central origin
    \param baseLinkPt :: base surface of the wedge
    \param FL :: Flight line the wedge is inserted to
    \param topLinkPt :: top link surface
    \param bottomLinkPt :: bottom link surface

  */
{
  ELog::RegMethod RegA("WedgeItem","createObjects");

  HeadRule HR;

  HR=ModelSupport::getHeadRule(SMap, buildIndex,"1 3 -4")*
    FC.getFullRule(baseLinkPt)*
    FL.getFullRule(topLinkPt)*
    FL.getFullRule(bottomLinkPt);
  System.addCell(cellIndex++,mat,0.0,HR);
  
  addOuterSurf(HR);
  return;
}

void
WedgeItem::createLinks()
  /*!
    Create all the linkes [OutGoing]
  */
{
  ELog::RegMethod RegA("WedgeItem","createLinks");

  const Geometry::Vec3D nearPt(Origin+Y*outerCyl->getRadius());
  const Geometry::Plane *pX = ModelSupport::buildPlane(SMap, buildIndex+1003,
						       Origin, X);
  const Geometry::Plane *pZ = SMap.realPtr<Geometry::Plane>(buildIndex+5);
  const Geometry::Plane *pBC = SMap.realPtr<Geometry::Plane>(buildIndex+1);

  const Geometry::Plane *p3 = SMap.realPtr<Geometry::Plane>(buildIndex+3);
  const Geometry::Plane *p4 = SMap.realPtr<Geometry::Plane>(buildIndex+4);

  const Geometry::Vec3D pt0 = SurInter::getPoint(pX, pZ, outerCyl, nearPt);

  // points on the edges of WedgeItem
  const Geometry::Vec3D A = SurInter::getPoint(p4, outerCyl, pZ, nearPt);
  const Geometry::Vec3D B = SurInter::getPoint(p3, outerCyl, pZ, nearPt);
  const Geometry::Vec3D C = SurInter::getPoint(pZ, p4, pBC, nearPt);
  const Geometry::Vec3D D = SurInter::getPoint(pZ, p3, pBC, nearPt);

  FixedComp::setConnect(0, pt0, Y);
  FixedComp::setLinkSurf(0, outerCyl->getName());

  FixedComp::setConnect(1, (C+D)/2, -Y);
  FixedComp::setLinkSurf(1, -pBC->getName());

  FixedComp::setConnect(2, (A+C)/2, -p4->getNormal());
  FixedComp::setLinkSurf(2, p4->getName());
  
  FixedComp::setConnect(3, (B+D)/2, p3->getNormal());
  FixedComp::setLinkSurf(3, -p3->getName());

  return;
}


void
WedgeItem::createAll(Simulation& System,
		     const attachSystem::FixedComp& FC,
                     const long int sideIndex,
		     const long int baseLinkPt,
		     const attachSystem::FixedComp& FL,
		     const long int topLinkPt,
                     const long int bottomLinkPt)
  /*!
    Generic function to create everything
    \param System :: Simulation item

    \param FC :: Central origin
    \param baseLinkPt :: base surface of the wedge
    \param FL :: Flight line the wedge is inserted to
    \param topLinkPt :: top link surface
    \param bottomLinkPt :: bottom link surface
  */
{
  ELog::RegMethod RegA("WedgeItem","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces(FC,baseLinkPt);
  createLinks();
  createObjects(System,FC,baseLinkPt,FL,topLinkPt,bottomLinkPt);
  insertObjects(System);              

  return;
}

}  // NAMESPACE essSystem
