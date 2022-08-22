/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   bibBuild/makeBib.cxx
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
#include <memory>
#include <boost/format.hpp>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "inputParam.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "HeadRule.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "ExternalCut.h"
#include "World.h"
#include "AttachSupport.h"

#include "RotorWheel.h"
#include "ProtonPipe.h"
#include "BilReflector.h"
#include "WaterMod.h"
#include "ColdH2Mod.h"
#include "FilterBox.h"
#include "GuideBox.h"
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
  OR.addObject(BeFilterForward);
  OR.addObject(BeFilterBackward);
  OR.addObject(CWall);
}

makeBib::makeBib(const makeBib& A) : 
  Rotor(A.Rotor),ProtonObj(A.ProtonObj),RefObj(A.RefObj),
  WatMod(A.WatMod),ColdMod(A.ColdMod),BeFilterForward(A.BeFilterForward),
  BeFilterBackward(A.BeFilterBackward),ColdMod2(A.ColdMod2),
  GuideArray(A.GuideArray),ShieldArray(A.ShieldArray),
  CWall(A.CWall)
  /*!
    Copy constructor
    \param A :: makeBib to copy
  */
{}

makeBib&
makeBib::operator=(const makeBib& A)
  /*!
    Assignment operator
    \param A :: makeBib to copy
    \return *this
  */
{
  if (this!=&A)
    {
      Rotor=A.Rotor;
      ProtonObj=A.ProtonObj;
      RefObj=A.RefObj;
      WatMod=A.WatMod;
      ColdMod=A.ColdMod;
      BeFilterForward=A.BeFilterForward;
      BeFilterBackward=A.BeFilterBackward;
      ColdMod2=A.ColdMod2;
      GuideArray=A.GuideArray;
      ShieldArray=A.ShieldArray;
      CWall=A.CWall;
    }
  return *this;
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
    \param System :: Simulation system
    \param voidCell :: Void cell number
   */

{
  ELog::RegMethod RegA("makeBib","buildGuideArray");
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  // First two are on Cold side:
  for(size_t i=0;i<2;i++)
    {
      std::shared_ptr<GuideBox> GA
	(new GuideBox("Guide"+std::to_string(i+1)));
      GA->addInsertCell(voidCell);
      GA->setCutSurf("Limit",BeFilterForward->getLinkString(2));      
      GA->createAll(System,*ColdMod,2);
      OR.addObject(GA);
      
      attachSystem::addToInsertForced(System,*RefObj,*GA);
      attachSystem::addToInsertSurfCtrl(System,*CWall,*GA);
      GuideArray.push_back(GA);
    }
  attachSystem::addToInsertSurfCtrl(System,*GuideArray[0],*GuideArray[1]);
  
  // COLD2 guides
  for(size_t i=0;i<3;i++)
    {
      std::shared_ptr<GuideBox> GA
	(new GuideBox("Guide"+std::to_string(i+3)));
      GA->addInsertCell(voidCell);
      GA->setCutSurf("Limit",BeFilterBackward->getLinkString(2));      
      GA->createAll(System,*ColdMod2,2);
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
    Construct the guide array system (shielding)
    \param System :: Simulation syste
   */
{
  ELog::RegMethod RegA("makeBib","buildShieldArray");
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  for(size_t i=0;i<5;i++)
    {
      std::shared_ptr<GuideShield> GS
	(new GuideShield("GShield",i+1));
      GS->addInsertCell(CWall->getInnerCell());
      GS->setCutSurf("Front",RefObj->getFullRule(3));
      GS->setCutSurf("Back",CWall->getFullRule(1));
      GS->setCutSurf("Inner",GuideArray[i]->getExclude());
      GS->createAll(System,*GuideArray[i],0);
      OR.addObject(GS);      
      ShieldArray.push_back(GS);
    }
  
  return;
}

void 
makeBib::build(Simulation& System,
	       const mainSystem::inputParam&)
  /*!
    Carry out the full build
    \param System :: Simulation system
    \param IParam :: Input parameters
   */
{
  // For output stream
  ELog::RegMethod RControl("makeBib","build");

  int voidCell(74123);  // This number gets updated for things like a 
                        // void vessel

  Rotor->createAll(System,World::masterOrigin(),0);

  RefObj->createAll(System,*Rotor,9);
  attachSystem::addToInsertSurfCtrl(System,*RefObj,
				  Rotor->getCC("Target"));
  attachSystem::addToInsertForced(System,*RefObj,
				 Rotor->getCC("Body"));
  CWall->addInsertCell(voidCell);
  CWall->setCutSurf("Outer",RefObj->getFullRule(3));
  CWall->setCutSurf("Front",RefObj->getFullRule(1));
  CWall->setCutSurf("Back",RefObj->getFullRule(2));
  CWall->createAll(System,*Rotor,9);
  attachSystem::addToInsertSurfCtrl(System,*CWall,
				    Rotor->getCC("Body"));
  
  // first moderator
  ColdMod->setCutSurf("FrontFace",Rotor->getLinkString(2));
  ColdMod->createAll(System,*Rotor,13);

  attachSystem::addToInsertControl(System,*RefObj,*ColdMod);

  BeFilterForward->createAll(System,*ColdMod,2);
  attachSystem::addToInsertSurfCtrl(System,*RefObj,*BeFilterForward);


  ProtonObj->addInsertCell(voidCell);
  ProtonObj->createAll(System,*Rotor,14);
  attachSystem::addToInsertForced(System,*RefObj,*ProtonObj);

  // Second moderator
  ColdMod2->setCutSurf("FrontFace",Rotor->getLinkString(12));
  ColdMod2->createAll(System,*Rotor,13);
  attachSystem::addToInsertControl(System,*RefObj,*ColdMod2);
  attachSystem::addToInsertSurfCtrl(System,*ColdMod2,Rotor->getCC("Target"));
  attachSystem::addToInsertForced(System,*ProtonObj,*ColdMod2);
  attachSystem::addToInsertForced(System,*CWall,*ProtonObj);

  BeFilterBackward->createAll(System,*ColdMod2,2);
  attachSystem::addToInsertSurfCtrl(System,*RefObj,*BeFilterBackward);

  buildGuideArray(System,voidCell);
  buildShieldArray(System);

  return;
}


}   // NAMESPACE bibSystem

 
