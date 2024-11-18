/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   System/construct/Torus.cxx
 *
 * Copyright (c) 2004-2024 by Konstantin Batkov
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

#include "Torus.h"

namespace constructSystem
{

Torus::Torus(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedRotate(Key,6),
  attachSystem::CellMap(),
  attachSystem::SurfMap()
 /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

Torus::Torus(const Torus& A) :
  attachSystem::ContainedComp(A),
  attachSystem::FixedRotate(A),
  attachSystem::CellMap(A),
  attachSystem::SurfMap(A),
  rMinor(A.rMinor),
    rMajor(A.rMajor),
    nSides(A.nSides),
  mat(A.mat)
  /*!
    Copy constructor
    \param A :: Torus to copy
  */
{}

Torus&
Torus::operator=(const Torus& A)
  /*!
    Assignment operator
    \param A :: Torus to copy
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
        nSides=A.nSides;
      mat=A.mat;
    }
  return *this;
}

Torus*
Torus::clone() const
/*!
  Clone self
  \return new (this)
 */
{
    return new Torus(*this);
}

Torus::~Torus()
  /*!
    Destructor
  */
{}

void
Torus::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("Torus","populate");

  FixedRotate::populate(Control);

  rMinor=Control.EvalVar<double>(keyName+"MinorRadius");
  rMajor=Control.EvalVar<double>(keyName+"MajorRadius");
  if (rMinor*2>rMajor)
    throw ColErr::OrderError<double>(rMinor*2,rMajor,"MinorRadius*2 < MajorRadius");

  nSides=Control.EvalVar<int>(keyName+"NSides");
  if (nSides % 4 != 0)
    throw ColErr::ExitAbort("NSides must be evenly divisible by 4");

  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");

  return;
}

std::vector<std::pair<double, double>> Torus::getVertices() const
/*!
    Calculate the coordinates of the vertices of a regular polygon with n sides
    inscribed in a circle of radius r.

    Returns:
    list[tuple]: A list of (x, y) coordinates of the vertices.
 */
{
  std::vector<std::pair<double, double>> vertices;
  const double angle_step = 2 * M_PI / nSides;  // Angle between consecutive vertices in radians
  const double angle0 = angle_step/2.0;

  for (int i=0; i<nSides; ++i) {
    // Calculate the angle for the current vertex
    const double angle = static_cast<double>(i) * angle_step + angle0;
    // Compute x and y coordinates
    const std::pair<double, double> v(rMinor*cos(angle), rMinor*sin(angle));
    vertices.push_back(v);
  }
  return vertices;
}

double Torus::getApex(const std::pair<double, double>& a, const std::pair<double, double>& b) const
/*!
  Return the y-coordinate of the cone apex
 */
{
  if (std::abs(a.first-b.first)<Geometry::zeroTol)
    throw ColErr::ExitAbort("The line is vertical and does not intersect the y-axis.");

  //  Calculate slope (m) of the line
  const double m = (b.second - a.second) / (b.first - a.first);

  // Calculate y-intercept (c) using the line equation: y = mx + c
  const double c = b.second - m * a.first;

  return c;
}



void
Torus::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("Torus","createSurfaces");

  const std::vector<std::pair<double, double>> vertices = getVertices();

  const double r = rMajor-vertices[0].first;
  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,r);
  const double R = rMajor-vertices[nSides/2].first;
  ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Y,R);

  const double height = vertices[nSides/4].second;
  const double depth = vertices[3*nSides/4].second;
  SurfMap::makePlane("bottom",SMap,buildIndex+1,Origin+Y*(depth),Y);
  SurfMap::makePlane("top",SMap,buildIndex+2,Origin+Y*(height),Y);


  ELog::EM << depth << " " << height << ELog::endDiag;

  // SurfMap::makePlane("back",SMap,buildIndex+1,Origin-Y*(length/2.0),Y);
  // SurfMap::makePlane("front",SMap,buildIndex+2,Origin+Y*(length/2.0),Y);

  // SurfMap::makePlane("left",SMap,buildIndex+3,Origin-X*(width/2.0),X);
  // SurfMap::makePlane("right",SMap,buildIndex+4,Origin+X*(width/2.0),X);


  return;
}

void
Torus::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("Torus","createObjects");

  HeadRule HR;
  HR=ModelSupport::getHeadRule(SMap,buildIndex," 1 -2 7 -17");
  makeCell("Main",System,cellIndex++,mat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 1 -2 -7");
  makeCell("Inner",System,cellIndex++,0,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 1 -2 -17");
  addOuterSurf(HR);

  return;
}


void
Torus::createLinks()
  /*!
    Create all the links
  */
{
  ELog::RegMethod RegA("Torus","createLinks");

  // FixedComp::setConnect(0,Origin-Y*(length/2.0),-Y);
  // FixedComp::setNamedLinkSurf(0,"Back",SurfMap::getSignedSurf("#back"));

  ELog::EM << "TODO: Check and use names for the links below:" << ELog::endDiag;

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
Torus::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("Torus","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // constructSystem
