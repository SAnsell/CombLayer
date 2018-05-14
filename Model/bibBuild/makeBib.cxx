/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   bibBuild/makeBib.cxx
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#include "Simulation.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
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
	(new GuideBox(StrFunc::makeString("Guide",i+1)));
      GA->addInsertCell(voidCell);
      GA->createAll(System,*ColdMod,2,*BeFilterForward,2);      
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
	(new GuideBox(StrFunc::makeString("Guide",i+3)));
      GA->addInsertCell(voidCell);
      GA->createAll(System,*ColdMod2,2,*BeFilterBackward,2);      
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
      GS->createAll(System,*GuideArray[i],*RefObj,3,*CWall,1);      
      OR.addObject(GS);      
      ShieldArray.push_back(GS);
    }
  
  return;
}

void 
makeBib::build(Simulation& System,
	       const mainSystem::inputParam& IParam)
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

  Rotor->createAll(System,World::masterOrigin());

  RefObj->createAll(System,*Rotor,9);
  attachSystem::addToInsertSurfCtrl(System,*RefObj,
				  Rotor->getCC("Target"));
  attachSystem::addToInsertForced(System,*RefObj,
				 Rotor->getCC("Body"));
  CWall->addInsertCell(voidCell);
  CWall->createAll(System,*Rotor,8,*RefObj,2);
  attachSystem::addToInsertSurfCtrl(System,*CWall,
				    Rotor->getCC("Body"));
  
  // first moderator
  ColdMod->createAll(System,*Rotor,12,1);

  attachSystem::addToInsertControl(System,*RefObj,*ColdMod);

  BeFilterForward->createAll(System,*ColdMod,2);
  attachSystem::addToInsertSurfCtrl(System,*RefObj,*BeFilterForward);


  ProtonObj->addInsertCell(voidCell);
  ProtonObj->createAll(System,*Rotor,13);
  attachSystem::addToInsertForced(System,*RefObj,*ProtonObj);

  // Second moderator
  ColdMod2->createAll(System,*Rotor,12,11);
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

 
