/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   insertUnit/insertObject.cxx
 *
 * Copyright (c) 2004-2023 by Stuart Ansell
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
#include <algorithm>
#include <cmath>
#include <complex>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "surfRegister.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "MaterialSupport.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedRotate.h"
#include "BaseMap.h"
#include "SurfMap.h"
#include "CellMap.h"
#include "ContainedComp.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "AttachSupport.h"
#include "insertObject.h"

namespace insertSystem
{

insertObject::insertObject(const std::string& Key)  :
  attachSystem::FixedRotate(Key,6),
  attachSystem::ContainedComp(),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  attachSystem::FrontBackCut(),
  populated(0),defMat(0),delayInsert(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

insertObject::insertObject(std::string  baseKey,
			   const std::string& Key)  :
  attachSystem::FixedRotate(Key,6),
  attachSystem::ContainedComp(),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  attachSystem::FrontBackCut(),
  baseName(std::move(baseKey)),populated(0),defMat(0),delayInsert(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

insertObject::insertObject(const insertObject& A) :  
  attachSystem::FixedRotate(A),
  attachSystem::ContainedComp(A),
  attachSystem::CellMap(A),
  attachSystem::SurfMap(A),
  attachSystem::FrontBackCut(A),
  baseName(A.baseName),populated(A.populated),
  defMat(A.defMat),delayInsert(A.delayInsert)
  /*!
    Copy constructor
    \param A :: insertObject to copy
  */
{}

insertObject&
insertObject::operator=(const insertObject& A)
  /*!
    Assignment operator
    \param A :: insertObject to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedRotate::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      attachSystem::CellMap::operator=(A);
      attachSystem::SurfMap::operator=(A);
      attachSystem::FrontBackCut::operator=(A);
      populated=A.populated;
      defMat=A.defMat;
      delayInsert=A.delayInsert;
    }
  return *this;
}

insertObject::~insertObject() 
  /*!
    Destructor
  */
{}

void
insertObject::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Data Base
  */
{
  ELog::RegMethod RegA("insertObject","populate");
  
  if (!populated)
    {
      FixedRotate::populate(baseName,Control);      
      defMat=ModelSupport::EvalMat<int>
	(Control,keyName+"Mat",baseName+"Mat");
      populated=1;
    }
  return;
}
  
void
insertObject::findObjects(Simulation& System)
  /*!
    Insert the objects into the main simulation. It is separated
    from creation since we need to determine those object that 
    need to have an exclude item added to them.
    \param System :: Simulation to add object to
  */
{
  ELog::RegMethod RegA("insertObject","findObjects");

  typedef std::map<int,MonteCarlo::Object*> MTYPE;
  
  System.populateCells();
  System.validateObjSurfMap();


  MTYPE OMap;
  attachSystem::lineIntersect(System,*this,OMap);
  
  // Add exclude string
  MTYPE::const_iterator ac;
  for(ac=OMap.begin();ac!=OMap.end();ac++)
    attachSystem::ContainedComp::addInsertCell(ac->first);
  
  
  return;
}

void
insertObject::setStep(const double XS,const double YS,
		     const double ZS)
  /*!
    Set the values but NOT the populate flag
    \param XS :: X-step [width]
    \param YS :: Y-step [depth] 
    \param ZS :: Z-step [height]
   */
{
  xStep=XS;
  yStep=YS;
  zStep=ZS;
  return;
}

void
insertObject::setStep(const Geometry::Vec3D& XYZ)
  /*!
    Set the values but NOT the populate flag
    \param XYZ :: X/Y/Z
   */
{
  xStep=XYZ[0];
  yStep=XYZ[1];
  zStep=XYZ[2];
  return;
}

void
insertObject::setAngles(const double XS,const double ZA)
  /*!
    Set the values but NOT the populate flag
    \param XY :: XY angle
    \param ZA :: Z angle
   */
{
  zAngle=XS;
  xAngle=ZA;
  return;
}

 
  
}  // NAMESPACE insertSystem
