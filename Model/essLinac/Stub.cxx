/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/Stub.cxx
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
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "surfDBase.h"
#include "surfDIter.h"
#include "surfDivide.h"
#include "SurInter.h"
#include "mergeTemplate.h"
#include "FrontBackCut.h"
#include "Stub.h"

namespace essSystem
{

Stub::Stub(const std::string& Key)  :
  attachSystem::ContainedGroup(),
  attachSystem::FixedOffset(Key,6),
  attachSystem::FrontBackCut(),
  surfIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(surfIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

Stub::Stub(const Stub& A) : 
  attachSystem::ContainedGroup(A),
  attachSystem::FixedOffset(A),
  attachSystem::FrontBackCut(A),
  surfIndex(A.surfIndex),cellIndex(A.cellIndex),
  engActive(A.engActive),
  length(A.length),width(A.width),height(A.height),
  wallThick(A.wallThick),
  mainMat(A.mainMat),wallMat(A.wallMat)
  /*!
    Copy constructor
    \param A :: Stub to copy
  */
{}

Stub&
Stub::operator=(const Stub& A)
  /*!
    Assignment operator
    \param A :: Stub to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedGroup::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      attachSystem::FrontBackCut::operator=(A);
      cellIndex=A.cellIndex;
      engActive=A.engActive;
      length=A.length;
      width=A.width;
      height=A.height;
      wallThick=A.wallThick;
      mainMat=A.mainMat;
      wallMat=A.wallMat;
    }
  return *this;
}

Stub*
Stub::clone() const
/*!
  Clone self
  \return new (this)
 */
{
    return new Stub(*this);
}
  
Stub::~Stub() 
  /*!
    Destructor
  */
{}

void
Stub::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("Stub","populate");

  FixedOffset::populate(Control);
  engActive=Control.EvalPair<int>(keyName,"","EngineeringActive");

  const size_t Nlegs(3);
  for (size_t i=0; i<Nlegs-1; i++)
    {
      const double L = Control.EvalVar<double>(keyName+"Length"+
					       std::to_string(i+1));
      length.push_back(L);
    }

  width=Control.EvalVar<double>(keyName+"Width");
  height=Control.EvalVar<double>(keyName+"Height");
  wallThick=Control.EvalVar<double>(keyName+"WallThick");

  mainMat=ModelSupport::EvalMat<int>(Control,keyName+"MainMat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");

  return;
}
  
void
Stub::createUnitVector(const attachSystem::FixedComp& FC,
			      const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: object for origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("Stub","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();

  return;
}
  
void
Stub::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("Stub","createSurfaces");

  ModelSupport::buildPlane(SMap,surfIndex+1,Origin-Y*(width/2.0),Y);
  ModelSupport::buildPlane(SMap,surfIndex+2,Origin+Y*(width/2.0),Y);

  ModelSupport::buildPlane(SMap,surfIndex+4,Origin+X*(length[0]),X);

  ModelSupport::buildPlane(SMap,surfIndex+5,Origin-Z*(height/2.0),Z);
  ModelSupport::buildPlane(SMap,surfIndex+6,Origin+Z*(height/2.0),Z);

  ModelSupport::buildPlane(SMap,surfIndex+11,Origin-Y*(width/2.0+wallThick),Y);
  ModelSupport::buildPlane(SMap,surfIndex+12,Origin+Y*(width/2.0+wallThick),Y);

  ModelSupport::buildPlane(SMap,surfIndex+14,Origin+X*(length[0]+wallThick),X);

  ModelSupport::buildPlane(SMap,surfIndex+15,Origin-Z*(height/2.0+wallThick),Z);
  ModelSupport::buildPlane(SMap,surfIndex+16,Origin+Z*(height/2.0+wallThick),Z);

  ModelSupport::buildShiftedPlane(SMap,surfIndex+104,
				  SMap.realPtr<Geometry::Plane>(surfIndex+4),
				  -height);
    
  ModelSupport::buildShiftedPlane(SMap,surfIndex+105,
				  SMap.realPtr<Geometry::Plane>(surfIndex+5),
				  length[1]-height);

  ModelSupport::buildShiftedPlane(SMap,surfIndex+106,
				  SMap.realPtr<Geometry::Plane>(surfIndex+5),
				  length[1]);

  ModelSupport::buildShiftedPlane(SMap,surfIndex+114,
				  SMap.realPtr<Geometry::Plane>(surfIndex+104),
				  -wallThick);

  ModelSupport::buildShiftedPlane(SMap,surfIndex+115,
				  SMap.realPtr<Geometry::Plane>(surfIndex+105),
				  -wallThick);

  ModelSupport::buildShiftedPlane(SMap,surfIndex+116,
				  SMap.realPtr<Geometry::Plane>(surfIndex+106),
				  wallThick);
  return;
}
  
void
Stub::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("Stub","createObjects");

  std::string Out;
  attachSystem::ContainedGroup::addCC("Full");

  attachSystem::ContainedGroup::addCC("Leg1");

  Out=ModelSupport::getComposite(SMap,surfIndex," 1 -2 -4 5 -6 ")+backRule();
  System.addCell(MonteCarlo::Qhull(cellIndex++,mainMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,surfIndex," 11 -12 -4 15 -16 (-1:2:4:-5) ")+backRule();
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,surfIndex," 1 -2 -104 6 -16 ")+backRule();
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,surfIndex," 11 -12 -14 15 -16 ")+backRule();
  addOuterSurf("Leg1",Out);
  addOuterUnionSurf("Full",Out);

  attachSystem::ContainedGroup::addCC("Leg2");

  Out=ModelSupport::getComposite(SMap,surfIndex," 1 -2 104 -4 6 -106 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,mainMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,surfIndex," 11 -12 4 -14 15 -105 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,surfIndex,
   				 " 11 -12 114 -4 16 -105 (-1:2:-104:4:-16:105) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,surfIndex," 11 -12 114 -14 6 -116 ");
  addOuterUnionSurf("Leg2",Out);
  addOuterUnionSurf("Full",Out);

  attachSystem::ContainedGroup::addCC("Leg3");
  Out=ModelSupport::getComposite(SMap,surfIndex," 1 -2 4 105 -106 ")+frontRule();
  System.addCell(MonteCarlo::Qhull(cellIndex++,mainMat,0.0,Out));
  addOuterUnionSurf("Leg3",Out);
  addOuterUnionSurf("Full",Out);

  return;
}

  
void
Stub::createLinks()
  /*!
    Create all the linkes
  */
{
  ELog::RegMethod RegA("Stub","createLinks");

  //  FrontBackCut::createLinks(*this, Origin, Y);
  
  //  FixedComp::setConnect(0,Origin,-Y);
  //  FixedComp::setLinkSurf(0,-SMap.realSurf(surfIndex+1));
  
  return;
}
  
  

  
void
Stub::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("Stub","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);              

  return;
}

}  // essSystem
