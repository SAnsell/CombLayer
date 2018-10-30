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
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "generateSurf.h"

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



InnerZone::InnerZone(const InnerZone& A) : 
  FCName(A.FCName),cellIndex(A.cellIndex),FCPtr(A.FCPtr),
  CellPtr(A.CellPtr),surroundHR(A.surroundHR),frontHR(A.frontHR),
  backHR(A.backHR),middleHR(A.middleHR),frontDivider(A.frontDivider)
  /*!
    Copy constructor
    \param A :: InnerZone to copy
  */
{}


InnerZone&
InnerZone::operator=(const InnerZone& A)
  /*!
    Assignment operator
    \param A :: InnerZone to copy
    \return *this
  */
{
  if (this!=&A)
    {
      FCName=A.FCName;
      surroundHR=A.surroundHR;
      frontHR=A.frontHR;
      backHR=A.backHR;
      middleHR=A.middleHR;
      frontDivider=A.frontDivider;
    }
  return *this;
}

InnerZone
InnerZone::buildMiddleZone(const int flag) const
  /*!
    Construct a middle division unit based on
    the direction flag 
    \param flag :: -ve for left +ve for right 
      0 for forward unit
    \return new InnerZone
   */
{
  ELog::RegMethod RegA("InnerZone","buildMiddleZone");
  InnerZone BUnit(*this);
  BUnit.addMiddleToSurround(flag);
  BUnit.frontHR=frontDivider;
  ELog::EM<<"Front Divider == "<<frontDivider<<ELog::endDiag;
  return BUnit;
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
InnerZone::addMiddleToSurround(const int flag)
  /*!
    Set the capture/surround rule
    \param Flag :: +/- surface [0 to only reset middle]
  */
{
  ELog::RegMethod RegA("InnerZone","addMiddleToSurround");
  if (flag>0)
    surroundHR.addIntersection(middleHR);
  else
    surroundHR.addIntersection(middleHR.complement());
  middleHR.reset();
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

void
InnerZone::setMiddle(const HeadRule& HR)
  /*!
    Set the front facing rule
    \param HR :: Rule at front [inward]
  */
{
  middleHR=HR;
  return;
}

void
InnerZone::constructMiddleSurface(ModelSupport::surfRegister& SMap,
				  const int surfID,
				  const attachSystem::FixedComp& FC,
				  const long int sideIndex)
  /*!
    Construct a middle surface based on a link point
    The surfaces normal is orthogonal to the y axis of the link point
    and the Z axis of the FixedComp
    \param SMap :: Surface register
    \param surfID :: new surface number
    \param FC :: Fixed point for axis
    \param sideIdnex :: link axis
   */
{
  ELog::RegMethod RegA("InnerZone","constructMiddleSurface");
  
  
  const Geometry::Vec3D DPoint(FC.getLinkPt(sideIndex));
  Geometry::Vec3D crossX,crossY,crossZ;
  FC.selectAltAxis(sideIndex,crossX,crossY,crossZ);
  ModelSupport::buildPlane(SMap,surfID,DPoint,crossX);
  
  middleHR=HeadRule(SMap.realSurf(surfID));
  return;
}

  
int
InnerZone::createOuterVoidUnit(Simulation& System,
			       MonteCarlo::Object* masterCell,
			       HeadRule& FDivider,
			       const attachSystem::FixedComp& FC,
			       const long int sideIndex)
  /*!
    Construct outer void object main pipe
    \param System :: Simulation
    \param masterCell :: full master cell
    \param FDivider :: Front divider
    \param FC :: FixedComp
    \param sideIndex :: link point
    \return cell nubmer
  */
{
  ELog::RegMethod RegA("InnerZone","createOuterVoidUnit");

    // construct an cell based on previous cell:
  std::string Out;

  if (!FDivider.hasRule())
    FDivider=frontHR;

  const HeadRule& backDivider=
    (sideIndex) ? FC.getFullRule(-sideIndex) :
    backHR;
  
  Out=surroundHR.display()+
    FDivider.display()+backDivider.display();
  
  CellPtr->makeCell("OuterVoid",System,cellIndex++,0,0.0,Out);
  FDivider=backDivider;
  FDivider.makeComplement();

  // make the master cell valid:
  refrontMasterCell(masterCell,FDivider);
  return cellIndex-1;
}

int
InnerZone::createOuterVoidUnit(Simulation& System,
			       MonteCarlo::Object* masterCell,
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
  return createOuterVoidUnit(System,masterCell,frontDivider,FC,sideIndex);
}



  
void
InnerZone::refrontMasterCell(MonteCarlo::Object* MCell,
			     const HeadRule& FHR) const
  /*!
    This horrific function to re-build MCell so that it is correct
    as createOuterVoid consumes the front of the master cell
    \param MCell :: master cell object
    \param FC :: FixedComp
    \param sideIndex :: side index for back of FC object
  */
{
  ELog::RegMethod RegA("InnerZone","refrontMasterCell");

  std::string Out;  

  Out=surroundHR.display() + backHR.display()+ FHR.display();
  MCell->procString(Out);
  return;
}

 
MonteCarlo::Object*
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
  CellPtr->makeCell("MasterVoid",System,cellIndex++,0,0.0,Out);
  MonteCarlo::Object* OPtr= System.findQhull(cellIndex-1);

  return OPtr;
}




MonteCarlo::Object*
InnerZone::constructMasterCell(Simulation& System,
			       const attachSystem::ContainedComp& CC)
 /*!
    Construct outer void object main pipe
    \param System :: Simulation
    \param CC :: Contained Comp
    \return cell object
  */
{
  ELog::RegMethod RegA("balderFrontEnd","constructMasterCell(CC)");
 
  MonteCarlo::Object* ORef=constructMasterCell(System);
  CC.insertExternalObject(System,*ORef);
  return ORef;
}


}  // NAMESPACE attachSystem
