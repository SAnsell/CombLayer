/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   essBuild/KlystronGallery.cxx
 *
 * Copyright (c) 2018 by Konstantin Batkov
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
#include "Qhull.h"
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
#include "surfDBase.h"
#include "surfDIter.h"
#include "surfDivide.h"
#include "SurInter.h"
#include "mergeTemplate.h"

#include "KlystronGallery.h"

namespace essSystem
{

KlystronGallery::KlystronGallery(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(Key,9),
  surfIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(surfIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

KlystronGallery::KlystronGallery(const KlystronGallery& A) :
  attachSystem::ContainedComp(A),
  attachSystem::FixedOffset(A),
  surfIndex(A.surfIndex),cellIndex(A.cellIndex),
  lengthBack(A.lengthBack),
  lengthFront(A.lengthFront),
  widthLeft(A.widthLeft),
  widthRight(A.widthRight),
  height(A.height),
  depth(A.depth),
  wallThick(A.wallThick),
  roofThick(A.roofThick),
  floorThick(A.floorThick),
  roofAngle(A.roofAngle),
  airMat(A.airMat),wallMat(A.wallMat)
  /*!
    Copy constructor
    \param A :: KlystronGallery to copy
  */
{}

KlystronGallery&
KlystronGallery::operator=(const KlystronGallery& A)
  /*!
    Assignment operator
    \param A :: KlystronGallery to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      cellIndex=A.cellIndex;
      lengthBack=A.lengthBack;
      lengthFront=A.lengthFront;
      widthLeft=A.widthLeft;
      widthRight=A.widthRight;
      height=A.height;
      depth=A.depth;
      wallThick=A.wallThick;
      roofThick=A.roofThick;
      floorThick=A.floorThick;
      roofAngle=A.roofAngle;
      airMat=A.airMat;
      wallMat=A.wallMat;
    }
  return *this;
}

KlystronGallery*
KlystronGallery::clone() const
/*!
  Clone self
  \return new (this)
 */
{
    return new KlystronGallery(*this);
}

KlystronGallery::~KlystronGallery()
  /*!
    Destructor
  */
{}

void
KlystronGallery::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("KlystronGallery","populate");

  FixedOffset::populate(Control);

  lengthBack=Control.EvalVar<double>(keyName+"LengthBack");
  lengthFront=Control.EvalVar<double>(keyName+"LengthFront");
  widthLeft=Control.EvalVar<double>(keyName+"WidthLeft");
  widthRight=Control.EvalVar<double>(keyName+"WidthRight");
  height=Control.EvalVar<double>(keyName+"Height");
  depth=Control.EvalVar<double>(keyName+"Depth");
  wallThick=Control.EvalVar<double>(keyName+"WallThick");
  roofThick=Control.EvalVar<double>(keyName+"RoofThick");
  floorThick=Control.EvalVar<double>(keyName+"FloorThick");
  roofAngle=Control.EvalVar<double>(keyName+"RoofAngle");

  airMat=ModelSupport::EvalMat<int>(Control,keyName+"AirMat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");

  return;
}

void
KlystronGallery::createUnitVector(const attachSystem::FixedComp& FC,
			      const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: object for origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("KlystronGallery","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();

  return;
}

void
KlystronGallery::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("KlystronGallery","createSurfaces");

  ModelSupport::buildPlane(SMap,surfIndex+1,Origin-Y*(lengthBack),Y);
  ModelSupport::buildPlane(SMap,surfIndex+2,Origin+Y*(lengthFront),Y);

  ModelSupport::buildPlane(SMap,surfIndex+3,Origin-X*(widthLeft),X);
  ModelSupport::buildPlane(SMap,surfIndex+4,Origin+X*(widthRight),X);

  ModelSupport::buildPlane(SMap,surfIndex+5,Origin-Z*(depth),Z);

  Geometry::Vec3D topNorm(Z);
  Geometry::Quaternion::calcQRotDeg(-roofAngle,Y).rotate(topNorm);

  ModelSupport::buildPlane(SMap,surfIndex+6,
			   Origin-X*(widthLeft)+Z*(height),topNorm);

  ModelSupport::buildPlane(SMap,surfIndex+11,Origin-Y*(lengthBack+wallThick),Y);
  ModelSupport::buildPlane(SMap,surfIndex+12,Origin+Y*(lengthFront+wallThick),Y);

  ModelSupport::buildPlane(SMap,surfIndex+13,Origin-X*(widthLeft+wallThick),X);
  ModelSupport::buildPlane(SMap,surfIndex+14,Origin+X*(widthRight+wallThick),X);

  ModelSupport::buildPlane(SMap,surfIndex+15,Origin-Z*(depth+floorThick),Z);
  ModelSupport::buildShiftedPlane(SMap,surfIndex+16,
				  SMap.realPtr<Geometry::Plane>(surfIndex+6),
				  roofThick);

  return;
}

void
KlystronGallery::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("KlystronGallery","createObjects");

  std::string Out;
  Out=ModelSupport::getComposite(SMap,surfIndex," 1 -2 3 -4 5 -6 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,airMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,surfIndex," 11 -12 13 -14 15 -16 (-1:2:-3:4:-5:6) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,surfIndex," 11 -12 13 -14 15 -16 ");
  addOuterSurf(Out);

  return;
}


void
KlystronGallery::createLinks()
  /*!
    Create all the linkes
  */
{
  ELog::RegMethod RegA("KlystronGallery","createLinks");

  const Geometry::Vec3D midY(Y*((lengthFront-lengthBack)/2.0));

  FixedComp::setConnect(0,Origin-Y*(lengthBack+wallThick),-Y);
  FixedComp::setLinkSurf(0,-SMap.realSurf(surfIndex+11));

  FixedComp::setConnect(1,Origin+Y*(lengthFront+wallThick),Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(surfIndex+12));

  FixedComp::setConnect(2,Origin-X*(widthLeft+wallThick)+midY,-X);
  FixedComp::setLinkSurf(2,-SMap.realSurf(surfIndex+13));

  FixedComp::setConnect(3,Origin+X*(widthRight+wallThick)+midY,X);
  FixedComp::setLinkSurf(3,SMap.realSurf(surfIndex+14));

  FixedComp::setConnect(4,Origin-Z*(depth+floorThick)+midY,-Z);
  FixedComp::setLinkSurf(4,-SMap.realSurf(surfIndex+15));

  // highest roof x+ corner
  FixedComp::setConnect(5,Origin-X*(widthLeft)+
			Z*(height+roofThick/cos(roofAngle*M_PI/180))+midY,Z);
  FixedComp::setLinkSurf(5,SMap.realSurf(surfIndex+16));

  FixedComp::setConnect(6,Origin-X*(widthLeft)+midY,-X);
  FixedComp::setLinkSurf(6,-SMap.realSurf(surfIndex+3));

  FixedComp::setConnect(7,Origin-Z*(depth)+midY,Z);
  FixedComp::setLinkSurf(7,SMap.realSurf(surfIndex+5));

  FixedComp::setConnect(8,Origin-X*(widthLeft)+Z*(height)+midY,-Z);
  FixedComp::setLinkSurf(8,-SMap.realSurf(surfIndex+6));

   // for (int i=0; i<9; i++)
   //    ELog::EM << "KG lp " << i << ":\t" << getLinkSurf(i+1) << " " << getLinkPt(i+1) << ELog::endDiag;


  return;
}




void
KlystronGallery::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("KlystronGallery","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // essSystem
