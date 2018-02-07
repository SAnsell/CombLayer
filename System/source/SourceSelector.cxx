/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   source/SourceSelector.cxx
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#include <memory>
#include <array>

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
#include "Source.h"
#include "SrcItem.h"
#include "SrcData.h"
#include "surfRegister.h"
#include "HeadRule.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "LinkSupport.h"
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
#include "particleConv.h"
#include "SourceBase.h"
#include "WorkData.h"
#include "World.h"
#include "activeUnit.h"
#include "activeFluxPt.h"
#include "ActivationSource.h"
#include "SourceSelector.h"

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
  ELog::RegMethod RegA("SourceSelector[F]","sourceSelection");
  
  const FuncDataBase& Control=System.getDataBase();

  const ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  const std::string DObj=IParam.getValue<std::string>("sdefObj",0);
  const std::string DSnd=IParam.getValue<std::string>("sdefObj",1);
  const std::string Dist=IParam.getValue<std::string>("sdefObj",2);

  Geometry::Vec3D DOffsetStep;
  double D;
  if (!StrFunc::convert(Dist,DOffsetStep) && 
      !StrFunc::convert(Dist,D))
    DOffsetStep[1]=D;
  
  const attachSystem::FixedComp& FC=
    (DObj.empty()) ?  World::masterOrigin() :
    *(OR.getObjectThrow<attachSystem::FixedComp>(DObj,"Object not found"));
  
  const long int linkIndex=(DSnd.empty()) ?  0 :
    attachSystem::getLinkIndex(DSnd)/1000;

  // NOTE: No return to allow active SSW systems  
  std::string sdefType=IParam.getValue<std::string>("sdefType");
  std::string sName;
  


  if (sdefType.empty() && IParam.hasKey("kcode") &&
      IParam.flag("kcode"))
    sName="kcode";

  else if (sdefType=="TS1")                            // parabolic source
    sName=SDef::createTS1Source(Control,FC,linkIndex);

  else if (sdefType=="TS1Gauss")                   // TS1Gauss
    sName=SDef::createTS1GaussianSource(Control,FC,linkIndex); 

  else if (sdefType=="TS1GaussNew")                // TS1NewGauss
    sName=SDef::createTS1GaussianNewSource(Control,FC,linkIndex);     

  else if (sdefType=="TS1Muon")                    // TS1Muon
    sName=SDef::createTS1MuonSource(Control,FC,linkIndex); 

  else if (sdefType=="TS3Expt")                    
    sName=SDef::createTS3ExptSource(Control,FC,linkIndex); 

  else if (sdefType=="TS1EPBColl" || sdefType=="TS1EpbColl")
    sName=SDef::createTS1EPBCollSource(Control,FC,linkIndex); 

  else if (sdefType=="Bilbao")                    // bilbauSource
    sName=SDef::createBilbaoSource(Control,FC,linkIndex);

  else if (sdefType=="Wiggler")                       // blader wiggler
    sName=SDef::createWigglerSource(Control,FC,linkIndex);

  else if (sdefType=="ess")                       // essSource
    sName=SDef::createESSSource(Control,FC,linkIndex);

  else if (sdefType=="essLinac")                 // essLinacSource
    sName=SDef::createESSLinacSource(Control,FC,linkIndex);
  
  else if (sdefType=="D4C")
    sName=SDef::createD4CSource(Control,FC,linkIndex);

  else if (sdefType=="Sinbad" || sdefType=="sinbad")
    sName=SDef::createSinbadSource(Control,FC,linkIndex);

  else if (sdefType=="Gamma" || sdefType=="gamma")
    sName=SDef::createGammaSource(Control,"gammaSource",
				  FC,linkIndex);
  
  else if (sdefType=="Laser" || sdefType=="laser")
    sName=SDef::createGammaSource(Control,"laserSource",
				  FC,linkIndex);
  
  else if (sdefType=="Activation" || sdefType=="activation")
    activationSelection(System,IParam);

  else if (sdefType=="Point" || sdefType=="point")
    {
      sName=SDef::createPointSource(Control,"pointSource",
			      FC,linkIndex);
    }
  else if (sdefType=="Disk" || sdefType=="disk")
    {
      sName=SDef::createGammaSource(Control,"diskSource",FC,
				    linkIndex);
    }
  else if (sdefType=="Beam" || sdefType=="beam")
    {
      sName=SDef::createBeamSource(Control,"beamSource",
			     FC,linkIndex);
    }
  else if (sdefType=="LENS" || sdefType=="lens")
    {
      sName=SDef::createLensSource(Control,FC,linkIndex);
    }
  else if (sdefType=="TS2")
    {  
      // Basic TS2 source
      // NOTE THIS IS the old stupid TS2 origin system
      sName=SDef::createTS2Source(Control,World::masterTS2Origin(),0);
    }
  else if (sdefType=="kcode")
    {
      ELog::EM<<"kcode sdef selected"<<ELog::endDiag;
    }
  else
    {
      ELog::EM<<"sdefType :\n"
	"Activation :: Activation source \n"
	"TS1 :: Target station one \n"
	"TS2 :: Target station two \n"
	"TS1Gauss :: Target station one [old gaussian beam] sigma = 15 mm \n"
	"TS1GaussNew :: Target station one [old gaussian beam] sigma = 18 mm \n"	
	"TS1Muon :: TS1 [proton beam for muon target] \n"
	"TS1EpbColl :: TS1 [proton beam for 3rd collimator] \n"			
	"ess :: ESS beam proton\n"
	"Bilbao :: Bilbao beam proton\n"
	"Laser :: Laser D/T fussion source\n"
	"Point :: Test point source\n"
	"Beam :: Test Beam [Radial] source \n"
	"Wiggler :: Wiggler Source for balder \n"
	"D4C :: D4C neutron beam"<<ELog::endBasic;
    }

  if (!IParam.flag("sdefVoid") && !sName.empty())
    System.setSourceName(sName);
  
  return;
}

void
activationSelection(Simulation& System,
		    const mainSystem::inputParam& IParam)
  /*!
    Select all the info for activation output from
    fluxes.
    \param System :: Simuation to use
    \param IParam :: input parameters
   */
{
  ELog::RegMethod RegA("SourceSelector","activationSelection");

  const size_t nP=IParam.setCnt("activation");

  Geometry::Vec3D APt,BPt;
  std::string OName="Data.ssw";
  std::string cellDir="Cell";
  size_t timeSeg(1);
  size_t nVol=System.getPC().getNPS();
  Geometry::Vec3D weightPt;
  double weightDist(-1.0);
  double scale(1.0);

  if (nP)
    {
      std::shared_ptr<SDef::ActivationSource> AS =
	std::dynamic_pointer_cast<SDef::ActivationSource>
	(SDef::makeActivationSource("ActivationSource"));
      if (!AS)
	throw ColErr::DynamicConv("SourceBase","ActivationSource",
				  " convertion ");
    
  
      for(size_t index=0;index<nP;index++)
	{
	  std::string eMess
	    ("Insufficient item for activation["+StrFunc::makeString(index)+"]");
	  const std::string key=
	    IParam.getValueError<std::string>("activation",index,0,eMess);
	  eMess+=" at key "+key;
	  if (key=="help")
	    {
	      ELog::EM<<"activation help:"<<ELog::endBasic;
	      ELog::EM<<"-- timeStep index :: sets the time step from cinder\n"
		      <<"-- box Vec3D Vec3D :: corner points of box to sample\n"
		      <<"-- out string :: output file [def:Data.ssw]\n"
		      <<"-- cell string :: cell header name [def: Cell]\n"
		      <<"-- nVol size :: number of point for vol sample [def: npts]"
		      <<"-- weightPoint :: Point dist :: Scale to distance "
		      <<"-- weightPlane :: Vec3D : Axis3D "
		      <<ELog::endBasic;
	    }
	  else if (key=="box")
	    {
	      size_t ptI(1);
	      APt=IParam.getCntVec3D("activation",index,ptI,
				     "Start point of box not defined");
	      BPt=IParam.getCntVec3D("activation",index,ptI,
				     "End point of box not defined");
	    }
	  else if (key=="out")
	    {
	      OName=IParam.getValueError<std::string>("activation",index,1,eMess);
	    }
	  else if (key=="cell" || key=="cellDir")
	    {
	      cellDir=IParam.getValueError<std::string>("activation",index,1,eMess);
	    }
	  else if (key=="timeStep")
	    {
	      timeSeg=IParam.getValueError<size_t>("activation",index,1,eMess);
	    }
	  else if (key=="weightPoint")
	    {
	      size_t itemIndex(1);
	      weightPt=IParam.getCntVec3D("activation",index,itemIndex,eMess);
	      weightDist=IParam.getValueError<double>
		("activation",index,itemIndex,eMess);
	    }
	  else if (key=="scale")
	    {
	      scale=IParam.getValueError<double>("activation",index,1,eMess);
	    }
	  else if (key=="nVol")
	    {
	      nVol=IParam.getValueError<size_t>("activation",index,1,eMess);
	    }
	  else if (key=="weightPlane")
	    {
	      size_t itemCnt(1);
	      const Geometry::Vec3D PPt=
		IParam.getCntVec3D("activation",index,itemCnt,eMess);
	      const Geometry::Vec3D PAxis=
		IParam.getCntVec3D("activation",index,itemCnt,eMess);
	      AS->setPlane(PPt,PAxis,2.0);
	    }
	  else
	    {
	      throw ColErr::InContainerError<std::string>
		(key,"Key not found for activation");
	    }
	}
            
      AS->setBox(APt,BPt);
      AS->setTimeSegment(timeSeg);
      AS->setNPoints(nVol);
      AS->setWeightPoint(weightPt,weightDist);
      AS->setScale(scale);
      AS->createAll(System,cellDir,OName);
    }

  return;
}
  
  

  
} // NAMESPACE SDef
