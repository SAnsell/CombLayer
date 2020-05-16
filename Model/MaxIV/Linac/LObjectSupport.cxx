/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File: Linac/LObjectSupport.cxx
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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
#include <iterator>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "inputParam.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Rules.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "FixedGroup.h"
#include "FixedOffsetGroup.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "InnerZone.h"
#include "AttachSupport.h"
#include "generateSurf.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generalConstruct.h"

#include "VacuumPipe.h"
#include "CorrectorMag.h"
#include "FlatPipe.h"
#include "LQuad.h"
#include "LSexupole.h"
#include "DipoleDIBMag.h"

namespace tdcSystem
{

int
correctorMagnetPair(Simulation& System,
		    attachSystem::InnerZone& buildZone,
		    const std::shared_ptr<attachSystem::FixedComp>& pipe,
		    const std::shared_ptr<tdcSystem::CorrectorMag>& CMA,
		    const std::shared_ptr<tdcSystem::CorrectorMag>& CMB)
  /*!
    Given a pipe build the two correct magnets round it and then 
    correctly do the InnerZone splitting.
    \param System :: Simulation to use
    \param buildZone :: Inner zoner to be split - 
            currently at start of pipe
    \param CMA :: First Corrector Magnet 
    \param CMB :: Second Corrector Magnet 
    \return outerCell number
    \todo see about generalization when needed.
  */
{
  ELog::RegMethod RegA("LObjectSupport[F]","correctorMagnetPair");

  attachSystem::ContainedComp* CPtr=
    dynamic_cast<attachSystem::ContainedComp*>(pipe.get());
  if (!CPtr)
    throw ColErr::DynamicConv("FixedComp","ContainedComp","pipe");

  MonteCarlo::Object* masterCell=buildZone.getMaster();
		      
  CMA->setCutSurf("Inner",*pipe,"outerPipe");
  CMA->createAll(System,*pipe,"#front");

  CMB->setCutSurf("Inner",*pipe,"outerPipe");
  CMB->createAll(System,*pipe,"#front");

  int outerCell=buildZone.createOuterVoidUnit(System,masterCell,*CMA,-1);
  CPtr->insertInCell(System,outerCell);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*CMA,2);
  CMA->insertInCell(System,outerCell);

  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*CMB,-1);
  CPtr->insertInCell(System,outerCell);
  
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*CMB,2);
  CMB->insertInCell(System,outerCell);
  

  return outerCell;
}

template<typename MagTYPE>
int
pipeMagUnit(Simulation& System,
	    attachSystem::InnerZone& buildZone,
	    const std::shared_ptr<attachSystem::FixedComp>& pipe,
	    const std::string& linkName,
	    const std::string& outerName,
	    const std::shared_ptr<MagTYPE>& magUnit)
  /*!
    Given a pipe build magnet unit round it and then 
    correctly do the InnerZone splitting.
    \param System :: Simulation to use
    \param buildZone :: Inner zoner to be split - 
            currently at start of pipe
    \param pipe :: Inner object
    \param linkName :: Construction link point
    \param surfName :: Link point in pipe for outer surf 
        [typically : outerPipe]
    \param magUnit :: Magnetic unit
    \return outerCell number
  */
{
  ELog::RegMethod RegA("LObjectSupport[F]","pipeMagUnit");

  MonteCarlo::Object* masterCell=buildZone.getMaster();

  magUnit->setCutSurf("Inner",*pipe,outerName);
  magUnit->createAll(System,*pipe,linkName);
  attachSystem::ContainedComp* CPtr=
    dynamic_cast<attachSystem::ContainedComp*>(pipe.get());
  if (!CPtr)
    throw ColErr::DynamicConv("FixedComp","Contained(Comp/Group)","pipe");

  int outerCell=buildZone.createOuterVoidUnit
    (System,masterCell,*magUnit,-1);
  CPtr->insertInCell(System,outerCell);

  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*magUnit,2);
  magUnit->insertInCell(System,outerCell);

  return outerCell;
}

template<typename MagTYPE>
int
pipeMagGroup(Simulation& System,
	     attachSystem::InnerZone& buildZone,
	     const std::shared_ptr<attachSystem::FixedComp>& pipe,
	     const std::set<std::string>& containerSet,
	     const std::string& linkName,
	     const std::string& outerName,
	     const std::shared_ptr<MagTYPE>& magUnit)
  /*!
    Given a pipe build magnet unit round it and then 
    correctly do the InnerZone splitting.
    \param System :: Simulation to use
    \param buildZone :: Inner zoner to be split - 
            currently at start of pipe
    \param pipe :: Inner object [containedGroup]
    \param containerSet :: Insert set for containerGroup
    \param linkName :: Construction link point
    \param surfName :: Link point in pipe for outer surf 
        [typically : outerPipe]
    \param magUnit :: Magnetic unit
    \return outerCell number
  */
{
  ELog::RegMethod RegA("LObjectSupport[F]","pipeMagGroup");

  MonteCarlo::Object* masterCell=buildZone.getMaster();

  magUnit->setCutSurf("Inner",*pipe,outerName);
  magUnit->createAll(System,*pipe,linkName);
  attachSystem::ContainedGroup* CPtr=
    dynamic_cast<attachSystem::ContainedGroup*>(pipe.get());
  if (!CPtr)
    throw ColErr::DynamicConv("FixedComp","Contained(Comp/Group)","pipe");

  int outerCell=buildZone.createOuterVoidUnit
    (System,masterCell,*magUnit,-1);
  for(const std::string& containerName: containerSet) 
    CPtr->insertInCell(containerName,System,outerCell);
  
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*magUnit,2);
  magUnit->insertInCell(System,outerCell);

  return outerCell;
}

int
pipeTerminate(Simulation& System,
	      attachSystem::InnerZone& buildZone,
	      const std::shared_ptr<attachSystem::FixedComp>& pipe)
  /*!
    Teminate a pipe in the inde of a Inner zone
    \param System :: Simulation to use
    \param buildZone :: Inner zoner to be split - 
            currently at start of pipe
    \return outerCell number
  */
{
  ELog::RegMethod RegA("LObjectSupport[F]","pipeTerminate");

  MonteCarlo::Object* masterCell=buildZone.getMaster();

  const int outerCell=
    buildZone.createOuterVoidUnit(System,masterCell,*pipe,2);
  attachSystem::ContainedComp* CPtr=
    dynamic_cast<attachSystem::ContainedComp*>(pipe.get());
  if (CPtr)
    CPtr->insertInCell(System,outerCell);
  else
    {
      attachSystem::ContainedGroup* CGPtr=
	dynamic_cast<attachSystem::ContainedGroup*>(pipe.get());
      if (!CGPtr)
	throw ColErr::DynamicConv("FixedComp","Contained(Comp/Group)","pipe");
      
      CGPtr->insertAllInCell(System,outerCell);
    }

  return outerCell;
}

int
pipeTerminateGroup(Simulation& System,
		   attachSystem::InnerZone& buildZone,
		   const std::shared_ptr<attachSystem::FixedComp>& pipe,
		   const std::set<std::string>& containerSet)
  /*!
    Teminate a pipe in the inde of a Inner zone
    \param System :: Simulation to use
    \param buildZone :: Inner zoner to be split - 
            currently at start of pipe
    \return outerCell number
  */
{
  ELog::RegMethod RegA("LObjectSupport[F]","pipeTerminate");

  MonteCarlo::Object* masterCell=buildZone.getMaster();

  const int outerCell=
    buildZone.createOuterVoidUnit(System,masterCell,*pipe,2);
  attachSystem::ContainedGroup* CGPtr=
    dynamic_cast<attachSystem::ContainedGroup*>(pipe.get());
  if (!CGPtr)
    throw ColErr::DynamicConv("FixedComp","ContainedGroup","pipe");

  for(const std::string& containerName : containerSet)
    CGPtr->insertInCell(containerName,System,outerCell);

  return outerCell;
}

///\cond TEMPLATE


template 
int pipeMagUnit(Simulation&,
		attachSystem::InnerZone&,
		const std::shared_ptr<attachSystem::FixedComp>&,
		const std::string&,
		const std::string&,
		const std::shared_ptr<tdcSystem::LQuad>&);

template 
int pipeMagUnit(Simulation&,
		attachSystem::InnerZone&,
		const std::shared_ptr<attachSystem::FixedComp>&,
		const std::string&,
		const std::string&,
		const std::shared_ptr<tdcSystem::LSexupole>&);

template 
int pipeMagUnit(Simulation&,
		attachSystem::InnerZone&,
		const std::shared_ptr<attachSystem::FixedComp>&,
		const std::string&,
		const std::string&,
		const std::shared_ptr<tdcSystem::DipoleDIBMag>&);
template 
int pipeMagGroup(Simulation&,
		 attachSystem::InnerZone&,
		 const std::shared_ptr<attachSystem::FixedComp>&,
		 const std::set<std::string>&,
		 const std::string&,
		 const std::string&,
		 const std::shared_ptr<tdcSystem::LQuad>&);

template 
int pipeMagGroup(Simulation&,
		 attachSystem::InnerZone&,
		 const std::shared_ptr<attachSystem::FixedComp>&,
		 const std::set<std::string>&,
		 const std::string&,
		 const std::string&,
		 const std::shared_ptr<tdcSystem::DipoleDIBMag>&);



///\endcond TEMPLATE

}   // NAMESPACE tdcSystem

