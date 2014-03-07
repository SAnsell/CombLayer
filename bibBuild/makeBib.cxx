/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   bibBuild/makeBib.cxx
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#include <utility>
#include <cmath>
#include <complex>
#include <list>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <iterator>
#include <boost/array.hpp>
#include <boost/format.hpp>
#include <boost/shared_ptr.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "inputParam.h"
#include "Triple.h"
#include "NRange.h"
#include "NList.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Rules.h"
#include "HeadRule.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "Object.h"
#include "Qhull.h"
#include "KGroup.h"
#include "Source.h"
#include "KCode.h"
#include "insertInfo.h"
#include "insertBaseInfo.h"
#include "InsertComp.h"
#include "ModeCard.h"
#include "PhysImp.h"
#include "PhysCard.h"
#include "LSwitchCard.h"
#include "PhysicsCards.h"
#include "Simulation.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "LayerComp.h"
#include "World.h"
#include "FlightLine.h"
#include "AttachSupport.h"

#include "RotorWheel.h"
#include "ProtonPipe.h"
#include "BilReflector.h"
#include "WaterMod.h"
#include "ColdH2Mod.h"
#include "FilterBox.h"
#include "GuideBox.h"
#include "NiGuide.h"
#include "ConcreteWall.h"
#include "GuideShield.h"

#include "makeBib.h"


namespace bibSystem
{

makeBib::makeBib() :
  Rotor(new RotorWheel("RotorWheel")),
  ProtonObj(new ProtonPipe("ProtonObj")),
  RefObj(new BilReflector("Refl")),
  WatMod(new WaterMod("WatMod")),
  ColdMod(new ColdH2Mod("ColdMod")),

  BeFilterForward(new FilterBox("BeFilterForward")),
  BeFilterBackward(new FilterBox("BeFilterBackward")),

  ColdMod2(new ColdH2Mod("ColdMod2")),
  FBox(new FilterBox("FBox")),
  CWall(new ConcreteWall("ConcWall"))
 /*!
    Constructor
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(Rotor);
  OR.addObject(ProtonObj);
  OR.addObject(RefObj);
  OR.addObject(WatMod);
  OR.addObject(ColdMod);
  OR.addObject(ColdMod2);
  OR.addObject(FBox);
  OR.addObject(BeFilterForward);
  OR.addObject(BeFilterBackward);
  OR.addObject(CWall);
}


makeBib::~makeBib()
  /*!
    Destructor
  */
{}

void
makeBib::buildGuideArray(Simulation& System,const int voidCell)
  /*!
    Construct the guide array system
    \param SimPtr :: Simulation syste
    \param voidCell :: Void cell number
   */

{
  ELog::RegMethod RegA("makeBib","buildGuideArray");
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  // First two are on Cold side:
  for(size_t i=0;i<2;i++)
    {
      boost::shared_ptr<GuideBox> GA
	(new GuideBox(StrFunc::makeString("Guide",i+1)));
      GA->addInsertCell(voidCell);
      GA->createAll(System,*ColdMod,1,*BeFilterForward,1);      
      OR.addObject(GA);
      
      attachSystem::addToInsertForced(System,*RefObj,*GA);
      attachSystem::addToInsertSurfCtrl(System,*CWall,*GA);
      GuideArray.push_back(GA);
    }
  attachSystem::addToInsertSurfCtrl(System,*GuideArray[0],*GuideArray[1]);
  
  // COLD2 guides
  for(size_t i=0;i<3;i++)
    {
      boost::shared_ptr<GuideBox> GA
	(new GuideBox(StrFunc::makeString("Guide",i+3)));
      GA->addInsertCell(voidCell);
      GA->createAll(System,*ColdMod2,1,*BeFilterBackward,1);      
      OR.addObject(GA);
      
      attachSystem::addToInsertForced(System,*RefObj,*GA);
      attachSystem::addToInsertSurfCtrl(System,*CWall,*GA);
      GuideArray.push_back(GA);
    }
  attachSystem::addToInsertSurfCtrl(System,*GuideArray[2],*GuideArray[3]);
  attachSystem::addToInsertSurfCtrl(System,*GuideArray[3],*GuideArray[4]);
  attachSystem::addToInsertSurfCtrl(System,*GuideArray[2],*GuideArray[4]);
  return;
}

void
makeBib::buildShieldArray(Simulation& System)
  /*!
    Construct the guide array system
    \param SimPtr :: Simulation syste
   */
{
  ELog::RegMethod RegA("makeBib","buildShieldArray");
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  for(size_t i=0;i<5;i++)
    {
      boost::shared_ptr<GuideShield> GS
	(new GuideShield("GShield",i+1));
      GS->addInsertCell(CWall->getInnerCell());
      GS->createAll(System,*GuideArray[i],*RefObj,2,*CWall,0);      
      OR.addObject(GS);      
      ShieldArray.push_back(GS);
    }
  
  return;
}

void 
makeBib::build(Simulation* SimPtr,
	       const mainSystem::inputParam& IParam)
  /*!
    Carry out the full build
    \param SimPtr :: Simulation system
    \param IParam :: Input parameters
   */
{
  // For output stream
  ELog::RegMethod RControl("makeBib","build");

  int voidCell(74123);  // This number gets updated for things like a 
                        // void vessel

  Rotor->createAll(*SimPtr,World::masterOrigin());

  RefObj->createAll(*SimPtr,*Rotor,8);
  attachSystem::addToInsertSurfCtrl(*SimPtr,*RefObj,
				  Rotor->getKey("Target"));
  attachSystem::addToInsertForced(*SimPtr,*RefObj,
				 Rotor->getKey("Body"));
  CWall->addInsertCell(voidCell);
  CWall->createAll(*SimPtr,*Rotor,8,*RefObj,2);
  attachSystem::addToInsertSurfCtrl(*SimPtr,*CWall,
				    Rotor->getKey("Body"));
  
  // first moderator
  ColdMod->createAll(*SimPtr,*Rotor,12,1);

  attachSystem::addToInsertControl(*SimPtr,*RefObj,*ColdMod);

  BeFilterForward->createAll(*SimPtr,*ColdMod,1);
  attachSystem::addToInsertSurfCtrl(*SimPtr,*RefObj,*BeFilterForward);


  ProtonObj->addInsertCell(voidCell);
  ProtonObj->createAll(*SimPtr,*Rotor,13);
  attachSystem::addToInsertForced(*SimPtr,*RefObj,*ProtonObj);

  // Second moderator
  ColdMod2->createAll(*SimPtr,*Rotor,12,11);
  attachSystem::addToInsertControl(*SimPtr,*RefObj,*ColdMod2);
  attachSystem::addToInsertSurfCtrl(*SimPtr,*ColdMod2,Rotor->getKey("Target"));
  attachSystem::addToInsertForced(*SimPtr,*ProtonObj,*ColdMod2);
  attachSystem::addToInsertForced(*SimPtr,*CWall,*ProtonObj);

  BeFilterBackward->createAll(*SimPtr,*ColdMod2,1);
  attachSystem::addToInsertSurfCtrl(*SimPtr,*RefObj,*BeFilterBackward);

  buildGuideArray(*SimPtr,voidCell);
  buildShieldArray(*SimPtr);

  return;
}


}   // NAMESPACE bibSystem

 
