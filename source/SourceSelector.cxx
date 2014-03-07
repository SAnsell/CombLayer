/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   source/SourceSelector.cxx
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
#include <cmath>
#include <complex>
#include <list>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <boost/shared_ptr.hpp>
#include <boost/array.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "doubleErr.h"
#include "Triple.h"
#include "NRange.h"
#include "NList.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "KCode.h"
#include "Source.h"
#include "SrcItem.h"
#include "SrcData.h"
#include "surfRegister.h"
#include "HeadRule.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "LinearComp.h"
#include "SecondTrack.h"
#include "inputParam.h"
#include "PhysCard.h"
#include "LSwitchCard.h"
#include "ModeCard.h"
#include "PhysImp.h"
#include "PhysicsCards.h"
#include "Simulation.h"
#include "SourceCreate.h"
#include "localRotate.h"
#include "masterRotate.h"
#include "objectRegister.h"
#include "ChipIRSource.h"

namespace SDef
{

void 
sourceSelection(Simulation& System,
		const mainSystem::inputParam& IParam)
  /*!
    Build the source based on the input parameter table
    \param System :: Simulation to use
    \param IParam :: Input parameter
  */
{
  ELog::RegMethod RegA("SourceSelector","sourceSelection");
  const FuncDataBase& Control=System.getDataBase();
  SDef::Source& sourceCard=System.getPC().getSDefCard();
  const masterRotate& MR = masterRotate::Instance();
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  if (IParam.flag("sdefVoid"))
    {
      sourceCard.deactivate();
      return;      
    }

  // If a chipIR style directional source
  if (IParam.flag("sdefFile"))
    {
      const std::string FName=IParam.getValue<std::string>("sdefFile");
      const std::string DObj=IParam.getValue<std::string>("sdefObj");
      const int index=IParam.getValue<int>("sdefIndex")-1;

      const attachSystem::SecondTrack* SPtr(0);
      const attachSystem::FixedComp* LPtr(0);
  
      if (DObj=="shutter" || DObj=="torpedo")
	{
	  LPtr=OR.getObject<attachSystem::FixedComp>
	    (StrFunc::makeString(DObj,index));
	}
      else
	LPtr=OR.getObject<attachSystem::FixedComp>(DObj);

      SPtr=dynamic_cast<const attachSystem::SecondTrack*>(LPtr);
        
      if (!LPtr)
	{
	  ELog::EM<<"Failed to find object for :"<<DObj<<ELog::endErr;
	  return;
	}
      
      // Construct CSDEF :
      SDef::ChipIRSource CSdef;
      const double Angle=
	IParam.getFlagDef<double>("sdefAngle",Control,"chipSourceAngle"); 
      const double Radius=
	IParam.getFlagDef<double>("sdefRadius",Control,"chipSourceRadial"); 
      Geometry::Vec3D SPos;
      if ( IParam.flag("sdefPos") )
	{
	  // Care need to invert the position:
	  SPos=IParam.getValue<Geometry::Vec3D>("sdefPos");
	  SPos=MR.reverseRotate(SPos);           // View point	    
	}
      else
	SPos=LPtr->getCentre();
      
      // Handle direction
      Geometry::Vec3D SDir((SPtr) ? SPtr->getBeamAxis() : LPtr->getY());
      if (IParam.flag("sdefVec"))
	SDir=IParam.getValue<Geometry::Vec3D>("sdefVec");
      else if (IParam.flag("sdefZRot"))
	{
	  const double rotAngle=IParam.getValue<double>("sdefZRot");
	  const Geometry::Quaternion Qz=
	    Geometry::Quaternion::calcQRotDeg(rotAngle,LPtr->getX());
	  Qz.rotate(SDir);
	}
      
      CSdef.createAll(FName,SDir,SPos,Angle,Radius,sourceCard);

      if (IParam.flag("ECut"))
	CSdef.setCutEnergy(IParam.getValue<double>("ECut"));
      return;
    }
  const std::string sdefType=IParam.getValue<std::string>("sdefType");
  ELog::EM<<"SDEF == "<<IParam.getValue<std::string>("sdefType")<<
    ELog::endDebug;
  if (sdefType=="TS1")
    SDef::createTS1Source(Control,sourceCard);
  else if (sdefType=="TS1Gauss") 
    SDef::createTS1GaussianSource(Control,sourceCard);
  else if (sdefType=="TS1GaussNew")
    SDef::createTS1GaussianNewSource(Control,sourceCard);     
  else if (sdefType=="TS1Muon")
    SDef::createTS1MuonSource(Control,sourceCard); // Goran
  else if (sdefType=="TS1EpbColl")
    SDef::createTS1EpbCollSource(Control,sourceCard); // Goran
  else if (sdefType=="Bilbao")
    SDef::createBilbaoSource(Control,sourceCard);
  else if (sdefType=="ess")
    SDef::createESSSource(Control,sourceCard);
  else if (sdefType=="Sinbad" || sdefType=="sinbad")
    SDef::createSinbadSource(Control,sourceCard);
  else if (sdefType=="TS2")
    {
  // Basic TS2 source
      if(IParam.hasKey("horr") && IParam.flag("horr"))
	sourceCard.setTransform(System.createSourceTransform());
  
      // Basic TS2 source
      SDef::createTS2Source(sourceCard);
    }
  else
    {
      ELog::EM<<"sdefType :\n"
	"TS1 :: Target station one \n"
	"TS2 :: Target station two \n"
	"TS1Gauss :: Target station one [old gaussian beam] sigma = 15 mm \n"
	"TS1GaussNew :: Target station one [old gaussian beam] sigma = 18 mm \n"	
	"TS1Muon :: TS1 [proton beam for muon target] \n"
	"TS1EpbColl :: TS1 [proton beam for 3rd collimator] \n"			
	"ess :: ESS beam proton"
	"Bilbao :: Bilbao beam proton"<<ELog::endBasic;
    }
	

  return;
}

} // NAMESPACE SDef
