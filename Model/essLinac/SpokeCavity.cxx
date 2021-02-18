/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   essBuild/SpokeCavity.cxx
 *
 * Copyright (c) 2017-2021 by Konstantin Batkov
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
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SpokeCavity.h"

namespace essSystem
{

SpokeCavity::SpokeCavity(const std::string& Base,
			 const std::string& Key,const size_t Index) :
  attachSystem::ContainedComp(),
  attachSystem::FixedRotate(Base+Key+std::to_string(Index),6),
  attachSystem::CellMap(),
  baseName(Base),extraName(Base+Key),voidMat(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

SpokeCavity::SpokeCavity(const SpokeCavity& A) :
  attachSystem::ContainedComp(A),
  attachSystem::FixedRotate(A),attachSystem::CellMap(A),
  baseName(A.baseName),
  extraName(A.extraName),
  length(A.length),
  mat(A.mat),
  voidMat(A.voidMat),
  thickness(A.thickness)

  /*!
    Copy constructor
    \param A :: SpokeCavity to copy
  */
{}

SpokeCavity&
SpokeCavity::operator=(const SpokeCavity& A)
  /*!
    Assignment operator
    \param A :: SpokeCavity to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedRotate::operator=(A);
      attachSystem::CellMap::operator=(A);
      length=A.length;
      mat=A.mat;
      voidMat=A.voidMat;
      thickness=A.thickness;
    }
  return *this;
}

SpokeCavity*
SpokeCavity::clone() const
/*!
  Clone self
  \return new (this)
 */
{
    return new SpokeCavity(*this);
}

SpokeCavity::~SpokeCavity()
  /*!
    Destructor
  */
{}

void
SpokeCavity::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("SpokeCavity","populate");

  FixedRotate::populate(Control);

  length=Control.EvalTail<double>(keyName,extraName,"Length");
  thickness=Control.EvalTail<double>(keyName,extraName,"Thickness");
  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");


  //  voidMat=Control.EvalPair<int>(keyName,extraName,"VoidMat");

  return;
}


void
SpokeCavity::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("SpokeCavity","createSurfaces");

  // Dividers
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin,X);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin,Z);

  ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length),Y);
  ModelSupport::buildPlane(SMap,buildIndex+12,Origin+Y*(length),Y);

  double coneAngle1 = 17.3;
  double coneAngle2 = 28.22;

  double coneOffSet1 = thickness/sin(coneAngle1*M_PI/180.0);
  double coneOffSet2 = thickness/sin(coneAngle2*M_PI/180.0);
 
  // Inner part

  double pipeRad = 2.801; //otherwise particle will start on surface
  
  ModelSupport::buildCylinder(SMap,buildIndex+8,Origin,Y,pipeRad);

  ModelSupport::buildCylinder(SMap,buildIndex+9,Origin,Y,24.06);
  ModelSupport::buildPlane(SMap,buildIndex+10,Origin+Y*(0.5*length-38.94),Y);
  ModelSupport::buildPlane(SMap,buildIndex+11,Origin+Y*(0.5*length+38.94),Y);

  ModelSupport::buildCylinder(SMap,buildIndex+13,Origin,Y,7.39);
  ModelSupport::buildPlane(SMap,buildIndex+14,Origin+Y*(0.5*length-26.37),Y);
  ModelSupport::buildPlane(SMap,buildIndex+15,Origin+Y*(0.5*length+26.37),Y);

  ModelSupport::buildCone(SMap,buildIndex+16,Origin+Y*(0.5*length+2.66),Y,coneAngle1,1);
  ModelSupport::buildCone(SMap,buildIndex+17,Origin+Y*(0.5*length-2.66),Y,coneAngle1,-1);

  ModelSupport::buildCone(SMap,buildIndex+18,Origin+Y*(0.5*length-79.74),Y,coneAngle2,1);
  ModelSupport::buildCone(SMap,buildIndex+19,Origin+Y*(0.5*length+79.74),Y,coneAngle2,-1);

  // Outer part
  ModelSupport::buildCylinder(SMap,buildIndex+28,Origin,Y,pipeRad+thickness);

  ModelSupport::buildCylinder(SMap,buildIndex+29,Origin,Y,24.06+thickness);
  ModelSupport::buildPlane(SMap,buildIndex+210,Origin+Y*(0.5*length-38.94-thickness),Y);
  ModelSupport::buildPlane(SMap,buildIndex+211,Origin+Y*(0.5*length+38.94+thickness),Y);

  ModelSupport::buildCylinder(SMap,buildIndex+213,Origin,Y,7.39-thickness);
  ModelSupport::buildPlane(SMap,buildIndex+214,Origin+Y*(0.5*length-26.37-thickness),Y);
  ModelSupport::buildPlane(SMap,buildIndex+215,Origin+Y*(0.5*length+26.37+thickness),Y);

  ModelSupport::buildCone(SMap,buildIndex+216,Origin+Y*(0.5*length+2.66+coneOffSet1),Y,coneAngle1,1);
  ModelSupport::buildCone(SMap,buildIndex+217,Origin+Y*(0.5*length-2.66-coneOffSet1),Y,coneAngle1,-1);

  ModelSupport::buildCone(SMap,buildIndex+218,Origin+Y*(0.5*length-79.74-coneOffSet2),Y,coneAngle2,1);
  ModelSupport::buildCone(SMap,buildIndex+219,Origin+Y*(0.5*length+79.74+coneOffSet2),Y,coneAngle2,-1);


  return;
}

void
SpokeCavity::createObjects(Simulation& System)
  /*!
    Adds the all the components

    \todo This addOuterUnionSurf is AWFUL. It COMPLETE slows
    down the MCNP run. It should be basically a wrapper cell.

    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("SpokeCavity","createObjects");

  HeadRule HR;

  // Inner Part

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 13 -9 10 -11 16 17 -18 -19");
  makeCell("Inner",System,cellIndex++,voidMat,0.0,HR);
  addOuterUnionSurf(HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 8 -13 14 -15");
  makeCell("Inner",System,cellIndex++,voidMat,0.0,HR);
  addOuterUnionSurf(HR);

  
  HR=ModelSupport::getHeadRule(SMap,buildIndex," 1 -2 -8");
  makeCell("Inner",System,cellIndex++,voidMat,0.0,HR);
  addOuterUnionSurf(HR);


  // Outer part

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 216 -16 15 -211");
  makeCell("Outer",System,cellIndex++,mat,0.0,HR);
  addOuterUnionSurf(HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 217 -17 210 -14");
  makeCell("Outer",System,cellIndex++,mat,0.0,HR);
  addOuterUnionSurf(HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 19 -219 -211 -29");
  makeCell("Outer",System,cellIndex++,mat,0.0,HR);
  addOuterUnionSurf(HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 18 -218 210 -29");
  makeCell("Outer",System,cellIndex++,mat,0.0,HR);
  addOuterUnionSurf(HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 10 -11  9 -29 -18 -19");
  makeCell("Outer",System,cellIndex++,mat,0.0,HR);
  addOuterUnionSurf(HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 210 -10 13 -9 -18 -19 16 17");
  makeCell("Outer",System,cellIndex++,mat,0.0,HR);
  addOuterUnionSurf(HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 11 -211 13 -9 -18 -19 16 17");
  makeCell("Outer",System,cellIndex++,mat,0.0,HR);
  addOuterUnionSurf(HR);
  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"214 -14 8 -13 -217");
  makeCell("Outer",System,cellIndex++,mat,0.0,HR);
  addOuterUnionSurf(HR);
  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"15 -215 8 -13 -216 ");
  makeCell("Outer",System,cellIndex++,mat,0.0,HR);
  addOuterUnionSurf(HR);
  
  HR=ModelSupport::getHeadRule(SMap,buildIndex," 1 -214 8 -28" );
  makeCell("Outer",System,cellIndex++,mat,0.0,HR);
  addOuterUnionSurf(HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 215 -2 8 -28" );
  makeCell("Outer",System,cellIndex++,mat,0.0,HR);
  addOuterUnionSurf(HR);

  
  return;
}


void
SpokeCavity::createLinks()

  /*!
    Create all the linkes
  */
{
  ELog::RegMethod RegA("SpokeCavity","createLinks");

  FixedComp::setConnect(0,Origin,-Y);
  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+1));

  FixedComp::setConnect(1,Origin+Y*(length),Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+12));


  // for (int i=6; i<8; i++)
  //   ELog::EM << keyName << " " << i << "\t" << getLinkSurf(i) << "\t" << getLinkPt(i) << "\t\t" << getLinkAxis(i) << ELog::endDiag;

  return;
}




void
SpokeCavity::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("SpokeCavity","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // essSystem
