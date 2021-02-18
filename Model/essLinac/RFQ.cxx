/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   essBuild/RFQ.cxx
 *
 * Copyright (c) 2018-2021 by Konstantin Batkov
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
#include "surfRegister.h"
#include "objectRegister.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "SurInter.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "inputParam.h"
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
#include "FixedOffset.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"

#include "RFQ.h"

namespace essSystem
{

RFQ::RFQ(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(Key,6),
  attachSystem::CellMap()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

RFQ::RFQ(const RFQ& A) :
  attachSystem::ContainedComp(A),
  attachSystem::FixedOffset(A),
  attachSystem::CellMap(A),
  length(A.length),
  outerWidth(A.outerWidth),
  innerWidth(A.innerWidth),
  wallThick(A.wallThick),
  vaneThick(A.vaneThick),
  vaneLength(A.vaneLength),
  vaneTipThick(A.vaneTipThick),
  vaneTipDist(A.vaneTipDist),
  coolantOuterRadius(A.coolantOuterRadius),
  coolantOuterDist(A.coolantOuterDist),
  coolantInnerRadius(A.coolantInnerRadius),
  coolantInnerDist(A.coolantInnerDist),
  mainMat(A.mainMat),wallMat(A.wallMat),
  coolantMat(A.coolantMat)
  /*!
    Copy constructor
    \param A :: RFQ to copy
  */
{}

RFQ&
RFQ::operator=(const RFQ& A)
  /*!
    Assignment operator
    \param A :: RFQ to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      attachSystem::CellMap::operator=(A);
      length=A.length;
      outerWidth=A.outerWidth;
      innerWidth=A.innerWidth;
      wallThick=A.wallThick;
      vaneThick=A.vaneThick;
      vaneLength=A.vaneLength;
      vaneTipThick=A.vaneTipThick;
      vaneTipDist=A.vaneTipDist;
      coolantOuterRadius=A.coolantOuterRadius;
      coolantOuterDist=A.coolantOuterDist;
      coolantInnerRadius=A.coolantInnerRadius;
      coolantInnerDist=A.coolantInnerDist;
      mainMat=A.mainMat;
      wallMat=A.wallMat;
      coolantMat=A.coolantMat;
    }
  return *this;
}

RFQ*
RFQ::clone() const
/*!
  Clone self
  \return new (this)
 */
{
    return new RFQ(*this);
}

RFQ::~RFQ()
  /*!
    Destructor
  */
{}

void
RFQ::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("RFQ","populate");

  FixedOffset::populate(Control);

  length=Control.EvalVar<double>(keyName+"Length");
  outerWidth=Control.EvalVar<double>(keyName+"OuterWidth");
  innerWidth=Control.EvalVar<double>(keyName+"InnerWidth");
  wallThick=Control.EvalVar<double>(keyName+"WallThick");
  vaneThick=Control.EvalVar<double>(keyName+"VaneThick");
  vaneLength=Control.EvalVar<double>(keyName+"VaneLength");
  vaneTipThick=Control.EvalVar<double>(keyName+"VaneTipThick");
  vaneTipDist=Control.EvalVar<double>(keyName+"VaneTipDist");
  coolantOuterRadius=Control.EvalVar<double>(keyName+"CoolantOuterRadius");
  coolantOuterDist=Control.EvalVar<double>(keyName+"CoolantOuterDist");
  coolantInnerRadius=Control.EvalVar<double>(keyName+"CoolantInnerRadius");
  coolantInnerDist=Control.EvalVar<double>(keyName+"CoolantInnerDist");

  mainMat=ModelSupport::EvalMat<int>(Control,keyName+"MainMat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  coolantMat=ModelSupport::EvalMat<int>(Control,keyName+"CoolantMat");

  return;
}


void
RFQ::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("RFQ","createSurfaces");

  const double theta(45.0);

  ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length),Y);

  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(outerWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(outerWidth/2.0),X);

  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(outerWidth/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(outerWidth/2.0),Z);

  // inner surfaces
  Geometry::Vec3D dirX(X);
  const double dx(innerWidth/2.0/cos(theta*M_PI/180.0));
  Geometry::Quaternion::calcQRotDeg(-theta,Y).rotate(dirX);
  ModelSupport::buildPlane(SMap,buildIndex+13,Origin-X*(dx),dirX);
  ModelSupport::buildPlane(SMap,buildIndex+14,Origin+X*(dx),dirX);

  Geometry::Vec3D dirZ(Z);
  Geometry::Quaternion::calcQRotDeg(-theta,Y).rotate(dirZ);
  ModelSupport::buildPlane(SMap,buildIndex+15,Origin-Z*(dx),dirZ);
  ModelSupport::buildPlane(SMap,buildIndex+16,Origin+Z*(dx),dirZ);

  // outer surfaces
  const double VL(vaneLength*cos(theta*M_PI/180));
  const std::vector<Geometry::Vec3D> coolantDir = {-X,X,-Z,Z};
  for (int i=0; i<4; i++)
    {
      ModelSupport::buildShiftedPlane(SMap,buildIndex+23+i,
				      SMap.realPtr<Geometry::Plane>(buildIndex+13+i),
				      (i%2) ? wallThick : -wallThick);

      const int s((i%3) ? 4 : 3);
      const Geometry::Vec3D A =
	SurInter::getPoint(SMap.realSurfPtr(buildIndex+23+i),
			   SMap.realSurfPtr(buildIndex+s),
			   SMap.realSurfPtr(buildIndex+1));
      const Geometry::Plane *p =
	ModelSupport::buildPlane(SMap,buildIndex+33+i, A, i<2 ? dirZ : dirX);
      ModelSupport::buildShiftedPlane(SMap,buildIndex+i+133,p, (i%2) ? VL : -VL);

      ModelSupport::buildCylinder(SMap,buildIndex+i*10+7,
				  Origin+coolantDir[static_cast<size_t>(i)]*coolantOuterDist,
				  Y,coolantOuterRadius);

      ModelSupport::buildCylinder(SMap,buildIndex+i*10+107,
				  Origin+coolantDir[static_cast<size_t>(i)]*coolantInnerDist,
				  Y,coolantInnerRadius);

      // vane tips
      ModelSupport::buildCylinder(SMap,buildIndex+i*10+207,
				  Origin+coolantDir[static_cast<size_t>(i)]*vaneTipDist,
				  Y,vaneTipThick/2.0);
      // auxiliary plane for cylinders
      ModelSupport::buildPlane(SMap,buildIndex+i*10+208,
			       Origin+coolantDir[static_cast<size_t>(i)]*vaneTipDist,
			       coolantDir[static_cast<size_t>(i)]);
    }

  // Vanes
  ModelSupport::buildPlane(SMap,buildIndex+103,Origin-X*(vaneThick/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+104,Origin+X*(vaneThick/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+105,Origin-Z*(vaneThick/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+106,Origin+Z*(vaneThick/2.0),Z);

  // Vane tips
  ModelSupport::buildPlane(SMap,buildIndex+203,Origin-X*(vaneTipThick/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+204,Origin+X*(vaneTipThick/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+205,Origin-Z*(vaneTipThick/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+206,Origin+Z*(vaneTipThick/2.0),Z);

  // aux cylinder to simplify central void cell
  const Geometry::Vec3D A =
    SurInter::getPoint(SMap.realSurfPtr(buildIndex+203),
		       SMap.realSurfPtr(buildIndex+228),
		       SMap.realSurfPtr(buildIndex+1));
  const double Rin(sqrt(pow(A.X(), 2) + pow(A.Z(), 2)));
  ModelSupport::buildCylinder(SMap,buildIndex+209,Origin,Y,Rin);

  const Geometry::Vec3D B =
    SurInter::getPoint(SMap.realSurfPtr(buildIndex+103),
		       SMap.realSurfPtr(buildIndex+105),
		       SMap.realSurfPtr(buildIndex+1));
  const double Rout(sqrt(pow(B.X(), 2) + pow(B.Z(), 2)));
  ModelSupport::buildCylinder(SMap,buildIndex+219,Origin,Y,Rout);

  return;
}

void
RFQ::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("RFQ","createObjects");

  const HeadRule sideHR(ModelSupport::getHeadRule(SMap,buildIndex," 1 -2 "));

  HeadRule HR;
  // Vanes
  // -X
  HR=ModelSupport::getHeadRule(SMap,buildIndex,
				 " (36:-33) 105 -106 -103 -136 133 107 ");
  makeCell("wall",System,cellIndex++,wallMat,0.0,HR*sideHR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex," -107 ");
  makeCell("coolant",System,cellIndex++,coolantMat,0.0,HR*sideHR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex," 34 -33 13 -105 -103 ");
  makeCell("main",System,cellIndex++,mainMat,0.0,HR*sideHR);

  // -Z
  HR=ModelSupport::getHeadRule(SMap,buildIndex,
				 " (34:36) 103 -104 -136 -134 127 ");
  makeCell("wll",System,cellIndex++,wallMat,0.0,HR*sideHR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex," -127 ");
  makeCell("coolant",System,cellIndex++,coolantMat,0.0,HR*sideHR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex," 36 -35 15 104 -105 ");
  makeCell("main",System,cellIndex++,mainMat,0.0,HR*sideHR);

  // +X
  HR=ModelSupport::getHeadRule(SMap,buildIndex,
				 " (-35:34) 105 -106 103 135 -134 117 ");
  makeCell("wall",System,cellIndex++,wallMat,0.0,HR*sideHR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex," -117 ");
  makeCell("coolant",System,cellIndex++,coolantMat,0.0,HR*sideHR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex," 34 -33 -14 106 104 ");
  makeCell("main",System,cellIndex++,mainMat,0.0,HR*sideHR);

  // +Z
  HR=ModelSupport::getHeadRule(SMap,buildIndex,
				 " (-33:-35) 103 -104 135 133 137 ");
  makeCell("wall",System,cellIndex++,wallMat,0.0,HR*sideHR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex," -137 ");
  makeCell("coolant",System,cellIndex++,coolantMat,0.0,HR*sideHR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex," 36 -35 -16 106 -103 ");
  makeCell("main",System,cellIndex++,mainMat,0.0,HR*sideHR);

  // vane tip -x
  HR=ModelSupport::getHeadRule(SMap,buildIndex," ((136:-133) 205 -206 208 ) ");
  makeCell("vane",System,cellIndex++,wallMat,0.0,HR*sideHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex, " -208 -207) ");
  makeCell("vane",System,cellIndex++,wallMat,0.0,HR*sideHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex, " 105 -205 -133 219 -135 ");
  makeCell("vane",System,cellIndex++,mainMat,0.0,HR*sideHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex, " 206 -106 136 134 219 ");
  makeCell("vane",System,cellIndex++,mainMat,0.0,HR*sideHR);

  // vane tip +x
  HR=ModelSupport::getHeadRule(SMap,buildIndex," ((-135:134) 205 -206 218 ) ");
  makeCell("vane",System,cellIndex++,wallMat,0.0,HR*sideHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," -218 -217 ) ");
  makeCell("vane",System,cellIndex++,wallMat,0.0,HR*sideHR);

  
  // vane tip -z
  HR=ModelSupport::getHeadRule(SMap,buildIndex," ((136:134) 203 -204 228 ) ");
  makeCell("vane",System,cellIndex++,wallMat,0.0,HR*sideHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," -228 -227) ");
  makeCell("vane",System,cellIndex++,wallMat,0.0,HR*sideHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex, " 103 -203 134 219 -135 ");
  makeCell("vane",System,cellIndex++,mainMat,0.0,HR*sideHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex, " 204 -104 136 219 -133 ");
  makeCell("vane",System,cellIndex++,mainMat,0.0,HR*sideHR);

  // vane tip +z
  HR=ModelSupport::getHeadRule(SMap,buildIndex," ((-135:-133) 203 -204 238) ");
  makeCell("vane",System,cellIndex++,wallMat,0.0,HR*sideHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," -238 -237 ");
  makeCell("vane",System,cellIndex++,wallMat,0.0,HR*sideHR);

  // void in the corners
  HR=ModelSupport::getHeadRule(SMap,buildIndex, " -219 -203 -205 209 ");
  makeCell("corner",System,cellIndex++,mainMat,0.0,HR*sideHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex, " -219 204 -205 209 ");
  makeCell("corner",System,cellIndex++,mainMat,0.0,HR*sideHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex, " -219 204 206 209 ");
  makeCell("corner",System,cellIndex++,mainMat,0.0,HR*sideHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex, " -219 -203 206 209 ");
  makeCell("corner",System,cellIndex++,mainMat,0.0,HR*sideHR);

  // very central void
  HR=ModelSupport::getHeadRule(SMap,buildIndex,
				 " -209 207 217 227 237 ");
  makeCell("centralVoid",System,cellIndex++,mainMat,0.0,HR*sideHR);


  HR=ModelSupport::getHeadRule(SMap,buildIndex," 23 -13 34 -33 ");
  makeCell("wall",System,cellIndex++,wallMat,0.0,HR*sideHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 14 -24 34 -33 ");
  makeCell("wall",System,cellIndex++,wallMat,0.0,HR*sideHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 25 -15 36 -35 ");
  makeCell("wall",System,cellIndex++,wallMat,0.0,HR*sideHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 16 -26 36 -35 ");
  makeCell("wall",System,cellIndex++,wallMat,0.0,HR*sideHR);

  // -X
  HR=ModelSupport::getHeadRule(SMap,buildIndex," 3 33 -36 7 ");
  makeCell("wall",System,cellIndex++,wallMat,0.0,HR*sideHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," -7 ");
  makeCell("coolant",System,cellIndex++,coolantMat,0.0,HR*sideHR);

  // -Z
  HR=ModelSupport::getHeadRule(SMap,buildIndex," 5 -34 -36 27 ");
  makeCell("wall",System,cellIndex++,wallMat,0.0,HR*sideHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," -27 ");
  makeCell("coolant",System,cellIndex++,coolantMat,0.0,HR*sideHR);

  // +X
  HR=ModelSupport::getHeadRule(SMap,buildIndex," -4 35 -34 17 ");
  makeCell("wall",System,cellIndex++,wallMat,0.0,HR*sideHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," -17 ");
  makeCell("collant",System,cellIndex++,coolantMat,0.0,HR*sideHR);

  // +Z
  HR=ModelSupport::getHeadRule(SMap,buildIndex," -6 33 35 37 ");
  makeCell("wall",System,cellIndex++,wallMat,0.0,HR*sideHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," -37 ");
  makeCell("coolant",System,cellIndex++,coolantMat,0.0,HR*sideHR);


  HR=ModelSupport::getHeadRule(SMap,buildIndex," 23 -24 25 -26 3 -4 5 -6 ");
  addOuterSurf(HR*sideHR);

  return;
}


void
RFQ::createLinks()
  /*!
    Create all the linkes
  */
{
  ELog::RegMethod RegA("RFQ","createLinks");

  //  FixedComp::setConnect(0,Origin,-Y);
  //  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+1));

  FixedComp::setConnect(0,Origin+Y*(length),Y);
  FixedComp::setLinkSurf(0,SMap.realSurf(buildIndex+2));

  return;
}




void
RFQ::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("RFQ","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // essSystem
