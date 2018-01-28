/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/portItem.cxx
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#include <array>

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
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Cylinder.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Rules.h"
#include "HeadRule.h"
#include "Object.h"
#include "Line.h"
#include "Qhull.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "generateSurf.h"
#include "AttachSupport.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "LineTrack.h"
#include "BaseMap.h"
#include "CellMap.h"

#include "portItem.h"

namespace constructSystem
{

portItem::portItem(const std::string& Key) :
  attachSystem::FixedComp(Key,2),
  attachSystem::ContainedComp(),statusFlag(0),
  portIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(portIndex+1),radius(0.0),wall(0.0),
  flangeRadius(0.0),flangeThick(0.0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

  
portItem::~portItem() 
  /*!
    Destructor
  */
{}

void
portItem::setMain(const double L,const double R,const double WT)
  /*!
    Set parameters
    \param L :: external length
    \param R :: Internal radius
    \param WT :: Wall thick
    
  */
{
  externalLength=L;
  radius=R;
  wall=WT;
  return;
}

void
portItem::setFlange(const double FR,const double FT)
  /*!
    Set flange parameters
    \param FR :: Flange radius
    \param FT :: Flange thick
  */
{
  flangeRadius=FR;
  flangeThick=FT;
  return;
}

void
portItem::createUnitVector(const attachSystem::FixedComp& FC,
			   const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("portItem","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  refComp=FC.getKeyName();
  statusFlag |= 1;
  return;
}

void
portItem::setCentLine(const Geometry::Vec3D& O,
		      const Geometry::Vec3D& A)
  /*!
    Set position
   */
{
  Origin=O;
  FixedComp::reOrientate(1,A.unit());  
  return;
}

  
void
portItem::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("portItem","createSurfaces");

  ModelSupport::buildCylinder(SMap,portIndex+7,Origin,Y,radius);
  ModelSupport::buildCylinder(SMap,portIndex+17,Origin,Y,radius+wall);
  ModelSupport::buildCylinder(SMap,portIndex+27,Origin,Y,
			      radius+wall+flangeThick);
  
  return;
}

void
portItem::createObjects(Simulation& System)
  /*!
    Adds the vacuum box
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("portItem","createObjects");

  std::string Out;

  return;
}

void
portItem::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane.
    Port position are used for first two link points
    Note that 0/1 are the flange surfaces
  */
{
  ELog::RegMethod RegA("portItem","createLinks");
  
  
  return;
}

void
portItem::constructTrack(Simulation& System)
  /*!
    Construct a track system
   */
{
  ELog::RegMethod RegA("portItem","constructTrack");

  const ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  if ((statusFlag & 1)!=1)
    {
      ELog::EM<<"Failed to set in port:"<<keyName<<ELog::endCrit;
      return;
    }
  System.populateCells();
  System.validateObjSurfMap();

  ModelSupport::LineTrack LT(Origin,Y,-1.0);
  LT.calculate(System);
  
  const std::vector<MonteCarlo::Object*>& OVec=LT.getObjVec();

  for(const MonteCarlo::Object* oPtr : OVec)
    {	  
      const int ONum=oPtr->getName();
      if (OR.hasCell(refComp,ONum))
	{
	  ELog::EM<<"Cell == "<<ONum<<ELog::endDiag;
	}
      //      if (OMap.find(ONum)==OMap.end())
      //	OMap.emplace(ONum,oc);
    }

  return;
}
  
}  // NAMESPACE constructSystem
