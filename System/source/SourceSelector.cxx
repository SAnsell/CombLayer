/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   source/SourceSelector.cxx
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
#include "WorkData.h"
#include "activeUnit.h"
#include "activeFluxPt.h"
#include "ActiveWeight.h"
#include "ActivationSource.h"
#include "SourceSelector.h"

namespace SDef
{

long int
getLinkIndex(const std::string& Snd) 
  /*!
    Convert a name front back etc into a standard link number
    \param Snd :: Snd link work    
    \return link number [-ve for beamFront/beamBack]
  */
{
  ELog::RegMethod RegA("SourceSelector[F]","getLinkIndex");
  
  long int linkPt(0);
  if (!Snd.empty() && !StrFunc::convert(Snd,linkPt))
    {
      if (Snd=="origin") 
	linkPt=0;
      else if (Snd=="front") 
	linkPt=1;
      else if (Snd=="back")
	linkPt=2;
      else if (Snd=="beamFront")
	linkPt=-1;
      else if (Snd=="beamBack")
	linkPt=-2;
      else 
	throw ColErr::InContainerError<std::string>(Snd,"String");
    }
  return linkPt;
}

void
processSDefFile(const mainSystem::inputParam& IParam,
		const FuncDataBase& Control,
		const std::string& DObj,
		SDef::Source& sourceCard)
  /*!
    Process the case of an sdefFile [spectrum] -- use 
    the void sdef card
    \param IParam :: input Parameters
    \param Control :: dataBase for variables
    \param DObj :: Name of object
   */
{
  ELog::RegMethod RegA("SourceSelector","processSDefFile");

  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();
  const masterRotate& MR = masterRotate::Instance();
  
  const std::string FName=IParam.getValue<std::string>("sdefFile");
  
  const int index=IParam.getValue<int>("sdefIndex")-1;

  const attachSystem::SecondTrack* SPtr(0);
  const attachSystem::FixedComp* LPtr(0);
  
  if (DObj=="shutter" || DObj=="torpedo")
    LPtr=OR.getObjectThrow<attachSystem::FixedComp>
      (StrFunc::makeString(DObj,index),DObj+"FixedComp");
  else
    LPtr=OR.getObjectThrow<attachSystem::FixedComp>(DObj,"FixedComp");
  
  SPtr=dynamic_cast<const attachSystem::SecondTrack*>(LPtr);
  
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
  SDef::Source& sourceCard=System.getPC().getSDefCard();

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

  const attachSystem::FixedComp* FCPtr=
    OR.getObject<attachSystem::FixedComp>(DObj);

  const long int linkIndex=getLinkIndex(DSnd);

  // NOTE: No return to allow active SSW systems

  // If a chipIR style directional source
  if (IParam.flag("sdefFile"))
    {
      processSDefFile(IParam,Control,DObj,sourceCard);
      return;
    }
  
  std::string sdefType=IParam.getValue<std::string>("sdefType");
  if (sdefType.empty() && IParam.hasKey("kcode") &&
      IParam.flag("kcode"))
    sdefType="kcode";
  
  if (sdefType=="TS1")
    SDef::createTS1Source(Control,sourceCard);
  else if (sdefType=="TS1Gauss") 
    SDef::createTS1GaussianSource(Control,sourceCard);
  else if (sdefType=="TS1GaussNew")
    SDef::createTS1GaussianNewSource(Control,sourceCard);     
  else if (sdefType=="TS1Muon")
    SDef::createTS1MuonSource(Control,sourceCard); // Goran
  else if (sdefType=="TS3Expt")
    SDef::createTS3ExptSource(Control,sourceCard); 
  else if (sdefType=="TS1EpbColl")
    SDef::createTS1EpbCollSource(Control,sourceCard); // Goran
  else if (sdefType=="Bilbao")
    SDef::createBilbaoSource(Control,sourceCard);
  else if (sdefType=="ess")
    SDef::createESSSource(Control,sourceCard);
  else if (sdefType=="essLinac")
    SDef::createESSLinacSource(Control,sourceCard);
  else if (sdefType=="essPort")
    SDef::createESSPortSource(Control,FCPtr,linkIndex,
			      sourceCard);
  
  else if (sdefType=="D4C")
    SDef::createD4CSource(Control,sourceCard);
  else if (sdefType=="Sinbad" || sdefType=="sinbad")
    SDef::createSinbadSource(Control,sourceCard);
  else if (sdefType=="Gamma" || sdefType=="gamma")
    SDef::createGammaSource(Control,"gammaSource",sourceCard);
  else if (sdefType=="Laser" || sdefType=="laser")
    SDef::createGammaSource(Control,"laserSource",sourceCard);
  else if (sdefType=="Activation" || sdefType=="activation")
    activationSelection(System,IParam);
  else if (sdefType=="ActiveWeight" || sdefType=="activeWeight")
    activeWeight(System,IParam);
  else if (sdefType=="Point" || sdefType=="point")
    {
      if (FCPtr)
        {
          SDef::createPointSource(Control,"pointSource",
                                  *FCPtr,linkIndex,DOffsetStep,sourceCard);
        }
      else
	{
          ELog::EM<<"Free Point Source "<<ELog::endDiag;
	  SDef::createPointSource(Control,"pointSource",DObj,sourceCard);
	}
    }
  else if (sdefType=="Disk" || sdefType=="disk")
    {
      if (FCPtr)
	SDef::createGammaSource(Control,"diskSource",
				*FCPtr,linkIndex,sourceCard);
      else
	SDef::createGammaSource(Control,"diskSource",
				sourceCard);
    }
  else if (sdefType=="Beam" || sdefType=="beam")
    {
      ELog::EM<<"SDEF == :: Beam"<<ELog::endDiag;
      if (FCPtr)
	SDef::createBeamSource(Control,"beamSource",
			       *FCPtr,linkIndex,sourceCard);
      else
	SDef::createBeamSource(Control,"beamSource",sourceCard);
    }
  else if (sdefType=="LENS" || sdefType=="lens")
    {
      const attachSystem::FixedComp* PC=
	OR.getObject<attachSystem::FixedComp>("ProtonBeam");
      if (!PC)
	throw ColErr::InContainerError<std::string>("ProtonBeam",
						  "Object container");
      
	SDef::createLensSource(Control,sourceCard,*PC);
    }
  else if (sdefType=="TS2")
    {
      // Basic TS2 source
      if(IParam.hasKey("horr") && IParam.flag("horr"))
	sourceCard.setTransform(System.createSourceTransform());
  
      // Basic TS2 source
      SDef::createTS2Source(Control,sourceCard);
    }
  else if (sdefType=="kcode")
    {
      ELog::EM<<"kcode sdef selected"<<ELog::endDiag;
    }
  else
    {
      ELog::EM<<"sdefType :\n"
	"Activation :: Activation source \n"
        "ActiveWeight :: Activation weighted source \n"
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
	"D4C :: D4C neutron beam"<<ELog::endBasic;
    }
	
  if (IParam.flag("sdefVoid"))
    sourceCard.deactivate();

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
      else
        {
          throw ColErr::InContainerError<std::string>
            (key,"Key not found for activation");
        }
    }
  
  SDef::ActivationSource AS;
  AS.setBox(APt,BPt);
  AS.setTimeSegment(timeSeg);
  AS.setNPoints(nVol);
  AS.setWeightPoint(weightPt,weightDist);
  AS.setScale(scale);
  AS.createSource(System,cellDir,OName);

  return;
}
  
void
activeWeight(Simulation& System,
             const mainSystem::inputParam& IParam)
  /*!
    Select all the info for activation output
    \param System :: Simuation to use
    \param IParam :: input parameters
   */
{
  ELog::RegMethod RegA("SourceSelector","activationSelection");

  //File for input/
  const std::string OName="test.source";
  //    IParam.getDefValue<std::string>("test.source","actFile",0,1);

  size_t index(0);
  const Geometry::Vec3D APt=
    IParam.getCntVec3D("actBox",0,index,"Start Point of box not defined");
  const Geometry::Vec3D BPt=
    IParam.getCntVec3D("actBox",0,index,"End Point of box not defined");

  // WEIGHTING:
  index=0;
  const Geometry::Vec3D CPoint=
    IParam.getCntVec3D("actBias",0,index,"Start Point of box not defined");
  const Geometry::Vec3D Axis=
    IParam.getCntVec3D("actBias",0,index,"Axis Line not defined");
  const double distW=IParam.getDefValue<double>(2.0,"actBias",0,index);
  const double angleW=IParam.getDefValue<double>(2.0,"actBias",0,index+1);

  // MATERIAL:
  std::vector<std::string> MatName;
  std::vector<std::string> MatFile;
  const size_t nP=IParam.setCnt("actMat");
  for(size_t index=0;index<nP;index++)
    {
      const size_t nItems=IParam.itemCnt("actMat",index);
      for(size_t j=0;j<nItems+1;j+=2)
	{
	  MatName.push_back
	    (IParam.getValueError<std::string>
	     ("actMat",index,j,"Material Name"));
	  MatFile.push_back
	    (IParam.getValueError<std::string>
	     ("actMat",index,j+1,"Material File"));
	}
    }


  SDef::ActiveWeight AS;
  AS.setBiasConst(CPoint,Axis,distW,angleW);
  AS.setBox(APt,BPt);

  for(size_t i=0;i<MatName.size();i++)
    AS.addMaterial(MatName[i],MatFile[i]);

  AS.setNPoints(System.getPC().getNPS());
  AS.createSource(System,OName);

  return;
}
  

  
} // NAMESPACE SDef
