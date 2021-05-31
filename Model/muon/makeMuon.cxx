/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   muon/makeMuon.cxx
 *
 * Copyright (c) 2004-2019 by Stuart Ansell/Goran Skoro
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
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "World.h"
#include "AttachSupport.h"
#include "targSimpleShield.h"
#include "targetVesselBox.h"
#include "muonCarbonTarget.h"
#include "muonTube.h"
#include "coneColl.h"
#include "cShieldLayer.h"
#include "profileMon.h"
#include "muonQ1.h"
#include "muBeamWindow.h"
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
  MuRoomObj(new muSystem::targSimpleShield("MuRoom")), 
  Q1Obj(new muSystem::muonQ1("Q1quad")),
  Q2Obj(new muSystem::muonQ1("Q2quad")),
  CernFront(new muSystem::targSimpleShield("CernFront")), 
  CernBack(new muSystem::targSimpleShield("CernBack")),
  BWin1Obj(new muSystem::muBeamWindow("BWin1")), 
  BWin2Obj(new muSystem::muBeamWindow("BWin2")),
  PosRing1Obj(new muSystem::muonTube("PosRing1")),
  MuTube1Obj(new muSystem::muonTube("MuTube1")),     
  MuTube2Obj(new muSystem::muonTube("MuTube2")),       
  PosRing2Obj(new muSystem::muonTube("PosRing2")),
  MuTube3Obj(new muSystem::muonTube("MuTube3")),       
  PosRing3Obj(new muSystem::muonTube("PosRing3")),    
  MuTube4Obj(new muSystem::muonTube("MuTube4")),   
  MuTube5Obj(new muSystem::muonTube("MuTube5")),   
  PosRing4Obj(new muSystem::muonTube("PosRing4")),
  ShieldPlugObj(new muSystem::cShieldLayer("ShieldPlug"))
         
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
  OR.addObject(MuRoomObj); 
  OR.addObject(Q1Obj); 
  OR.addObject(Q2Obj);
  OR.addObject(CernFront);
  OR.addObject(CernBack);    
  OR.addObject(BWin1Obj);  
  OR.addObject(BWin2Obj);  
  OR.addObject(PosRing1Obj);
  OR.addObject(MuTube1Obj);     
  OR.addObject(MuTube2Obj);       
  OR.addObject(PosRing2Obj);
  OR.addObject(MuTube3Obj);       
  OR.addObject(PosRing3Obj);    
  OR.addObject(MuTube4Obj);   
  OR.addObject(MuTube5Obj);   
  OR.addObject(PosRing4Obj);
  OR.addObject(ShieldPlugObj);  
  
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
  TargShieldObj->createAll(*SimPtr,World::masterOrigin(),0);

// target vessel    
  VessBoxObj->addInsertCell(74123);
  VessBoxObj->createAll(*SimPtr,World::masterOrigin(),0);
  attachSystem::addToInsertSurfCtrl(*SimPtr,*TargShieldObj,*VessBoxObj);
  
// target     
  TargetObj->createAll(*SimPtr,World::masterOrigin(),0);
  attachSystem::addToInsertSurfCtrl(*SimPtr,*VessBoxObj,*TargetObj);

// epb tubes  
  EpbInTubeObj->addInsertCell(74123);
  EpbInTubeObj->createAll(*SimPtr,World::masterOrigin(),0); 
  attachSystem::addToInsertForced(*SimPtr,*TargShieldObj,*EpbInTubeObj);
  //  attachSystem::addToInsertForced(*SimPtr,*VessBoxObj,*EpbInTubeObj);
  attachSystem::addToInsertForced(*SimPtr,*VessBoxObj,*EpbInTubeObj);

//  
  EpbOutTubeObj->addInsertCell(74123);
  EpbOutTubeObj->createAll(*SimPtr,World::masterOrigin(),0); 
  attachSystem::addToInsertForced(*SimPtr,*TargShieldObj,*EpbOutTubeObj);
  attachSystem::addToInsertForced(*SimPtr,*VessBoxObj,*EpbOutTubeObj);

// 1st collimator  
  FirstShieldObj->addInsertCell(74123);
  FirstShieldObj->createAll(*SimPtr,World::masterOrigin(),0);

  FirstCollObj->addInsertCell(74123);
  FirstCollObj->createAll(*SimPtr,World::masterOrigin(),0);     
  attachSystem::addToInsertForced(*SimPtr,*FirstShieldObj,*FirstCollObj);
  attachSystem::addToInsertForced(*SimPtr,*TargShieldObj,*FirstCollObj);

// muon beam window 1    
  BWin1Obj->createAll(*SimPtr,World::masterOrigin(),0);    
  attachSystem::addToInsertForced(*SimPtr,*TargShieldObj,*BWin1Obj);
  attachSystem::addToInsertForced(*SimPtr,*VessBoxObj,*BWin1Obj);

// muon beam window 2    
  BWin2Obj->createAll(*SimPtr,World::masterOrigin(),0);    
  attachSystem::addToInsertForced(*SimPtr,*TargShieldObj,*BWin2Obj);
  attachSystem::addToInsertForced(*SimPtr,*VessBoxObj,*BWin2Obj);
// room
  MuRoomObj->addInsertCell(74123);
  MuRoomObj->createAll(*SimPtr,World::masterOrigin(),0);    

  // position ring 1    
  PosRing1Obj->createAll(*SimPtr,World::masterOrigin(),0);  
  attachSystem::addToInsertForced(*SimPtr,*TargShieldObj,*PosRing1Obj);
  attachSystem::addToInsertForced(*SimPtr,*MuRoomObj,*PosRing1Obj);
  //  
  if (!IParam.flag("exclude") || 
      !IParam.compValue("E",std::string("EPBBeamline")))
    {
      ThirdShieldObj->addInsertCell(74123);
      ThirdShieldObj->createAll(*SimPtr,World::masterOrigin(),0);

      ProfMonObj->addInsertCell(74123);
      ProfMonObj->createAll(*SimPtr,World::masterOrigin(),0);
      ThirdCollObj->addInsertCell(74123);


      // 1st tube  
      FirstTubeObj->addInsertCell(74123);
      FirstTubeObj->createAll(*SimPtr,World::masterOrigin(),0);    
      attachSystem::addToInsertForced(*SimPtr,*FirstShieldObj,*FirstTubeObj);

      ThirdCollObj->createAll(*SimPtr,World::masterOrigin(),0); 
      attachSystem::addToInsertForced(*SimPtr,*ThirdShieldObj,*ThirdCollObj);
      attachSystem::addToInsertForced(*SimPtr,*ProfMonObj,*ThirdCollObj);
      attachSystem::addToInsertForced(*SimPtr,*FirstShieldObj,*ThirdCollObj);

      // 3rd tube  
      ThirdTubeObj->addInsertCell(74123);
      ThirdTubeObj->createAll(*SimPtr,World::masterOrigin(),0);     
      attachSystem::addToInsertForced(*SimPtr,*ThirdShieldObj,*ThirdTubeObj);
      attachSystem::addToInsertForced(*SimPtr,*ProfMonObj,*ThirdTubeObj);

      // Q44 quadropole and final tube      
      Q44Obj->addInsertCell(74123);
      Q44Obj->createAll(*SimPtr,World::masterOrigin(),0);  

      FinalTubeObj->addInsertCell(74123);
      FinalTubeObj->createAll(*SimPtr,World::masterOrigin(),0);     
      attachSystem::addToInsertForced(*SimPtr,*ProfMonObj,*FinalTubeObj);
      attachSystem::addToInsertForced(*SimPtr,*Q44Obj,*FinalTubeObj);
    }
  //
  if (!IParam.flag("exclude") || 
      !IParam.compValue("E",std::string("MuonBeamline")))
    {
      // +++++++++++++++++++++++++++++++++++  muon beamline  
      Q1Obj->createAll(*SimPtr,World::masterOrigin(),0);
      attachSystem::addToInsertForced(*SimPtr,*MuRoomObj,*Q1Obj);

      // Muon vacuum tube 1    
      MuTube1Obj->createAll(*SimPtr,World::masterOrigin(),0);         
      attachSystem::addToInsertForced(*SimPtr,*Q1Obj,*MuTube1Obj);
      attachSystem::addToInsertForced(*SimPtr,*MuRoomObj,*MuTube1Obj);

      Q2Obj->createAll(*SimPtr,World::masterOrigin(),0);
      attachSystem::addToInsertForced(*SimPtr,*MuRoomObj,*Q2Obj);

      CernFront->createAll(*SimPtr,World::masterOrigin(),0);
      attachSystem::addToInsertForced(*SimPtr,*MuRoomObj,*CernFront);

      CernBack->createAll(*SimPtr,World::masterOrigin(),0);     
      attachSystem::addToInsertForced(*SimPtr,*MuRoomObj,*CernBack);

      // Muon vacuum tube 2    
      //      MuTube2Obj->addInsertCell(74123);
      MuTube2Obj->createAll(*SimPtr,World::masterOrigin(),0);  
      attachSystem::addToInsertForced(*SimPtr,*Q1Obj,*MuTube2Obj);
      attachSystem::addToInsertForced(*SimPtr,*Q2Obj,*MuTube2Obj);
      attachSystem::addToInsertForced(*SimPtr,*CernFront,*MuTube2Obj);
      attachSystem::addToInsertForced(*SimPtr,*CernBack,*MuTube2Obj);
      attachSystem::addToInsertForced(*SimPtr,*MuRoomObj,*MuTube2Obj);

      // position ring 2    
      //      PosRing2Obj->addInsertCell(74123);
      PosRing2Obj->createAll(*SimPtr,World::masterOrigin(),0);      
      attachSystem::addToInsertForced(*SimPtr,*MuRoomObj,*PosRing2Obj);

      // Muon vacuum tube 3    
      MuTube3Obj->createAll(*SimPtr,World::masterOrigin(),0);  
      attachSystem::addToInsertForced(*SimPtr,*MuRoomObj,*MuTube3Obj);

      // position ring 3    
      PosRing3Obj->createAll(*SimPtr,World::masterOrigin(),0);
      attachSystem::addToInsertForced(*SimPtr,*MuRoomObj,*PosRing3Obj);

      // Shield plug 
      ShieldPlugObj->addInsertCell(74123);
      ShieldPlugObj->createAll(*SimPtr,World::masterOrigin(),0);
      attachSystem::addToInsertForced(*SimPtr,*MuRoomObj,*ShieldPlugObj);

      // Muon vacuum tube 4 & 5    
      MuTube4Obj->createAll(*SimPtr,World::masterOrigin(),0);
      attachSystem::addToInsertForced(*SimPtr,*ShieldPlugObj,*MuTube4Obj);
      attachSystem::addToInsertForced(*SimPtr,*MuTube4Obj,*MuTube2Obj);

      MuTube5Obj->addInsertCell(74123);
      MuTube5Obj->createAll(*SimPtr,World::masterOrigin(),0);      
      attachSystem::addToInsertForced(*SimPtr,*ShieldPlugObj,*MuTube5Obj);
      // position ring 4    
      PosRing4Obj->createAll(*SimPtr,World::masterOrigin(),0);        
    }
  return;
}


}   // NAMESPACE muSystem

