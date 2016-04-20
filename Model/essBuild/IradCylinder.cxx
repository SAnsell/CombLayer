/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/IradCylinder.cxx
 *
 * Copyright (c) 2004-2016 by Stuart Ansell
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
#include "surfRegister.h"
#include "objectRegister.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "Quadratic.h"
#include "Cylinder.h"
#include "surfIndex.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "support.h"
#include "stringCombine.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "ContainedComp.h"

#include "IradCylinder.h"

namespace essSystem
{

IradCylinder::IradCylinder(const std::string& Key) :
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(Key,6),
  attachSystem::CellMap(),
  iradIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(iradIndex+1)
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{}

IradCylinder::IradCylinder(const IradCylinder& A) : 
  attachSystem::ContainedComp(A),
  attachSystem::FixedOffset(A),
  attachSystem::CellMap(A),
  iradIndex(A.iradIndex),cellIndex(A.cellIndex),
  radius(A.radius),length(A.length),wallThick(A.wallThick),
  temp(A.temp),mat(A.mat),wallMat(A.wallMat)
  /*!
    Copy constructor
    \param A :: IradCylinder to copy
  */
{}

IradCylinder&
IradCylinder::operator=(const IradCylinder& A)
  /*!
    Assignment operator
    \param A :: IradCylinder to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      attachSystem::CellMap::operator=(A);
      cellIndex=A.cellIndex;
      radius=A.radius;
      length=A.length;
      wallThick=A.wallThick;
      temp=A.temp;
      mat=A.mat;
      wallMat=A.wallMat;
    }
  return *this;
}


  
IradCylinder*
IradCylinder::clone() const
 /*!
   Clone self 
   \return new (this)
 */
{
  return new IradCylinder(*this);
}

IradCylinder::~IradCylinder()
  /*!
    Destructor
  */
{}
  
void
IradCylinder::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("IradCylinder","populate");

  FixedOffset::populate(Control);
  
  radius=Control.EvalVar<double>(keyName+"Radius");
  length=Control.EvalVar<double>(keyName+"Length");
  wallThick=Control.EvalVar<double>(keyName+"WallThick");
  temp=Control.EvalVar<double>(keyName+"Temp");
  
  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  
  return;
}

void
IradCylinder::createUnitVector(const attachSystem::FixedComp& FC,
				      const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Centre for object
    \param sideIndex :: Inner link point
  */
{
  ELog::RegMethod RegA("IradCylinder","createUnitVector");
  attachSystem::FixedComp::createUnitVector(FC,sideIndex);

  return;
}


void
IradCylinder::createSurfaces()
  /*!
    Create planes for the inner structure iside DiskPreMod
  */
{
  ELog::RegMethod RegA("IradCylinder","createSurfaces");


  ModelSupport::buildPlane(SMap,iradIndex+1,Origin-Y*length/2.0,Y);
  ModelSupport::buildPlane(SMap,iradIndex+2,Origin+Y*length/2.0,Y);

  ModelSupport::buildCylinder(SMap,iradIndex+7,Origin,Y,radius);
  ModelSupport::buildCylinder(SMap,iradIndex+17,Origin,Y,radius+wallThick);
  
  ModelSupport::buildSphere(SMap,iradIndex+8,
                            Origin-Y*(length/2.0),radius);
  ModelSupport::buildSphere(SMap,iradIndex+18,
                            Origin-Y*(length/2.0),radius+wallThick);

  ModelSupport::buildSphere(SMap,iradIndex+9,
                            Origin+Y*(length/2.0),radius);
  ModelSupport::buildSphere(SMap,iradIndex+19,
                            Origin+Y*(length/2.0),radius+wallThick);

  return; 
}

void
IradCylinder::createObjects(Simulation& System)
/*!
    Create the objects
    \param System :: Simulation to add results
  */
{
  ELog::RegMethod RegA("IradCylinder","createObjects");

  std::string Out;
  // Centre
  Out=ModelSupport::getComposite(SMap,iradIndex," 1 -2 -7 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,mat,temp,Out));
  addCell("Centre",cellIndex-1);

  Out=ModelSupport::getComposite(SMap,iradIndex," -1 -8 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,mat,temp,Out));
  addCell("End",cellIndex-1);

  Out=ModelSupport::getComposite(SMap,iradIndex," 2 -9 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,mat,temp,Out));
  addCell("End",cellIndex-1);

  Out=ModelSupport::getComposite(SMap,iradIndex," 1 -2 -17 7 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,temp,Out));
  addCell("Wall",cellIndex-1);

  Out=ModelSupport::getComposite(SMap,iradIndex," -1 -18 8 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,temp,Out));
  addCell("Wall",cellIndex-1);

  Out=ModelSupport::getComposite(SMap,iradIndex," 2 -19 9 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,temp,Out));
  addCell("Wall",cellIndex-1);

  Out=ModelSupport::getComposite(SMap,iradIndex," (1:-18) (-2:-19) -17 ");
  addOuterSurf(Out);
  
  return; 
}
  
void
IradCylinder::createLinks()
  /*!
    Creates a full attachment set
  */
{  
  ELog::RegMethod RegA("IradCylinder","createLinks");

  FixedComp::setConnect(0,Origin-Y*(length+radius+wallThick),-Y);
  FixedComp::setLinkSurf(0,SMap.realSurf(iradIndex+18));
  FixedComp::setBridgeSurf(0,-SMap.realSurf(iradIndex+1));

  FixedComp::setConnect(1,Origin+Y*(length+radius+wallThick),Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(iradIndex+19));
  FixedComp::setBridgeSurf(1,SMap.realSurf(iradIndex+2));

  FixedComp::setConnect(2,Origin-X*(radius+wallThick),-X);
  FixedComp::setLinkSurf(2,SMap.realSurf(iradIndex+17));

  FixedComp::setConnect(3,Origin+X*(radius+wallThick),X);
  FixedComp::setLinkSurf(3,SMap.realSurf(iradIndex+17));

  FixedComp::setConnect(4,Origin-Z*(radius+wallThick),-Z);
  FixedComp::setLinkSurf(4,SMap.realSurf(iradIndex+17));

  FixedComp::setConnect(5,Origin+Z*(radius+wallThick),Z);
  FixedComp::setLinkSurf(5,SMap.realSurf(iradIndex+17));


  return;
}

void
IradCylinder::createAll(Simulation& System,
                        const attachSystem::FixedComp& FC,
                        const long int sideIndex)
  /*!
    External build everything
    \param System :: Simulation
    \param FC :: Attachment point - NOTE care with rotation
    \param sideIndex :: index for point
  */
{
  ELog::RegMethod RegA("IradCylinder","createAll");
  
  
  populate(System.getDataBase()); 
  createUnitVector(FC,sideIndex);
  
  createSurfaces();
  createObjects(System);
  createLinks();
  
  insertObjects(System);       
  return;
}
  
}  // NAMESPACE essSystem
