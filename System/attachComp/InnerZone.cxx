 /********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   attachComp/InnerZone.cxx
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
  CellPtr(dynamic_cast<CellMap*>(&FRef)),
  voidMat(0),masterCell(0)
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
  backHR(A.backHR),middleHR(A.middleHR),frontDivider(A.frontDivider),
  voidMat(A.voidMat)
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
      voidMat=A.voidMat;
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
InnerZone::triVoidUnit(Simulation& System,
		       MonteCarlo::Object* masterCell,
		       const HeadRule& CutA,
		       const HeadRule& CutB)
  /*!
    Tri construct for the main void.
    Note that CutA and CutB define the region to be cut.
    The regions created are pre-mid-post
    \param System :: Simulation
    \param masterCell :: full master cell
    \param FDivider :: Front divider
    \param CutA :: Cut surface A
    \param CutB :: Cut surface B
    \return cell nubmer for last cell unit
  */
{
  ELog::RegMethod RegA("InnerZone","tirVoidUnit(HR,HR)");

  return triVoidUnit(System,masterCell,frontDivider,CutA,CutB);
}

int
InnerZone::triVoidUnit(Simulation& System,
		       MonteCarlo::Object* masterCell,
		       HeadRule& FDivider,
		       const HeadRule& CutA,
		       const HeadRule& CutB)
  /*!
    Tri-cutter for the main void.
    Note that CutA and CutB define the region to be cut.
    \param System :: Simulation
    \param masterCell :: full master cell
    \param FDivider :: Front divider
    \param CutA :: Cut surface A
    \param CutB :: Cut surface B
    \return cell number of inner zone
  */
{
  ELog::RegMethod RegA("InnerZone","triVoidUnit(FDivider,HR,HR)");

    // construct an cell based on previous cell:
  std::string Out;

  if (!FDivider.hasRule())
    FDivider=frontHR;
  
  Out=surroundHR.display()+
    FDivider.display()+CutA.complement().display();
  CellPtr->makeCell("OuterVoid",System,cellIndex++,voidMat,0.0,Out);

  Out=surroundHR.display()+CutA.display()+CutB.display();
  CellPtr->makeCell("OuterVoid",System,cellIndex++,voidMat,0.0,Out);

  FDivider=CutB;
  FDivider.makeComplement();

  // make the master cell valid:
  refrontMasterCell(masterCell,FDivider);
  return cellIndex-1;
}

int
InnerZone::cutVoidUnit(Simulation& System,
		       MonteCarlo::Object* masterCell,
		       HeadRule& FDivider,
		       const HeadRule& CutA,
		       const HeadRule& CutB)
  /*!
    Cutter for the main void.
    Note that CutA and CutB define the region to be cut.
    \param System :: Simulation
    \param masterCell :: full master cell
    \param FDivider :: Front divider
    \param CutA :: Cut surface A
    \param CutB :: Cut surface B
    \return cell nubmer
  */
{
  ELog::RegMethod RegA("InnerZone","cutVoidUnit(FDivider,HR,HR)");

    // construct an cell based on previous cell:
  std::string Out;

  if (!FDivider.hasRule())
    FDivider=frontHR;
  
  Out=surroundHR.display()+
    FDivider.display()+CutA.complement().display();
  CellPtr->makeCell("OuterVoid",System,cellIndex++,voidMat,0.0,Out);
  FDivider=CutB;
  FDivider.makeComplement();

  // make the master cell valid:
  refrontMasterCell(masterCell,FDivider);

  ELog::EM<<"Out["<<FCName<<"]["<<cellIndex<<"] == "<<*masterCell<<ELog::endDiag;
    
  return cellIndex-1;
}

int
InnerZone::cutVoidUnit(Simulation& System,
		       MonteCarlo::Object* masterCell,
		       const HeadRule& CutA,
		       const HeadRule& CutB)
  /*!
    Cutter for the main void.
    Note that CutA and CutB define the region to be cut.
    \param System :: Simulation
    \param masterCell :: full master cell
    \param FDivider :: Front divider
    \param CutA :: Cut surface A
    \param CutB :: Cut surface B
    \return cell nubmer for last cell unit
  */
{
  ELog::RegMethod RegA("InnerZone","cutVoidUnit(HR,HR)");

  return cutVoidUnit(System,masterCell,frontDivider,CutA,CutB);
}

int
InnerZone::singleVoidUnit(Simulation& System,
			  MonteCarlo::Object* masterCell,
			  const HeadRule& CutA)
  /*!
    Cutter for the main void (end unit)
    \param System :: Simulation
    \param masterCell :: full master cell
    \param CutA :: Cut surface A
    \return cell nubmer
  */
{
  ELog::RegMethod RegA("InnerZone","singleVoidUnit(HR)");
  return singleVoidUnit(System,masterCell,frontDivider,CutA);
}

int
InnerZone::singleVoidUnit(Simulation& System,
		       MonteCarlo::Object* masterCell,
		       HeadRule& FDivider,
		       const HeadRule& CutA)
  /*!
    Cutter for the main void.
    Note that CutA and CutB define the region to be cut.
    \param System :: Simulation
    \param masterCell :: full master cell
    \param FDivider :: Front divider
    \param CutA :: Cut surface A
    \return cell nubmer
  */
{
  ELog::RegMethod RegA("InnerZone","singleVoidUnit(FDivider,HR)");

    // construct an cell based on previous cell:
  std::string Out;

  if (!FDivider.hasRule())
    FDivider=frontHR;
  
  Out=surroundHR.display()+
    FDivider.display()+CutA.complement().display();
  CellPtr->makeCell("OuterVoid",System,cellIndex++,voidMat,0.0,Out);
  FDivider=CutA;

  // make the master cell valid:
  refrontMasterCell(masterCell,FDivider);
  return cellIndex-1;
}
  
int
InnerZone::createOuterVoidUnit(Simulation& System,
			       MonteCarlo::Object* masterCell,
			       HeadRule& FDivider,
			       const HeadRule& BDivider)
  /*!
    Construct outer void object main pipe
    \param System :: Simulation
    \param masterCell :: full master cell
    \param FDivider :: Front divider
    \param BDivider :: Back divider
    \return cell nubmer
  */
{
  ELog::RegMethod RegA("InnerZone","createOuterVoidUnit(HR,HR)");

    // construct an cell based on previous cell:
  std::string Out;

  if (!FDivider.hasRule())
    FDivider=frontHR;
  
  Out=surroundHR.display()+
    FDivider.display()+BDivider.display();

  CellPtr->makeCell("OuterVoid",System,cellIndex++,voidMat,0.0,Out);
  FDivider=BDivider;
  FDivider.makeComplement();

  // make the master cell valid:
  refrontMasterCell(masterCell,FDivider);
  return cellIndex-1;
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
  ELog::RegMethod RegA("InnerZone","createOuterVoidUnit(HR)");

  const HeadRule& backDivider=
    (sideIndex) ? FC.getFullRule(-sideIndex) :
    backHR;
    
  return createOuterVoidUnit(System,masterCell,FDivider,backDivider);
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

int
InnerZone::createNamedOuterVoidUnit(Simulation& System,
				    const std::string& extraName,
				    MonteCarlo::Object* masterCell,
				    HeadRule& FDivider,
				    const attachSystem::FixedComp& FC,
				    const long int sideIndex)
  /*!
    Construct outer void object main pipe
    \param System :: Simulation
    \param extraName :: Name for outer void [in-addition to OuterVoid]
    \param masterCell :: full master cell
    \param FDivider :: Front divider
    \param FC :: FixedComp
    \param sideIndex :: link point
    \return cell nubmer
  */
{
  ELog::RegMethod RegA("InnerZone","createNamedOuterVoidUnit(HR)");
  const int cellN=
    createOuterVoidUnit(System,masterCell,FDivider,FC,sideIndex);
  CellPtr->addCell(extraName,cellN);
  return cellN;
}

int
InnerZone::createNamedOuterVoidUnit(Simulation& System,
				    const std::string& extraName,
				    MonteCarlo::Object* masterCell,
				    const attachSystem::FixedComp& FC,
				    const long int sideIndex)
  /*!
    Construct outer void object main pipe
    \param System :: Simulation
    \param extraName :: Name for outer void [in-addition to OuterVoid]
    \param masterCell :: full master cell
    \param FC :: FixedComp
    \param sideIndex :: link point
    \return cell nubmer
  */
{
  ELog::RegMethod RegA("InnerZone","createNamedOuterVoidUnit");
  const int cellN=
    createOuterVoidUnit(System,masterCell,frontDivider,FC,sideIndex);
  CellPtr->addCell(extraName,cellN);
  return cellN;
}
  
int
InnerZone::createNamedOuterVoidUnit(Simulation& System,
				    const std::string& extraName,
				    MonteCarlo::Object* masterCell,
				    HeadRule& FDivider,
				    const HeadRule& BDivider)
  /*!
    Construct outer void object main pipe
    \param System :: Simulation
    \param extraName :: Name for outer void [in-addition to OuterVoid]
    \param masterCell :: full master cell
    \param fRule :: Front rule
    \param bRule :: Back rule
    \return cell nubmer
  */
{
  ELog::RegMethod RegA("InnerZone","createNamedOuterVoidUnit(HR,HR)");
  const int cellN=
    createOuterVoidUnit(System,masterCell,FDivider,BDivider);
  CellPtr->addCell(extraName,cellN);
  return cellN;
}
  
void
InnerZone::refrontMasterCell(MonteCarlo::Object* MCell,
			     const HeadRule& FHR) const
  /*!
    This horrific function to re-build MCell so that it is correct
    as createOuterVoid consumes the front of the master cell
    \param MCell :: master cell object
    \param FHR :: Curs surface head rule 
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
    Construct outer void object 
    \param System :: Simulation
    \return cell object
  */
{
  ELog::RegMethod RegA("InnerZone","constructMasterCell");

  std::string Out;

  Out+=surroundHR.display() + backHR.display()+ frontHR.display();
  
  CellPtr->makeCell("MasterVoid",System,cellIndex++,voidMat,0.0,Out);
  masterCell= System.findObject(cellIndex-1);

  return masterCell;
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
  ELog::RegMethod RegA("InnerZone","constructMasterCell(CC)");
 
  masterCell=constructMasterCell(System);
  CC.insertExternalObject(System,*masterCell);

  return masterCell;
}


}  // NAMESPACE attachSystem
