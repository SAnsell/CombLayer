/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   muon/makeMuon.cxx
 *
 * Copyright (c) 2004-2014 by Stuart Ansell/Goran Skoro
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
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
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
#include "LinearComp.h"
#include "World.h"
#include "targSimpleShield.h"
#include "targetVesselBox.h"
#include "muonCarbonTarget.h"
#include "muonTube.h"
#include "coneColl.h"
#include "cShieldLayer.h"
#include "profileMon.h"
#include "muonQ1.h"
#include "makeMuon.h"

namespace muSystem
{

makeMuon::makeMuon() :
  TargShieldObj(new muSystem::targSimpleShield("TargShield")),
  VessBoxObj(new muSystem::targetVesselBox("VesselBox")),
  TargetObj(new muSystem::muonCarbonTarget("CarbonTrgt")),  
// epb
  EpbInTubeObj(new muSystem::muonTube("EpbInTube")), 
  EpbOutTubeObj(new muSystem::muonTube("EpbOutTube")),
  FirstCollObj(new muSystem::coneColl("FirstColl")),            
  ThirdCollObj(new muSystem::coneColl("ThirdColl")),            
  FirstShieldObj(new muSystem::cShieldLayer("FirstShield")),       
  FirstTubeObj(new muSystem::muonTube("FirstTube")), 
  ThirdShieldObj(new muSystem::cShieldLayer("ThirdShield")),
  ThirdTubeObj(new muSystem::muonTube("ThirdTube")), 
  ProfMonObj(new muSystem::profileMon("ProfileMonitor")),  
  Q44Obj(new muSystem::muonQ1("Q44quad")),
  FinalTubeObj(new muSystem::muonTube("FinalTube")), 
// muon                   
  Q1Obj(new muSystem::muonQ1("Q1quad")),
  Q2Obj(new muSystem::muonQ1("Q2quad")) 
  
//  Muon & EPB beamline 
  /*!
    Constructor
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(TargShieldObj);
  OR.addObject(VessBoxObj);
  OR.addObject(TargetObj); 
  OR.addObject(EpbInTubeObj); 
  OR.addObject(EpbOutTubeObj);
  OR.addObject(FirstCollObj); 
  OR.addObject(ThirdCollObj);         
  OR.addObject(FirstShieldObj);        
  OR.addObject(FirstTubeObj);    
  OR.addObject(ThirdShieldObj);
  OR.addObject(ThirdTubeObj);     
  OR.addObject(ProfMonObj);        
  OR.addObject(Q44Obj);
  OR.addObject(FinalTubeObj);     
  OR.addObject(Q1Obj); 
  OR.addObject(Q2Obj);

}

makeMuon::makeMuon(const makeMuon& A) : 
  TargShieldObj(A.TargShieldObj),VessBoxObj(A.VessBoxObj),
  TargetObj(A.TargetObj),EpbInTubeObj(A.EpbInTubeObj),
  EpbOutTubeObj(A.EpbOutTubeObj),FirstCollObj(A.FirstCollObj),
  ThirdCollObj(A.ThirdCollObj),FirstShieldObj(A.FirstShieldObj),
  FirstTubeObj(A.FirstTubeObj),ThirdShieldObj(A.ThirdShieldObj),
  ThirdTubeObj(A.ThirdTubeObj),ProfMonObj(A.ProfMonObj),
  Q44Obj(A.Q44Obj),FinalTubeObj(A.FinalTubeObj),
  Q1Obj(A.Q1Obj),Q2Obj(A.Q2Obj)
  /*!
    Copy constructor
    \param A :: makeMuon to copy
  */
{}

makeMuon&
makeMuon::operator=(const makeMuon& A)
  /*!
    Assignment operator
    \param A :: makeMuon to copy
    \return *this
  */
{
  if (this!=&A)
    {
      TargShieldObj=A.TargShieldObj;
      VessBoxObj=A.VessBoxObj;
      TargetObj=A.TargetObj;
      EpbInTubeObj=A.EpbInTubeObj;
      EpbOutTubeObj=A.EpbOutTubeObj;
      FirstCollObj=A.FirstCollObj;
      ThirdCollObj=A.ThirdCollObj;
      FirstShieldObj=A.FirstShieldObj;
      FirstTubeObj=A.FirstTubeObj;
      ThirdShieldObj=A.ThirdShieldObj;
      ThirdTubeObj=A.ThirdTubeObj;
      ProfMonObj=A.ProfMonObj;
      Q44Obj=A.Q44Obj;
      FinalTubeObj=A.FinalTubeObj;
      Q1Obj=A.Q1Obj;
      Q2Obj=A.Q2Obj;
    }
  return *this;
}

makeMuon::~makeMuon()
  /*!
    Destructor
   */
{}



void 
makeMuon::build(Simulation* SimPtr,
		  const mainSystem::inputParam& IParam)
  /*!
    Carry out the full build
    \param SimPtr :: Simulation system
    \param IParam :: Input parameters
   */
{
  // For output stream
  ELog::RegMethod RControl("makeMuon","build"); 
  
  TargShieldObj->addInsertCell(74123);
  TargShieldObj->createAll(*SimPtr,World::masterOrigin());
//  
  FirstShieldObj->addInsertCell(74123);
  FirstShieldObj->createAll(*SimPtr);
  ThirdShieldObj->addInsertCell(74123);
  ThirdShieldObj->createAll(*SimPtr);
  ProfMonObj->addInsertCell(74123);
  ProfMonObj->createAll(*SimPtr); 
// target vessel    
  VessBoxObj->addInsertCell(74123);
  TargShieldObj->addToInsertChain(*VessBoxObj);  
  VessBoxObj->createAll(*SimPtr);
// target     
//  TargShieldObj->addToInsertChain(*TargetObj);  
  VessBoxObj->addToInsertChain(*TargetObj);
  TargetObj->createAll(*SimPtr,World::masterOrigin());
// epb tubes  
  EpbInTubeObj->addInsertCell(74123);
  TargShieldObj->addToInsertChain(*EpbInTubeObj);
  VessBoxObj->addToInsertChain(*EpbInTubeObj);
  EpbInTubeObj->createAll(*SimPtr,World::masterOrigin()); 
//  
  EpbOutTubeObj->addInsertCell(74123);
  TargShieldObj->addToInsertChain(*EpbOutTubeObj);
  VessBoxObj->addToInsertChain(*EpbOutTubeObj);
  EpbOutTubeObj->createAll(*SimPtr,World::masterOrigin()); 
// 1st collimator  
  FirstCollObj->addInsertCell(74123);
  TargShieldObj->addToInsertChain(*FirstCollObj);
  FirstShieldObj->addToInsertChain(*FirstCollObj);
  FirstCollObj->createAll(*SimPtr); 
  ThirdCollObj->addInsertCell(74123);

// 1st tube  
  FirstTubeObj->addInsertCell(74123);
  FirstShieldObj->addToInsertChain(*FirstTubeObj);  
  ProfMonObj->addToInsertChain(*ThirdTubeObj);  
  FirstTubeObj->createAll(*SimPtr,World::masterOrigin());    
  FirstShieldObj->addToInsertChain(*ThirdCollObj);
  ThirdShieldObj->addToInsertChain(*ThirdCollObj); 
  ThirdCollObj->createAll(*SimPtr); 
// 3rd tube  
  ThirdTubeObj->addInsertCell(74123);
  ThirdShieldObj->addToInsertChain(*ThirdTubeObj);  
  ProfMonObj->addToInsertChain(*ThirdTubeObj);  
  ThirdTubeObj->createAll(*SimPtr,World::masterOrigin());     
// Q44 quadropole and final tube      
  Q44Obj->addInsertCell(74123);
  Q44Obj->createAll(*SimPtr);  
  FinalTubeObj->addInsertCell(74123);
  ProfMonObj->addToInsertChain(*FinalTubeObj);
  Q44Obj->addToInsertChain(*FinalTubeObj);      
  FinalTubeObj->createAll(*SimPtr,World::masterOrigin());     
//
  if (!IParam.flag("exclude") || 
      !IParam.compValue("E",std::string("MuonBeamline")))
  {
    // muon beamline  
    Q1Obj->addInsertCell(74123);
    Q1Obj->createAll(*SimPtr); 
    Q2Obj->addInsertCell(74123);
    Q2Obj->createAll(*SimPtr); 
  }
    return;
}


}   // NAMESPACE ts1System

