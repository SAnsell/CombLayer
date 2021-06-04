/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   src/SimPHITS.cxx
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
#include <cmath>
#include <complex> 
#include <vector>
#include <list>
#include <map> 
#include <set>
#include <string>
#include <algorithm>
#include <functional>
#include <numeric>
#include <iterator>
#include <memory>
#include <array>
#include <boost/format.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "Zaid.h"
#include "MXcards.h"
#include "Material.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Transform.h"
#include "Surface.h"
#include "surfIndex.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "surfRegister.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedRotate.h"
#include "Importance.h"
#include "Object.h"
#include "weightManager.h"
#include "WForm.h"
#include "inputSupport.h"
#include "phitsWriteSupport.h"
#include "SourceBase.h"
#include "sourceDataBase.h"
#include "phitsPhysics.h"
#include "phitsTally.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "magnetUnit.h"
#include "particleConv.h"
#include "Simulation.h"
#include "SimPHITS.h"


SimPHITS::SimPHITS() :
  Simulation(),icntl(0),nps(10000),rndSeed(1234567871),
  PhysPtr(new phitsSystem::phitsPhysics())
  /*!
    Constructor
  */
{}


SimPHITS::SimPHITS(const SimPHITS& A) :
  Simulation(A),nps(A.nps),rndSeed(A.rndSeed),
  PTItem(A.PTItem),MagItem(A.MagItem),
  PhysPtr(new phitsSystem::phitsPhysics(*PhysPtr))
 /*! 
   Copy constructor
   \param A :: Simulation to copy
 */
{
  // need to replace tally point with clone or just have copy
  for(auto& [name, tPtr ] : PTItem)
    tPtr=tPtr->clone();
}

SimPHITS&
SimPHITS::operator=(const SimPHITS& A)
  /*!
    Assignment operator
    \param A :: SimPHITS to copy
    \return *this
   */
{
  if (this!=&A)
    {
      Simulation::operator=(A);
      nps=A.nps;
      rndSeed=A.rndSeed;
      PTItem=A.PTItem;
      MagItem=A.MagItem;
      *PhysPtr=*PhysPtr;
    }
  return *this;
}

void
SimPHITS::setICNTL(const std::string& ICName)
  /*!
    Given the icntl name find the correct icntl number
    \param ICName :: Name to convert to number
   */
{
  ELog::RegMethod RegA("SimPHITS","setICNTL");

  int icn(0);
  if (StrFunc::convert(ICName,icn))
    {
      icntl=icn;
      return;
    }
  else if (ICName=="plot")
    {
      icntl=7;
      return;
    }
    
  throw ColErr::InContainerError<std::string>(ICName,"ICName");
  return;
}

void
SimPHITS::addTally(const phitsSystem::phitsTally& TRef)
  /*!
    Adds a tally to the main PTItem list.
    \param TRef :: Tally item to insert
    \return 0 :: Successful
    \return -1 :: Tally already in use
  */
{
  ELog::RegMethod RegA("SimPHITS","addTally");
  
  const std::string keyNum=TRef.getKeyName();
  if (PTItem.find(keyNum)!=PTItem.end())
    throw ColErr::InContainerError<std::string>(keyNum,"Tally Present");

  phitsSystem::phitsTally* TX=TRef.clone();
  PTItem.emplace(keyNum,TX);
  return;
}

int
SimPHITS::getNextTallyID() const
  /*!
    Get a unique id for the tally
    \return ID number [unused]
  */
{
  std::set<int> IDSet;
  for(const auto& [Name , PPtr] : PTItem)
    IDSet.emplace(PPtr->getID());

  int index(1);
  while(IDSet.find(index)!=IDSet.end())
    {
      index++;
    }
  return index;
}

void
SimPHITS::writeSource(std::ostream& OX) const
  /*!
    Writes out the sources 
    construction.
    \param OX :: Output stream
  */
{
  ELog::RegMethod RegA("SimPHITS","writeSource");

  SDef::sourceDataBase& SDB=SDef::sourceDataBase::Instance();
  
  OX<<"[Source]"<<std::endl;

  const SDef::SourceBase* SBPtr=
    SDB.getSource<SDef::SourceBase>(sourceName);
  if (SBPtr)
    SBPtr->writePHITS(OX);
  return;
}

void
SimPHITS::writeTally(std::ostream& OX) const
  /*!
    Writes out the tallies 
    construction.
    \param OX :: Output stream
   */
{
  OX<<"$ -----------------------------------------------------------"<<std::endl;
  OX<<"$ ------------------- TALLY CARDS ---------------------------"<<std::endl;
  OX<<"$ -----------------------------------------------------------"<<std::endl;
  for(const PTallyTYPE::value_type& TI : PTItem)
    TI.second->write(OX,fileName);

  return;
}

void
SimPHITS::addMagnetObject
(const std::shared_ptr<magnetSystem::magnetUnit>& MUnit)
  /*!
    Add an object to this data base
    \param MUnit :: Magnetic unit
   */
{
  ELog::RegMethod RegA("SimPHITS","addMagnetObject");
  
  addObject(MUnit);
  MUnit->setIndex(MagItem.size());
  MagItem.emplace(MUnit->getKeyName(),MUnit);
  return;
}

std::map<int,int>
SimPHITS::renumberCells(const std::vector<int>& cOffset,
			const std::vector<int>& cRange)
  /*!
    Re-arrange all the cell numbers to be sequentual from 1-N.
    \param cOffset :: Protected start
    \param cRange :: Protected range
    \return map<oldNumber,newNumber>
  */
{
  ELog::RegMethod RegA("SimMCNP","renumberCells");

  const std::map<int,int> RMap=
    Simulation::renumberCells(cOffset,cRange);

  // CARE HERE: RMap is the old number. The objects themselve
  //  have already been updated
  for(const auto [cNum,nNum] : RMap)
    {
      //      PhysPtr->substituteCell(cNum,nNum);
      for(PTallyTYPE::value_type& TI : PTItem)
	TI.second->renumberCell(cNum,nNum);
    }
  return RMap;
}


void
SimPHITS::writeTransform(std::ostream& OX) const
  /*!
    Write all the transforms in standard MCNPX output 
    type [These should now not be used].
    \param OX :: Output stream
  */

{
  if (!TList.empty())
    {
      OX<<"[transform]"<<std::endl;
      TransTYPE::const_iterator vt;
      for(vt=TList.begin();vt!=TList.end();vt++)
	vt->second.write(OX);
    }
  return;
}


void
SimPHITS::writeCells(std::ostream& OX) const
  /*!
    Write all the cells in standard MCNPX output 
    type.
    \param OX :: Output stream
  */
{
  boost::format FmtStr("  %1$d%|20t|%2$d\n");
  OX<<"[cell]"<<std::endl;
  OTYPE::const_iterator mp;
  for(mp=OList.begin();mp!=OList.end();mp++)
    mp->second->writePHITS(OX);

  OX<<std::endl;  // Empty line manditory for MCNPX

  OX<<"[temperature]"<<std::endl;
  for(mp=OList.begin();mp!=OList.end();mp++)
    {
      const double T=mp->second->getTemp();
      if (std::abs(T-300.0)>1.0 && std::abs<double>(T)>1e-6)
	{
	  OX<<FmtStr % mp->second->getName() % (T*8.6173422e-5);
	}
    }
  OX<<std::endl;
  
  return;
}

void
SimPHITS::writeSurfaces(std::ostream& OX) const
  /*!
    Write all the surfaces in standard MCNPX output 
    type.
    \param OX :: Output stream
  */

{
  OX<<"[surface] " <<std::endl;

  const ModelSupport::surfIndex::STYPE& SurMap=
    ModelSupport::surfIndex::Instance().surMap();

  for(const std::map<int,Geometry::Surface*>
	::value_type& mp : SurMap)
    mp.second->write(OX);

  return;
} 

void
SimPHITS::writeMaterial(std::ostream& OX) const
  /*!
    Write all the used Materials in standard MCNPX output 
    type.
    \param OX :: Output stream
  */
{
  OX<<"[ Material ]"<<std::endl;

  // set ordered otherwize output random [which is annoying]
  const std::map<int,const MonteCarlo::Material*> orderedMat=
    getOrderedMaterial();

  for(const auto& [matID,matPtr] : orderedMat)
    {
      (void) matID; 
      matPtr->writePHITS(OX);
    }

  return;
}


void
SimPHITS::writeWeights(std::ostream& OX) const
  /*!
    Write all the used Weight in standard MCNPX output 
    type.
    \param OX :: Output stream
  */

{
  WeightSystem::weightManager& WM=
    WeightSystem::weightManager::Instance();

  ELog::EM<<"WRITE WEIGHT"<<ELog::endCrit;
  WM.writePHITS(OX);
  return;
}


void
SimPHITS::writePhysics(std::ostream& OX) const
  /*!
    Write all the used Weight in standard MCNPX output 
    type. Note that it also has to add the rdum cards
    to the physics
    \param OX :: Output stream
  */
{  
  ELog::RegMethod RegA("SimPHITS","writePhysics");
  // Processing for point tallies

  WeightSystem::weightManager& WM=
    WeightSystem::weightManager::Instance();

  OX<<"[Parameters]"<<std::endl;

  StrFunc::writePHITS(OX,1,"icntl",icntl);
  StrFunc::writePHITS(OX,1,"maxcas",nps/10);
  StrFunc::writePHITS(OX,1,"maxbch",10);
  StrFunc::writePHITS(OX,1,"file(1)","/home/ansell/phits");
  StrFunc::writePHITS(OX,1,"file(6)","phits.out");
  StrFunc::writePHITS(OX,1,"file(7)","/home/ansell/mcnpxNew/xsdir_short");
  StrFunc::writePHITS(OX,1,"rseed",rndSeed);
  
  if (!MagItem.empty())
    {
      StrFunc::writePHITS(OX,1,"imagnf",1,"Turn on magnetic fields");
      StrFunc::writePHITS(OX,1,"ielctf",0,"Turn on electromag fields");
    }
    
  PhysPtr->writePHITS(OX);
  
  if (WM.hasParticle("n"))
    {
      const WeightSystem::WForm* NWForm=WM.getParticle("n");
      NWForm->writePHITSHead(OX);
    }
  OX<<"$ ++++++++++++++++++++++ END ++++++++++++++++++++++++++++"<<std::endl;

  
  OX<<std::endl;
  return;
}

void
SimPHITS::writeMagnet(std::ostream& OX) const
  /*! 
    Write all the magnet units to standard phits
    Note that wes use type 103 to get to usrmgf3.f
    \param OX :: Output stream
  */
{
  ELog::RegMethod RegA("SimPHITS","writeMagnet");

  if (!MagItem.empty())
    {
      // assume at least one magnetic region:
      OX<<"[Magnetic Field]"<<std::endl;
      StrFunc::writePHITSTableHead
	(OX,1,{"reg","typ","gap","mgf","trcl"});

      for(const auto& [name , oPtr] : OList)
	{
	  if (oPtr->hasMagField())
	    // -5: type
	    // 10.0 :  1/Stepsize : unused
	    // 10.0 : t to kG [field is provided in tesla:unused]
	    // 0 : transform (trcl)
	    StrFunc::writePHITSTable(OX,1,name,-5,10.0,10.0,0); 
	}

      OX<<"[Magnetic Units]"<<std::endl;
      for(const auto& [name , mPtr] : MagItem)
	mPtr->writePHITS(OX);
    }
  return;
}

void
SimPHITS::writeImportance(std::ostream& OX) const
  /*!
    Write all the importances / voluems
    \param OX :: Output stream
  */
{
  ELog::RegMethod RegA("SimMCNP","writeImportance");

  const particleConv& pConv=particleConv::Instance();
  
  std::ostringstream cx;

  // make set of particle:
  std::set<int> pList;

  // PHITS uses integer importance
  // zero for all:
  std::map<int,std::map<int,double>> ImpMap;
  ImpMap.emplace(0,std::map<int,double>());
    
  bool flag(0);
  double Imp;
  for(const int CN : cellOutOrder)
    {
      const MonteCarlo::Object* OPtr=findObject(CN);
      // flag indicates particles :
      std::tie(flag,Imp)=OPtr->getImpPair();  // returns 0 as well
      ImpMap[0].emplace(CN,Imp);	    
      if (!flag)
	{
	  const std::set<int>& PSet=OPtr->getImportance().getParticles();
	  for(const int P : PSet)
	    {
	      const double ImpVal=OPtr->getImp(P);
	      std::map<int,std::map<int,double>>::iterator mc;
	      mc=ImpMap.find(P);
	      if (mc==ImpMap.end())
		{
		  pList.erase(P);
		  ImpMap.emplace(P,ImpMap[0]);   // copy existing list in
		}
	      ImpMap[P].emplace(CN,ImpVal);
	    }
	}
    }

  for(const auto& [particleNumber,allMap] : ImpMap)
    {
      if (!allMap.empty())   // Note this can be empty if all cells assigned.
	{
	  OX<<"[Importance]"<<std::endl;
	  if (particleNumber==0)
	    OX<<"  part = all"<<std::endl;
	  else
	    OX<<"  part = "<<pConv.mcplToPHITS(particleNumber)<<std::endl;
			      
	  StrFunc::writePHITSTableHead(OX,1,{"reg","imp"});
	  StrFunc::writePHITSCellSet(OX,2,allMap);
	}
    }
  OX<<std::endl;

  return;
}

void
SimPHITS::write(const std::string& Fname) const
  /*!
    Write out all the system (in PHITS output format)
    \param Fname :: Output file 
  */
{
  std::ofstream OX(Fname.c_str());
  OX<<"[Title]"<<std::endl;
  writePhysics(OX);
  //  Simulation::writeVariables(OX);
  OX<<std::endl;
  
  writeCells(OX);
  writeSurfaces(OX);
  writeMaterial(OX);
  writeWeights(OX);
  writeTransform(OX);
  writeTally(OX);
  writeSource(OX);
  writeImportance(OX);
  writeMagnet(OX);

  OX<<"[end]"<<std::endl; 
  OX.close();
  return;
}
