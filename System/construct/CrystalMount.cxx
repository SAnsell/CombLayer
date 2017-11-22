/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/CrystalMount.cxx
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
#include "surfRegister.h"
#include "objectRegister.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "Quadratic.h"
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
#include "inputParam.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ContainedComp.h"
#include "CrystalMount.h"


namespace constructSystem
{

CrystalMount::CrystalMount(const std::string& Key,
                           const size_t Index) :
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(Key+StrFunc::makeString(Index),8),
  attachSystem::CellMap(),attachSystem::SurfMap(),
  baseName(Key),ID(Index),
  xtalIndex(ModelSupport::objectRegister::Instance().cell(keyName)),
  cellIndex(xtalIndex+1)
  /*!
    Constructor
    \param Key :: Name of construction key
    \param Index :: Index number
  */
{}

CrystalMount::CrystalMount(const CrystalMount& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedOffset(A),
  attachSystem::CellMap(A),attachSystem::SurfMap(A),
  baseName(A.baseName),ID(A.ID),xtalIndex(A.xtalIndex),
  cellIndex(A.cellIndex),active(A.active),width(A.width),
  thick(A.thick),length(A.length),gap(A.gap),wallThick(A.wallThick),
  baseThick(A.baseThick),xtalMat(A.xtalMat),wallMat(A.wallMat),
  yRotation(A.yRotation),zRotation(A.zRotation),
  viewPoint(A.viewPoint)
  /*!
    Copy constructor
    \param A :: CrystalMount to copy
  */
{}

CrystalMount&
CrystalMount::operator=(const CrystalMount& A)
  /*!
    Assignment operator
    \param A :: CrystalMount to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      attachSystem::CellMap::operator=(A);
      attachSystem::SurfMap::operator=(A);
      cellIndex=A.cellIndex;
      active=A.active;
      width=A.width;
      thick=A.thick;
      length=A.length;
      gap=A.gap;
      wallThick=A.wallThick;
      baseThick=A.baseThick;
      xtalMat=A.xtalMat;
      wallMat=A.wallMat;
      yRotation=A.yRotation;
      zRotation=A.zRotation;
      viewPoint=A.viewPoint;
    }
  return *this;
}


CrystalMount::~CrystalMount()
  /*!
    Destructor
   */
{}

void
CrystalMount::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("CrystalMount","populate");

  FixedOffset::populate(Control);

  active=Control.EvalDefPair<int>(keyName,baseName,"Active",1);
  width=Control.EvalPair<double>(keyName,baseName,"Width");
  thick=Control.EvalPair<double>(keyName,baseName,"Thick");
  length=Control.EvalPair<double>(keyName,baseName,"Length");

  gap=Control.EvalPair<double>(keyName,baseName,"Gap");
  wallThick=Control.EvalPair<double>(keyName,baseName,"WallThick");
  baseThick=Control.EvalDefPair<double>(keyName,baseName,"BaseThick",wallThick);

  xtalMat=ModelSupport::EvalMat<int>(Control,keyName+"XtalMat",
                                     baseName+"XtalMat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat",
                                     baseName+"WallMat");

  yRotation=Control.EvalPair<double>(keyName,baseName,"YRotation");
  zRotation=Control.EvalPair<double>(keyName,baseName,"ZRotation");
  return;
}

void
CrystalMount::createUnitVector(const attachSystem::FixedComp& FC,
                               const long int sideIndex)
  /*!
    Create the unit vectors.
    Note that it also set the view point that neutrons come from
    \param FC :: FixedComp for origin
    \param sideIndex :: direction for link
  */
{
  ELog::RegMethod RegA("CrystalMount","createUnitVector");
  attachSystem::FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
  viewPoint=FC.getLinkPt(sideIndex);

  applyFullRotate(0,yRotation,0,viewPoint);
  applyFullRotate(0,zRotation,viewPoint);
  
  return;
}

void
CrystalMount::createSurfaces()
  /*!
    Create planes for the silicon and Polyethene layers
  */
{
  ELog::RegMethod RegA("CrystalMount","createSurfaces");

  // main xstal

  ModelSupport::buildPlane(SMap,xtalIndex+1,Origin,Y);
  ModelSupport::buildPlane(SMap,xtalIndex+2,Origin+Y*thick,Y);
  ModelSupport::buildPlane(SMap,xtalIndex+3,Origin-X*(length/2.0),X);
  ModelSupport::buildPlane(SMap,xtalIndex+4,Origin+X*(length/2.0),X);
  ModelSupport::buildPlane(SMap,xtalIndex+5,Origin-Z*(width/2.0),Z);
  ModelSupport::buildPlane(SMap,xtalIndex+6,Origin+Z*(width/2.0),Z);    
  
  // inner void
  ModelSupport::buildPlane(SMap,xtalIndex+12,Origin+Y*(thick+gap),Y);
  ModelSupport::buildPlane(SMap,xtalIndex+13,Origin-X*(gap+length/2.0),X);
  ModelSupport::buildPlane(SMap,xtalIndex+14,Origin+X*(gap+length/2.0),X);
  ModelSupport::buildPlane(SMap,xtalIndex+15,Origin-Z*(gap+width/2.0),Z);
  ModelSupport::buildPlane(SMap,xtalIndex+16,Origin+Z*(gap+width/2.0),Z);    

  // inner void
  ModelSupport::buildPlane(SMap,xtalIndex+22,Origin+Y*(thick+gap+baseThick),Y);
  ModelSupport::buildPlane(SMap,xtalIndex+23,
			   Origin-X*(wallThick+gap+length/2.0),X);
  ModelSupport::buildPlane(SMap,xtalIndex+24,
			   Origin+X*(wallThick+gap+length/2.0),X);
  ModelSupport::buildPlane(SMap,xtalIndex+25,
			   Origin-Z*(wallThick+gap+width/2.0),Z);
  ModelSupport::buildPlane(SMap,xtalIndex+26,
			   Origin+Z*(wallThick+gap+width/2.0),Z);
  
  return; 
}

void
CrystalMount::createObjects(Simulation& System)
  /*!
    Create the vaned moderator
    \param System :: Simulation to add results
   */
{
  ELog::RegMethod RegA("CrystalMount","createObjects");

  std::string Out;
  if (active)
    {
      // xstal
      Out=ModelSupport::getComposite(SMap,xtalIndex," 1 -2 3 -4 5 -6 ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,xtalMat,0.0,Out));
      addCell("Xtal",cellIndex-1);
      
      
      Out=ModelSupport::getComposite
        (SMap,xtalIndex," 1 -12 13 -14 15 -16 (2:-3:4:-5:6) ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
      
      Out=ModelSupport::getComposite
        (SMap,xtalIndex," 1 -22 23 -24 25 -26 (12:-13:14:-15:16) ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));
      
      Out=ModelSupport::getComposite(SMap,xtalIndex," 1 -22 23 -24 25 -26 ");
      addOuterSurf(Out);
    }
  return; 
}

void
CrystalMount::createLinks()
  /*!
    Creates a full attachment set
  */
{
  ELog::RegMethod RegA("CrystalMount","createLinks");
  
  FixedComp::setConnect(0,Origin,-Y);
  FixedComp::setLinkSurf(0,-SMap.realSurf(xtalIndex+1));

  FixedComp::setConnect(1,Origin+Y*(gap+thick+baseThick),-Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(xtalIndex+22));

  const Geometry::Vec3D MidY(Origin+Y*((thick+gap+baseThick)/2.0));
  const double T(gap+wallThick);
  
  FixedComp::setConnect(2,MidY-X*(T+width/2.0),-X);
  FixedComp::setLinkSurf(2,-SMap.realSurf(xtalIndex+23));

  FixedComp::setConnect(3,MidY+X*(T+width/2.0),X);
  FixedComp::setLinkSurf(3,SMap.realSurf(xtalIndex+24));

  FixedComp::setConnect(4,MidY-Z*(T+length/2.0),-Z);
  FixedComp::setLinkSurf(4,-SMap.realSurf(xtalIndex+25));

  FixedComp::setConnect(5,MidY+Z*(T+length/2.0),Z);
  FixedComp::setLinkSurf(5,SMap.realSurf(xtalIndex+26));


  const Geometry::Vec3D incomingAxis=(Origin-viewPoint).unit();
  FixedComp::setConnect(6,viewPoint,-incomingAxis);
  FixedComp::setLinkSurf(6,SMap.realSurf(xtalIndex+1));

  const Geometry::Vec3D outgoingAxis=
    -Y*(2.0*incomingAxis.dotProd(Y))+incomingAxis;
  FixedComp::setConnect(7,Origin,outgoingAxis);
  FixedComp::setLinkSurf(7,-SMap.realSurf(xtalIndex+1));

  return;
}

void
CrystalMount::createAll(Simulation& System,
			const attachSystem::FixedComp& FC,
		      const long int sideIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComp to construct from
    \param sideIndex :: Side point
   */
{
  ELog::RegMethod RegA("CrystalMount","createAll");
  populate(System.getDataBase());

  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       

  return;
}

}  // NAMESPACE constructSystem
