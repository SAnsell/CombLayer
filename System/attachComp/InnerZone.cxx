 /********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   attachComp/InnerZone.cxx
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
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "AttachSupport.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "BaseMap.h"
#include "CellMap.h"

#include "InnerZone.h"

namespace attachSystem
{
 
InnerZone::InnerZone(attachSystem::FixedComp& FRef,
		     int& cRef) :
  FCName(FRef.getKeyName()),cellIndex(cRef),
  FCPtr(&FRef),
  CellPtr(dynamic_cast<CellMap*>(&FRef))
  /*!
    Add a fixed points to the system -- it is expected
    that this can be cast to a cellmap.
    \param FPtr :: Fixed Pointer
    \param cRef :: Reference to cell index [normaly FRef.cellIndex]
  */
{
  if (!CellPtr)
    ELog::EM<<"Error with InnerZone not having CellMap"<<ELog::endErr;
}


void
InnerZone::setSurround(const HeadRule& HR)
  /*!
    Set the capture/surround rule
    \param HR :: Surround rule [inward]
  */
{
  surroundHR=HR;
  return;
}
  
void
InnerZone::setFront(const HeadRule& HR)
  /*!
    Set the front facing rule
    \param HR :: Rule at front [inward]
  */
{
  frontHR=HR;
  return;
}
  
void
InnerZone::setBack(const HeadRule& HR)
  /*!
    Set the back facing rule
    \param HR :: Rule at back [inward]
  */
{
  backHR=HR;
  return;
}
  
int
InnerZone::createOuterVoidUnit(Simulation& System,
			       MonteCarlo::Object& masterCell,
			       HeadRule& frontDivider,
			       const attachSystem::FixedComp& FC,
			       const long int sideIndex)
  /*!
    Construct outer void object main pipe
    \param System :: Simulation
    \param masterCell :: full master cell
    \param FC :: FixedComp
    \param sideIndex :: link point
    \return cell nubmer
  */
{
  ELog::RegMethod RegA("InnerZone","createOuterVoidUnit");

    // construct an cell based on previous cell:
  std::string Out;
  
  if (!frontDivider.hasRule())
    frontDivider=frontHR;

  const HeadRule& backDivider=
    (sideIndex) ? FC.getFullRule(-sideIndex) :
    backHR;
  
  Out=surroundHR.display()+
    frontDivider.display()+backDivider.display();
  CellPtr->makeCell("OuterVoid",System,cellIndex++,0,0.0,Out);
  frontDivider=backDivider;
  
  // make the master cell valid:
  
  frontDivider.makeComplement();

  refrontMasterCell(masterCell,FC,sideIndex);
  return cellIndex-1;
}

void
InnerZone::refrontMasterCell(MonteCarlo::Object& MCell,
			     const attachSystem::FixedComp& FC,
			     const long int sideIndex) const
  /*!
    This horrifc function to re-build MCell so that it is correct
    as createOuterVoid consumes the front of the master cell
    \param MCell :: master cell object
    \param FC :: FixedComp
    \param sideIndex :: side index for back of FC object
  */
{
  ELog::RegMethod RegA("InnerZone","refrontMasterCell");

  std::string Out;  

  Out+=surroundHR.display() + backHR.display()+
    FC.getLinkString(sideIndex);
  MCell.procString(Out);
  return;
}
 

MonteCarlo::Object&
InnerZone::constructMasterCell(Simulation& System)
 /*!
    Construct outer void object main pipe
    \param System :: Simulation
    \return cell object
  */
{
  ELog::RegMethod RegA("balderFrontEnd","constructMasterCell");

  std::string Out;
  
  Out+=surroundHR.display() + backHR.display()+ frontHR.display();
  CellPtr->makeCell("Mastervoid",System,cellIndex++,0,0.0,Out);
  MonteCarlo::Object* OPtr= System.findQhull(cellIndex-1);
  return *OPtr;
}


}  // NAMESPACE attachSystem
