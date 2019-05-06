/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   essBuild/SpokeCavity.cxx
 *
 * Copyright (c) 2017-2018 by Konstantin Batkov
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
#include "surfEqual.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "inputParam.h"
#include "HeadRule.h"
#include "Object.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ReadFunctions.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "FixedOffset.h"
#include "surfDBase.h"
#include "surfDIter.h"
#include "surfDivide.h"
#include "SurInter.h"
#include "mergeTemplate.h"
#include "CellMap.h"
#include "SpokeCavity.h"

namespace essSystem
{

  SpokeCavity::SpokeCavity(const std::string& Base,const std::string& Key,const size_t Index) :
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(Base+Key+StrFunc::makeString(Index),6),
  attachSystem::CellMap(),
  baseName(Base),
  extraName(Base+Key),
  surfIndex(ModelSupport::objectRegister::Instance().cell(keyName)),
  cellIndex(surfIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

SpokeCavity::SpokeCavity(const SpokeCavity& A) :
  attachSystem::ContainedComp(A),
  attachSystem::FixedOffset(A),attachSystem::CellMap(A),
  baseName(A.baseName),
  extraName(A.extraName),
  surfIndex(A.surfIndex),cellIndex(A.cellIndex),
  engActive(A.engActive),
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
      attachSystem::FixedOffset::operator=(A);
      attachSystem::CellMap::operator=(A);
      cellIndex=A.cellIndex;
      engActive=A.engActive;
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

  FixedOffset::populate(Control);
  engActive=Control.EvalTriple<int>(keyName,baseName,"","EngineeringActive");

  length=Control.EvalPair<double>(keyName,extraName,"Length");
  voidMat=Control.EvalPair<int>(keyName,extraName,"VoidMat");
  thickness=Control.EvalPair<double>(keyName,extraName,"Thickness");
  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");

  //waterMat=ModelSupport::EvalMat<int>(Control,baseName+"WaterMat");

  return;
}

void
SpokeCavity::createUnitVector(const attachSystem::FixedComp& FC,
			      const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: object for origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("SpokeCavity","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();

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
  ModelSupport::buildPlane(SMap,surfIndex+3,Origin,X);
  ModelSupport::buildPlane(SMap,surfIndex+5,Origin,Z);

  ModelSupport::buildPlane(SMap,surfIndex+1,Origin,Y);
  ModelSupport::buildPlane(SMap,surfIndex+2,Origin+Y*(length),Y);
  ModelSupport::buildPlane(SMap,surfIndex+12,Origin+Y*(length),Y);

  double coneAngle1 = 17.3;
  double coneAngle2 = 28.22;

  double coneOffSet1 = thickness/sin(coneAngle1*M_PI/180.0);
  double coneOffSet2 = thickness/sin(coneAngle2*M_PI/180.0);
 
  // Inner part

  double pipeRad = 2.801; //otherwise particle will start on surface
  
  ModelSupport::buildCylinder(SMap,surfIndex+8,Origin,Y,pipeRad);

  ModelSupport::buildCylinder(SMap,surfIndex+9,Origin,Y,24.06);
  ModelSupport::buildPlane(SMap,surfIndex+10,Origin+Y*(0.5*length-38.94),Y);
  ModelSupport::buildPlane(SMap,surfIndex+11,Origin+Y*(0.5*length+38.94),Y);

  ModelSupport::buildCylinder(SMap,surfIndex+13,Origin,Y,7.39);
  ModelSupport::buildPlane(SMap,surfIndex+14,Origin+Y*(0.5*length-26.37),Y);
  ModelSupport::buildPlane(SMap,surfIndex+15,Origin+Y*(0.5*length+26.37),Y);

  ModelSupport::buildCone(SMap,surfIndex+16,Origin+Y*(0.5*length+2.66),Y,coneAngle1,1);
  ModelSupport::buildCone(SMap,surfIndex+17,Origin+Y*(0.5*length-2.66),Y,coneAngle1,-1);

  ModelSupport::buildCone(SMap,surfIndex+18,Origin+Y*(0.5*length-79.74),Y,coneAngle2,1);
  ModelSupport::buildCone(SMap,surfIndex+19,Origin+Y*(0.5*length+79.74),Y,coneAngle2,-1);

  // Outer part
  ModelSupport::buildCylinder(SMap,surfIndex+28,Origin,Y,pipeRad+thickness);

  ModelSupport::buildCylinder(SMap,surfIndex+29,Origin,Y,24.06+thickness);
  ModelSupport::buildPlane(SMap,surfIndex+210,Origin+Y*(0.5*length-38.94-thickness),Y);
  ModelSupport::buildPlane(SMap,surfIndex+211,Origin+Y*(0.5*length+38.94+thickness),Y);

  ModelSupport::buildCylinder(SMap,surfIndex+213,Origin,Y,7.39-thickness);
  ModelSupport::buildPlane(SMap,surfIndex+214,Origin+Y*(0.5*length-26.37-thickness),Y);
  ModelSupport::buildPlane(SMap,surfIndex+215,Origin+Y*(0.5*length+26.37+thickness),Y);

  ModelSupport::buildCone(SMap,surfIndex+216,Origin+Y*(0.5*length+2.66+coneOffSet1),Y,coneAngle1,1);
  ModelSupport::buildCone(SMap,surfIndex+217,Origin+Y*(0.5*length-2.66-coneOffSet1),Y,coneAngle1,-1);

  ModelSupport::buildCone(SMap,surfIndex+218,Origin+Y*(0.5*length-79.74-coneOffSet2),Y,coneAngle2,1);
  ModelSupport::buildCone(SMap,surfIndex+219,Origin+Y*(0.5*length+79.74+coneOffSet2),Y,coneAngle2,-1);



  // Dividers

  ModelSupport::buildCylinder(SMap,surfIndex+329,Origin,Y,50.0);
  return;
}

void
SpokeCavity::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("SpokeCavity","createObjects");

  std::string Out,Side;

  int SI(surfIndex);

  // Inner Part

  Out=ModelSupport::getComposite(SMap,surfIndex," 13 -9 10 -11 16 17 -18 -19");
  System.addCell(MonteCarlo::Object(cellIndex++,voidMat,0.0,Out));
  addOuterUnionSurf(Out);

  Out=ModelSupport::getComposite(SMap,surfIndex," 8 -13 14 -15");
  System.addCell(MonteCarlo::Object(cellIndex++,voidMat,0.0,Out));
  addOuterUnionSurf(Out);

  
  Out=ModelSupport::getComposite(SMap,surfIndex," 1 -2 -8");
  System.addCell(MonteCarlo::Object(cellIndex++,voidMat,0.0,Out));
  addOuterUnionSurf(Out);


  // Outer part


  Out=ModelSupport::getComposite(SMap,surfIndex," 216 -16 15 -211");
  System.addCell(MonteCarlo::Object(cellIndex++,mat,0.0,Out));
  addOuterUnionSurf(Out);

  Out=ModelSupport::getComposite(SMap,surfIndex," 217 -17 210 -14");
  System.addCell(MonteCarlo::Object(cellIndex++,mat,0.0,Out));
  addOuterUnionSurf(Out);

  Out=ModelSupport::getComposite(SMap,surfIndex," 19 -219 -211 -29");
  System.addCell(MonteCarlo::Object(cellIndex++,mat,0.0,Out));
  addOuterUnionSurf(Out);

  Out=ModelSupport::getComposite(SMap,surfIndex," 18 -218 210 -29");
  System.addCell(MonteCarlo::Object(cellIndex++,mat,0.0,Out));
  addOuterUnionSurf(Out);

  Out=ModelSupport::getComposite(SMap,surfIndex," 10 -11  9 -29 -18 -19");
  System.addCell(MonteCarlo::Object(cellIndex++,mat,0.0,Out));
  addOuterUnionSurf(Out);

  Out=ModelSupport::getComposite(SMap,surfIndex," 210 -10 13 -9 -18 -19 16 17");
  System.addCell(MonteCarlo::Object(cellIndex++,mat,0.0,Out));
  addOuterUnionSurf(Out);

  Out=ModelSupport::getComposite(SMap,surfIndex," 11 -211 13 -9 -18 -19 16 17");
  System.addCell(MonteCarlo::Object(cellIndex++,mat,0.0,Out));
  addOuterUnionSurf(Out);
  
  Out=ModelSupport::getComposite(SMap,surfIndex,"214 -14 8 -13 -217");
  System.addCell(MonteCarlo::Object(cellIndex++,mat,0.0,Out));
  addOuterUnionSurf(Out);
  
  Out=ModelSupport::getComposite(SMap,surfIndex,"15 -215 8 -13 -216 ");
  System.addCell(MonteCarlo::Object(cellIndex++,mat,0.0,Out));
  addOuterUnionSurf(Out);
  
  Out=ModelSupport::getComposite(SMap,surfIndex," 1 -214 8 -28" );
  System.addCell(MonteCarlo::Object(cellIndex++,mat,0.0,Out));
  addOuterUnionSurf(Out);

  Out=ModelSupport::getComposite(SMap,surfIndex," 215 -2 8 -28" );
  System.addCell(MonteCarlo::Object(cellIndex++,mat,0.0,Out));
  addOuterUnionSurf(Out);


  // cell dividers

  Out=ModelSupport::getComposite(SMap,surfIndex," 1 -2 29 -329");
  System.addCell(MonteCarlo::Object(cellIndex++,60,0.0,Out));
  addOuterUnionSurf(Out); 
  
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
  FixedComp::setLinkSurf(0,-SMap.realSurf(surfIndex+1));

  FixedComp::setConnect(1,Origin+Y*(length),Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(surfIndex+12));


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
