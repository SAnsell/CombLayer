/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   chip/makeChipIR.cxx
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
#include <boost/format.hpp>

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
#include "insertInfo.h"
#include "insertBaseInfo.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "FixedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "World.h"
#include "ChipIRFilter.h"
#include "ChipIRGuide.h"
#include "HoleUnit.h"
#include "PreCollimator.h"
#include "Jaws.h"
#include "ColBox.h"
#include "beamBlock.h"
#include "BeamStop.h"
#include "Table.h"
#include "BulkShield.h"
#include "boxValues.h"
#include "boxUnit.h"
#include "BoxLine.h"
#include "FeedThrough.h"
#include "FBBlock.h"
#include "chipIRHutch.h"

#include "makeChipIR.h"

#include "shutterBlock.h"
#include "GeneralShutter.h"
#include "debugMethod.h"

namespace hutchSystem
{

makeChipIR::makeChipIR() :
  GObj(new ChipIRGuide("chipGuide")),
  hutchObj(new chipIRHutch("chipHut")),
  FB("chipHutFB")
  /*!
    Constructor
  */
{
  ELog::RegMethod RegA("makeChipIR","makeChipIR");

  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(GObj);
  OR.addObject(hutchObj);
}

makeChipIR::makeChipIR(const makeChipIR& A) : 
  GObj(new ChipIRGuide(*A.GObj)),
  hutchObj(new chipIRHutch(*A.hutchObj)),
  FeedVec(A.FeedVec),FB(A.FB)
  /*!
    Copy constructor
    \param A :: makeChipIR to copy
  */
{}

makeChipIR&
makeChipIR::operator=(const makeChipIR& A)
  /*!
    Assignment operator
    \param A :: makeChipIR to copy
    \return *this
  */
{
  if (this!=&A)
    {
      *GObj = *A.GObj;
      *hutchObj = *A.hutchObj;
      FeedVec=A.FeedVec;
      FB=A.FB;
    }
  return *this;
}

makeChipIR::~makeChipIR()
  /*!
    Destructor
   */
{}


void
makeChipIR::buildIsolated(Simulation& System,
			  const mainSystem::inputParam& IParam)
  /*!
    Carry out the build with no linkage
    \param SimPtr :: Simulation system
    \param IParam :: Input parameters
   */
{
  ELog::RegMethod RegA("makeChipIR","buildIsolated");

  const int voidCell(74123);

  hutchObj->setCollFlag(IParam.getValue<int>("collFlag"));
  // chipguide/chiphutch
  GObj->addInsertCell("outer",voidCell);
  GObj->addInsertCell("rightwall",voidCell);
  GObj->addInsertCell("leftwall",voidCell);
  GObj->createAll(System,World::masterOrigin());

  hutchObj->addInsertCell(voidCell);
  hutchObj->createAll(System,World::masterTS2Origin(),*GObj,
  		  GObj->getCC("inner"));
  //  FB.createAll(System,*hutchObj);

  FB.createAll(System,*hutchObj);
  const FuncDataBase& Control=System.getDataBase();  
  const size_t NFeed=Control.EvalVar<size_t>("chipNWires");
  for(size_t i=0;i<NFeed;i++)
    {
      FeedVec.push_back
      (std::make_shared<FeedThrough>("chipWiresColl",i+1));
      FeedVec.back()->createAll(System,*hutchObj);
    }  
  
  return;
}

void 
makeChipIR::build(Simulation* SimPtr,
		  const mainSystem::inputParam& IParam,
		  const shutterSystem::BulkShield& BulkObj)
  /*!
    Carry out the full build
    \param SimPtr :: Simulation system
    \param IParam :: Input parameters
    \param BulkObj :: BulkShield object
   */
{
  ELog::RegMethod RegA("makeChipIR","build");
  const int voidCell(74123);
  const FuncDataBase& Control=SimPtr->getDataBase();  

  // Exit if no work to do:
  if (IParam.flag("exclude") && IParam.compValue("E",std::string("chipIR")))
    return;
  
  hutchObj->setCollFlag(IParam.getValue<int>("collFlag"));
  // chipguide/chiphutch
  GObj->setMonoSurface(BulkObj.getMonoSurf());
  GObj->addInsertCell("outer",voidCell);
  GObj->addInsertCell("rightwall",voidCell);
  GObj->addInsertCell("leftwall",voidCell);
  
  GObj->createAll(*SimPtr,BulkObj,0);
    
  hutchObj->addInsertCell(74123);
  hutchObj->createAll(*SimPtr,*GObj,2);
  //  hutchObj->createAll(*SimPtr,*BulkObj.getShutter(0),*GObj,
  //		      GObj->getCC("inner"));
  
  FB.createAll(*SimPtr,*hutchObj);

  const size_t NFeed=Control.EvalVar<size_t>("chipNWires");
  for(size_t i=0;i<NFeed;i++)
    {
      FeedVec.push_back
	(std::make_shared<FeedThrough>("chipWiresColl",i+1));
      FeedVec.back()->createAll(*SimPtr,*hutchObj);
    }  


  return;
}

}   // NAMESPACE HutchSystem
