/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   System/construct/torusUnit.cxx
 *
 * Copyright (c) 2004-2025 by Konstantin Batkov
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
#include "Exception.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "Vec3D.h"
#include "surfRegister.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"

#include "torusUnit.h"

namespace constructSystem
{

torusUnit::torusUnit(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedRotate(Key,6),
  attachSystem::CellMap(),
  attachSystem::SurfMap()
 /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

torusUnit::torusUnit(const torusUnit& A) :
  attachSystem::ContainedComp(A),
  attachSystem::FixedRotate(A),
  attachSystem::CellMap(A),
  attachSystem::SurfMap(A),
  rMinor(A.rMinor),
    rMajor(A.rMajor),
    nFaces(A.nFaces),
  mat(A.mat)
  /*!
    Copy constructor
    \param A :: torusUnit to copy
  */
{}

torusUnit&
torusUnit::operator=(const torusUnit& A)
  /*!
    Assignment operator
    \param A :: torusUnit to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedRotate::operator=(A);
      attachSystem::CellMap::operator=(A);
      rMinor=A.rMinor;
        rMajor=A.rMajor;
        nFaces=A.nFaces;
      mat=A.mat;
    }
  return *this;
}

torusUnit*
torusUnit::clone() const
/*!
  Clone self
  \return new (this)
 */
{
    return new torusUnit(*this);
}

torusUnit::~torusUnit()
  /*!
    Destructor
  */
{}

void
torusUnit::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("torusUnit","populate");

  FixedRotate::populate(Control);

  rMinor=Control.EvalVar<double>(keyName+"MinorRadius");
  rMajor=Control.EvalVar<double>(keyName+"MajorRadius");
  if (rMinor>rMajor)
    throw ColErr::OrderError<double>(rMinor,rMajor,"MinorRadius < MajorRadius");

  nFaces=Control.EvalVar<int>(keyName+"NFaces");
  if (nFaces % 4 != 0)
    throw ColErr::ExitAbort("NFaces must be evenly divisible by 4");

  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");

  return;
}

std::vector<std::pair<double, double>> torusUnit::getVertices() const
/*!
    Calculate the coordinates of the vertices of a regular polygon with n sides
    inscribed in a circle of radius r.

    Returns:
    list[tuple]: A list of (x, y) coordinates of the vertices.
 */
{
  std::vector<std::pair<double, double>> vertices;
  const double angle_step = 2 * M_PI / nFaces;  // Angle between consecutive vertices in radians
  const double angle0 = angle_step/2.0;

  for (int i=0; i<nFaces; ++i) {
    // Calculate the angle for the current vertex
    const double angle = static_cast<double>(i) * angle_step + angle0;
    // Compute x and y coordinates
    const std::pair<double, double> v(rMinor*cos(angle)-rMajor, rMinor*sin(angle));
    vertices.push_back(v);
  }
  return vertices;
}

double torusUnit::getApex(const std::pair<double, double>& a, const std::pair<double, double>& b) const
/*!
  Return the y-coordinate of the cone apex
 */
{
  if (std::abs(a.first-b.first)<Geometry::zeroTol)
    throw ColErr::ExitAbort("The line is vertical and does not intersect the y-axis.");

  const double x1 = a.first;
  const double y1 = a.second;
  const double x2 = b.first;
  const double y2 = b.second;

  //  Calculate slope (k) of the line
  const double k = (y2 - y1) / (x2 - x1);

  // Calculate y-intercept (b) using the line equation: y = kx + c
  const double c = y1 - k * (x1);

  return c;
}



void
torusUnit::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("torusUnit","createSurfaces");

  // dividing surfaces
  ModelSupport::buildPlane(SMap,buildIndex+11,Origin,Y);
  ModelSupport::buildCylinder(SMap,buildIndex+27,Origin,Y,rMajor);


  const std::vector<std::pair<double, double>> vertices = getVertices();

  const double r = -vertices[0].first;
  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,r);
  const double R = -vertices[nFaces/2].first;
  ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Y,R);

  const double height = vertices[nFaces/4].second;
  const double depth = vertices[3*nFaces/4].second;
  SurfMap::makePlane("bottom",SMap,buildIndex+1,Origin+Y*(depth),Y);
  SurfMap::makePlane("top",SMap,buildIndex+2,Origin+Y*(height),Y);

  int SI(8);
  for (int i=1; i<nFaces; ++i) {
    if ((i==nFaces/2) || (i==nFaces/4) || (i==3*nFaces/4))
      continue;
    const auto a = vertices[i-1];
    const auto b = vertices[i];
    const double apex = getApex(a, b);
    const double angle = -atan2(b.first-a.first, b.second-a.second) * 180.0/M_PI;

    ModelSupport::buildCone(SMap, buildIndex+SI, Origin+Y*apex, -Y, angle);
    SI+=10;
  }


  // SurfMap::makePlane("back",SMap,buildIndex+1,Origin-Y*(length/2.0),Y);
  // SurfMap::makePlane("front",SMap,buildIndex+2,Origin+Y*(length/2.0),Y);

  // SurfMap::makePlane("left",SMap,buildIndex+3,Origin-X*(width/2.0),X);
  // SurfMap::makePlane("right",SMap,buildIndex+4,Origin+X*(width/2.0),X);


  return;
}

void
torusUnit::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("torusUnit","createObjects");

  HeadRule HR,HR1;
  const int n = (nFaces-4)/4;
  int SI(0);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"11 -2 7 -27");// 8 18");
  for (int i=0; i<n; ++i) {
    HR1 *= ModelSupport::getHeadRule(SMap,buildIndex+SI,"8");
    SI+=10;
  }
  makeCell("Main1",System,cellIndex++,mat,0.0,HR*HR1);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"11 -2");
  HR1 *= ModelSupport::getHeadRule(SMap,buildIndex,"7");
  makeCell("InnerUpper",System,cellIndex++,0,0.0,HR*HR1.complement());

  HR1.reset();
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"11 -2 27 -17");
  for (int i=0; i<n; ++i) {
    HR1 *= ModelSupport::getHeadRule(SMap,buildIndex+SI,"-8");
    SI+=10;
  }
  makeCell("Main4",System,cellIndex++,mat,0.0,HR*HR1);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"11 -2 -17");
  makeCell("OuterUpper",System,cellIndex++,0,0.0,HR*HR1.complement());

  HR1.reset();
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -11 27 -17");// -48 -58");
  for (int i=0; i<n; ++i) {
    HR1 *= ModelSupport::getHeadRule(SMap,buildIndex+SI,"-8");
    SI+=10;
  }
  makeCell("Main3",System,cellIndex++,mat,0.0,HR*HR1);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -11 -17");
  makeCell("OuterUpper",System,cellIndex++,0,0.0,HR*HR1.complement());

  HR1.reset();
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -11 7 -27");// 68 78");
  for (int i=0; i<n; ++i) {
    HR1 *= ModelSupport::getHeadRule(SMap,buildIndex+SI,"8");
    SI+=10;
  }
  makeCell("Main2",System,cellIndex++,mat,0.0,HR*HR1);

  HR1 *= ModelSupport::getHeadRule(SMap,buildIndex,"7");
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -11");
  makeCell("InnerBottom",System,cellIndex++,0,0.0,HR*HR1.complement());


  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 -17");
  addOuterSurf(HR);

  return;
}


void
torusUnit::createLinks()
  /*!
    Create all the links
  */
{
  ELog::RegMethod RegA("torusUnit","createLinks");

  // FixedComp::setConnect(0,Origin-Y*(length/2.0),-Y);
  // FixedComp::setNamedLinkSurf(0,"Back",SurfMap::getSignedSurf("#back"));

  //  ELog::EM << "TODO: Check and use names for the links below:" << ELog::endDiag;

  // FixedComp::setConnect(1,Origin+Y*(length/2.0),Y);
  // FixedComp::setNamedLinkSurf(1,"Front",SMap.realSurf(buildIndex+2));

  // FixedComp::setConnect(2,Origin-X*(width/2.0),-X);
  // FixedComp::setNamedLinkSurf(2,"Left",-SMap.realSurf(buildIndex+3));

  // FixedComp::setConnect(3,Origin+X*(width/2.0),X);
  // FixedComp::setNamedLinkSurf(3,"Right",SMap.realSurf(buildIndex+4));

  // FixedComp::setConnect(4,Origin-Z*(height/2.0),-Z);
  // FixedComp::setNamedLinkSurf(4,"Bottom",-SMap.realSurf(buildIndex+5));

  // FixedComp::setConnect(5,Origin+Z*(height/2.0),Z);
  // FixedComp::setNamedLinkSurf(5,"Top",SMap.realSurf(buildIndex+6));

  return;
}

void
torusUnit::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("torusUnit","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // constructSystem
