/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   muon/makeMuon.cxx
 *
 * Copyright (c) 2004-2015 by Stuart Ansell/Goran Skoro
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
#include "Qhull.h"
#include "Simulation.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
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

#include "Debug.h"

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
  TargShieldObj->createAll(*SimPtr,World::masterOrigin());

// target vessel    
  VessBoxObj->addInsertCell(74123);
  VessBoxObj->createAll(*SimPtr,World::masterOrigin());
  attachSystem::addToInsertSurfCtrl(*SimPtr,*TargShieldObj,*VessBoxObj);
  
// target     
  TargetObj->createAll(*SimPtr,World::masterOrigin());
  attachSystem::addToInsertSurfCtrl(*SimPtr,*VessBoxObj,*TargetObj);

// epb tubes  
  EpbInTubeObj->addInsertCell(74123);
  EpbInTubeObj->createAll(*SimPtr,World::masterOrigin()); 
  attachSystem::addToInsertLineCtrl(*SimPtr,*TargShieldObj,*EpbInTubeObj);
  attachSystem::addToInsertLineCtrl(*SimPtr,*VessBoxObj,*EpbInTubeObj);
//  
  EpbOutTubeObj->addInsertCell(74123);
  EpbOutTubeObj->createAll(*SimPtr,World::masterOrigin()); 
  attachSystem::addToInsertLineCtrl(*SimPtr,*TargShieldObj,*EpbOutTubeObj);
  attachSystem::addToInsertLineCtrl(*SimPtr,*VessBoxObj,*EpbOutTubeObj);

// 1st collimator  
  FirstShieldObj->addInsertCell(74123);
  FirstShieldObj->createAll(*SimPtr,World::masterOrigin());

  FirstCollObj->addInsertCell(74123);
  FirstCollObj->createAll(*SimPtr,World::masterOrigin());     
  attachSystem::addToInsertLineCtrl(*SimPtr,*FirstShieldObj,*FirstCollObj);
  attachSystem::addToInsertLineCtrl(*SimPtr,*TargShieldObj,*FirstCollObj);

// muon beam window 1    
  BWin1Obj->createAll(*SimPtr,World::masterOrigin());    
  attachSystem::addToInsertLineCtrl(*SimPtr,*TargShieldObj,*BWin1Obj);
  attachSystem::addToInsertLineCtrl(*SimPtr,*VessBoxObj,*BWin1Obj);

// muon beam window 2    
  BWin2Obj->createAll(*SimPtr,World::masterOrigin());    
  attachSystem::addToInsertLineCtrl(*SimPtr,*TargShieldObj,*BWin2Obj);
  attachSystem::addToInsertLineCtrl(*SimPtr,*VessBoxObj,*BWin2Obj);
// room
  MuRoomObj->addInsertCell(74123);
  MuRoomObj->createAll(*SimPtr,World::masterOrigin());    

  // position ring 1    
  PosRing1Obj->createAll(*SimPtr,World::masterOrigin());  
  attachSystem::addToInsertLineCtrl(*SimPtr,*TargShieldObj,*PosRing1Obj);
  attachSystem::addToInsertLineCtrl(*SimPtr,*MuRoomObj,*PosRing1Obj);
//  
  if (!IParam.flag("exclude") || 
      !IParam.compValue("E",std::string("EPBBeamline")))
    {
      ThirdShieldObj->addInsertCell(74123);
      ThirdShieldObj->createAll(*SimPtr,World::masterOrigin());

      ProfMonObj->addInsertCell(74123);
      ProfMonObj->createAll(*SimPtr,World::masterOrigin());
      ThirdCollObj->addInsertCell(74123);


      // 1st tube  
      FirstTubeObj->addInsertCell(74123);
      FirstTubeObj->createAll(*SimPtr,World::masterOrigin());    
      attachSystem::addToInsertLineCtrl(*SimPtr,*FirstShieldObj,*FirstTubeObj);

      ThirdCollObj->createAll(*SimPtr,World::masterOrigin()); 
      attachSystem::addToInsertLineCtrl(*SimPtr,*ThirdShieldObj,*ThirdCollObj);
      attachSystem::addToInsertLineCtrl(*SimPtr,*ProfMonObj,*ThirdCollObj);
      attachSystem::addToInsertLineCtrl(*SimPtr,*FirstShieldObj,*ThirdCollObj);

      // 3rd tube  
      ThirdTubeObj->addInsertCell(74123);
      ThirdTubeObj->createAll(*SimPtr,World::masterOrigin());     
      attachSystem::addToInsertLineCtrl(*SimPtr,*ThirdShieldObj,*ThirdTubeObj);
      attachSystem::addToInsertLineCtrl(*SimPtr,*ProfMonObj,*ThirdTubeObj);

      // Q44 quadropole and final tube      
      Q44Obj->addInsertCell(74123);
      Q44Obj->createAll(*SimPtr,World::masterOrigin());  

      FinalTubeObj->addInsertCell(74123);
      FinalTubeObj->createAll(*SimPtr,World::masterOrigin());     
      attachSystem::addToInsertLineCtrl(*SimPtr,*ProfMonObj,*FinalTubeObj);
      attachSystem::addToInsertLineCtrl(*SimPtr,*Q44Obj,*FinalTubeObj);
    }
  //
  if (!IParam.flag("exclude") || 
      !IParam.compValue("E",std::string("MuonBeamline")))
    {
      // +++++++++++++++++++++++++++++++++++  muon beamline  
      Q1Obj->createAll(*SimPtr,World::masterOrigin());
      attachSystem::addToInsertLineCtrl(*SimPtr,*MuRoomObj,*Q1Obj);

      // Muon vacuum tube 1    
      MuTube1Obj->createAll(*SimPtr,World::masterOrigin());         
      attachSystem::addToInsertLineCtrl(*SimPtr,*Q1Obj,*MuTube1Obj);
      attachSystem::addToInsertLineCtrl(*SimPtr,*MuRoomObj,*MuTube1Obj);

      Q2Obj->createAll(*SimPtr,World::masterOrigin());
      attachSystem::addToInsertLineCtrl(*SimPtr,*MuRoomObj,*Q2Obj);

      CernFront->createAll(*SimPtr,World::masterOrigin());
      attachSystem::addToInsertLineCtrl(*SimPtr,*MuRoomObj,*CernFront);

      CernBack->createAll(*SimPtr,World::masterOrigin());     
      attachSystem::addToInsertLineCtrl(*SimPtr,*MuRoomObj,*CernBack);

      // Muon vacuum tube 2    
      //      MuTube2Obj->addInsertCell(74123);
      MuTube2Obj->createAll(*SimPtr,World::masterOrigin());  
      attachSystem::addToInsertLineCtrl(*SimPtr,*Q1Obj,*MuTube2Obj);
      attachSystem::addToInsertLineCtrl(*SimPtr,*Q2Obj,*MuTube2Obj);
      attachSystem::addToInsertLineCtrl(*SimPtr,*CernFront,*MuTube2Obj);
      attachSystem::addToInsertLineCtrl(*SimPtr,*CernBack,*MuTube2Obj);
      attachSystem::addToInsertLineCtrl(*SimPtr,*MuRoomObj,*MuTube2Obj);

      // position ring 2    
      //      PosRing2Obj->addInsertCell(74123);
      PosRing2Obj->createAll(*SimPtr,World::masterOrigin());      
      attachSystem::addToInsertLineCtrl(*SimPtr,*MuRoomObj,*PosRing2Obj);

      // Muon vacuum tube 3    
      MuTube3Obj->createAll(*SimPtr,World::masterOrigin());  
      attachSystem::addToInsertLineCtrl(*SimPtr,*MuRoomObj,*MuTube3Obj);

      // position ring 3    
      PosRing3Obj->createAll(*SimPtr,World::masterOrigin());
      attachSystem::addToInsertLineCtrl(*SimPtr,*MuRoomObj,*PosRing3Obj);

      // Shield plug 
      ShieldPlugObj->addInsertCell(74123);
      ShieldPlugObj->createAll(*SimPtr,World::masterOrigin());

      // Muon vacuum tube 4 & 5    
      MuTube4Obj->createAll(*SimPtr,World::masterOrigin());
      attachSystem::addToInsertLineCtrl(*SimPtr,*ShieldPlugObj,*MuTube4Obj);

      MuTube5Obj->addInsertCell(74123);
      MuTube5Obj->createAll(*SimPtr,World::masterOrigin());      
      attachSystem::addToInsertLineCtrl(*SimPtr,*ShieldPlugObj,*MuTube5Obj);
      // position ring 4    
      PosRing4Obj->createAll(*SimPtr,World::masterOrigin());        
    }
  return;
}


}   // NAMESPACE muSystem

