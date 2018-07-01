 /********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   attachComp/ContainedSpace.cxx
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
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <cmath>
#include <complex>
#include <list>
#include <vector>
#include <set>
#include <map>
#include <deque>
#include <string>
#include <algorithm>
#include <iterator>
#include <memory>
#include <functional>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "writeSupport.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Surface.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Sphere.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "SurInter.h"
#include "Rules.h"
#include "HeadRule.h"
#include "Object.h"
#include "Line.h"
#include "LineIntersectVisit.h"
#include "Qhull.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "Simulation.h"
#include "AttachSupport.h"
#include "SurInter.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "ContainedComp.h"
#include "SpaceCut.h"
#include "ContainedSpace.h"


namespace attachSystem
{

ContainedSpace::ContainedSpace()  :
  SpaceCut(),ContainedComp()
  /*!
    Constructor 
  */
{}

ContainedSpace::ContainedSpace(const ContainedSpace& A) : 
  SpaceCut(A),ContainedComp(A)
  /*!
    Copy constructor
    \param A :: ContainedSpace to copy
  */
{}

ContainedSpace&
ContainedSpace::operator=(const ContainedSpace& A)
  /*!
    Assignment operator
    \param A :: ContainedSpace to copy
    \return *this
  */
{
  if (this!=&A)
    {
      SpaceCut::operator=(A);
      ContainedComp::operator=(A);
    }
  return *this;
}

ContainedSpace::~ContainedSpace()
  /*!
    Deletion operator
  */
{}
  
void
ContainedSpace::insertObjects(Simulation& System,
			      attachSystem::FixedComp& FC)
  /*!
    Assumption that we can put this object into a space
    Makes the object bigger 
    Then constructs the outer boundary as if that object is 
    the outer boundary!
    \param System :: simulation system
    \param FC :: Fixed Comp for cell information
  */
{
  ELog::RegMethod RegA("ContainedSpace","insertObjects(FC)");

  if (!primaryCell && !insertCells.empty())
    primaryCell=insertCells.front();

  SpaceCut::insertObjects(System,FC);

  if (buildCell)
    ContainedComp::addInsertCell(buildCell);
  if (FCName=="BalderOpticsHutChicane0")
    ELog::EM<<"FC= "<<FCName<<" "<<buildCell<<ELog::endDiag;
  if (SpaceCut::insertValid())
    {
      StrFunc::removeItem(insertCells,SpaceCut::getPrimaryCell());
      ContainedComp::insertObjects(System);
    }
  else if (SpaceCut::hasPrimaryInsert())
    ContainedComp::insertObjects(System);

  return;
}

void
ContainedSpace::insertObjects(Simulation& System)
  /*!
    Assumption that we can put this object into a space
    Makes the object bigger 
    Then constructs the outer boundary as if that object is 
    the outer boundary!
    \param System :: simulation system
  */
{
  ELog::RegMethod RegA("ContainedSpace","insertObjects");


  if (!primaryCell && !insertCells.empty())
    primaryCell=insertCells.front();

  attachSystem::FixedComp* FC=
    dynamic_cast<attachSystem::FixedComp*>(this);

  if (FC)
    SpaceCut::insertObjects(System,*FC);
  else
    SpaceCut::insertObjects(System);
  
  if (buildCell)
    ContainedComp::addInsertCell(buildCell);
  
  if (SpaceCut::insertValid())
    {
      StrFunc::removeItem(insertCells,SpaceCut::getPrimaryCell());
      ContainedComp::insertObjects(System);
    }
  else if (SpaceCut::hasPrimaryInsert())
    ContainedComp::insertObjects(System);

  return;
}
			  
}  // NAMESPACE attachSystem
