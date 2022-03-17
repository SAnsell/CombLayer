/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   src/SimFLUKA.cxx
 *
 * Copyright (c) 2004-2021 by Stuart Ansell / Konstantin Batkov
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

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "writeSupport.h"
#include "MXcards.h"
#include "Zaid.h"
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
#include "Importance.h"
#include "Object.h"
#include "weightManager.h"
#include "inputSupport.h"
#include "SourceBase.h"
#include "sourceDataBase.h"
#include "surfRegister.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedRotate.h"
#include "pairValueSet.h"
#include "cellValueSet.h"
#include "flukaTally.h"
#include "radDecay.h"
#include "flukaPhysics.h"
#include "magnetUnit.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "LowMat.h"

#include "Simulation.h"
#include "SimFLUKA.h"

SimFLUKA::SimFLUKA() :
  Simulation(),
  alignment("*...+.WHAT....+....1....+....2....+....3....+....4....+....5....+....6....+.SDUM"),
  defType("PRECISION"),basicGeom(0),geomPrecision(0.0001),
  writeVariable(1),lowEnergyNeutron(1),
  nps(1000),rndSeed(2374891),
  PhysPtr(new flukaSystem::flukaPhysics()),
  RadDecayPtr(new flukaSystem::radDecay())
  /*!
    Constructor
  */
{}

SimFLUKA::SimFLUKA(const SimFLUKA& A) :
  Simulation(A),
  alignment(A.alignment),defType(A.defType),
  basicGeom(A.basicGeom),geomPrecision(A.geomPrecision),
  writeVariable(A.writeVariable),
  lowEnergyNeutron(A.lowEnergyNeutron),
  nps(A.nps),rndSeed(A.rndSeed),
  sourceExtraName(A.sourceExtraName),
  PhysPtr(new flukaSystem::flukaPhysics(*A.PhysPtr)),
  RadDecayPtr(new flukaSystem::radDecay(*A.RadDecayPtr))
 /*!
   Copy constructor
   \param A :: Simulation to copy
 */
{}

SimFLUKA&
SimFLUKA::operator=(const SimFLUKA& A)
  /*!
    Assignment operator
    \param A :: SimFLUKA to copy
    \return *this
   */
{
  if (this!=&A)
    {
      Simulation::operator=(A);
      defType=A.defType;
      basicGeom=A.basicGeom;
      writeVariable=A.writeVariable;
      lowEnergyNeutron=A.lowEnergyNeutron;
      nps=A.nps;
      rndSeed=A.rndSeed;
      sourceExtraName=A.sourceExtraName;
      clearTally();
      for(const FTallyTYPE::value_type& TM : A.FTItem)
	FTItem.emplace(TM->clone());
      *PhysPtr= *A.PhysPtr;
      *RadDecayPtr = *A.RadDecayPtr;
    }
  return *this;
}

SimFLUKA::~SimFLUKA()
  /*!
    Destructor
  */
{
  clearTally();
  delete PhysPtr;
  delete RadDecayPtr;
}

void
SimFLUKA::setDefaultPhysics(const std::string& dName)
  /*!
    Set the default physics name if valid
    \param dName :: Name to set [lower/upper case]
   */
{
  // short name / full name
  const static std::map<std::string,std::string>
    validItem({{"PRECISIO","PRECISION"},
	       {"EM-CASCAD","EM-CASCADE"},
	       {"CALORIME","CALORIMETRY"},
	       {"SHIELDIN","SHIELDING"},
	       {"NEW-DEF","NEW-DEFAULTS"},
	       {"HADROTHE","HADRONTHE"},
	       {"NEUTRONS","NEUTRONS"}
      });

  if (dName.empty())
    {
      defType="";
      return;
    }

  const std::string item=StrFunc::toUpperString(dName.substr(0,8));
  std::map<std::string,std::string>::const_iterator mc=
    validItem.find(item);
  if (mc==validItem.end())
    throw ColErr::InContainerError<std::string>(dName,"Default item not known");
  defType=mc->second;
  return;
}


void
SimFLUKA::clearTally()
  /*!
    Remove all the tallies
  */
{
  for(flukaSystem::flukaTally* FPtr : FTItem)
    delete FPtr;
  FTItem.erase(FTItem.begin(),FTItem.end());
  return;
}

int
SimFLUKA::getNextFTape() const
 /*!
   Horrible function to find the next availabe ftape unit
   note that we should start from >25
   \return available number
 */
{
  ELog::RegMethod RegA("SimFLUKA","getNextFTape");

  // max for fortran output stream 98 ??
  int nextFTape(24);
  for(const flukaSystem::flukaTally* FPtr : FTItem)
    if (std::abs(FPtr->getOutUnit())>nextFTape)
      nextFTape=std::abs(FPtr->getOutUnit());

  nextFTape++;
  if (nextFTape>98)
    throw ColErr::InContainerError<int>
      (98,"Tallies have exhaused available ftapes [25-98]");
  return nextFTape;
}

void
SimFLUKA::addTally(const flukaSystem::flukaTally& TI)
  /*!
    Add a tally to main stack
    \param TI :: Fluka tally to add
  */
{
  ELog::RegMethod RegA("SimFluka","addTally");

  // Fluka cannot share output [and fOutput > 25 ]
  const int fID=std::abs(TI.getID());

  // is id already present
  if (std::find_if(FTItem.begin(),FTItem.end(),
		   [&fID](const flukaSystem::flukaTally* FPtr)
		   {
		     return FPtr->getID()==fID;
		   }) != FTItem.end())
    {
      throw ColErr::InContainerError<int>(fID,"fID for tally");
    }

  FTItem.emplace(TI.clone());
  return;
}

flukaSystem::flukaTally*
SimFLUKA::getTally(const int TI) const
  /*!
    Get a tally to main stack
    \param TI :: Fluka tally to return
    \return tally Pointer
  */
{
  ELog::RegMethod RegA("SimFluka","getTally");

  for(flukaSystem::flukaTally* FPtr : FTItem)
    if (FPtr->getID()==TI)
      return FPtr;

  throw ColErr::InContainerError<int>(TI,"Fluka Tally");
}

void
SimFLUKA::setExtraSourceName(const std::string& S)
 /*!
   Set the source name from the database
   \param S :: Source name
  */
{
  sourceExtraName=S;
  return;
}

void
SimFLUKA::addUserFlags(const std::string& Key,const std::string& Extra)
{
  // replace with insert_or_assign in C++17
  FlagItem[Key]=Extra;
  return;
}


void
SimFLUKA::writeTally(std::ostream& OX) const
  /*!
    Writes out the tallies using a nice boost binding
    construction.
    \param OX :: Output stream
   */
{
  ELog::RegMethod RegA("SimFluka","writeTally");

  OX<<"* ------------------------------------------------------"<<std::endl;
  OX<<"* ------------------- TALLY CARDS ----------------------"<<std::endl;
  OX<<"* ------------------------------------------------------"<<std::endl;

  for(const FTallyTYPE::value_type& TI : FTItem)
    TI->write(OX);

  return;
}

void
SimFLUKA::writeFlags(std::ostream& OX) const
  /*!
    Writes out the flags using a look-up stack.
    Bascially an ultra primative method to get something
    into the fluka file, if nothing else fits.

    This set user weight : Allows call to FLUSCW in the
    modified fortran code
    \param OX :: Output stream
  */
{
  ELog::RegMethod RegA("SimFluka","writeFlags");


  const static std::map<std::string,int> indexMap(
    {
      {"before",1},
      {"shiftBefore",2},
      {"beforeShift",2},
      {"after",3},
      {"afterShift",4},
      {"shiftAfter",4}
    });

  std::ostringstream cx;
  for(const FlagTYPE::value_type& fPair : FlagItem)
    {
      if (fPair.first=="userWeight")
	{
	  int index(0);
	  double VIndex;
	  if (!StrFunc::convert(fPair.second,VIndex))
	    {
	      std::map<std::string,int>::const_iterator mc=
		indexMap.find(fPair.second);
	      index=(mc!=indexMap.end()) ? mc->second : 0;
	    }
	  else
	    {
	      index=static_cast<int>(VIndex);
	    }

	  if (index==0)
	    ELog::EM<<"WARNING userWeight set to zero"<<ELog::endWarn;

	  cx<<"USERWEIG - - "<<index<<" - -";
	  StrFunc::writeFLUKA(cx.str(),OX);
	}
      else
	throw ColErr::InContainerError<std::string>
	  (fPair.first,"FlagItem key");
    }

  return;
}


void
SimFLUKA::writeMagField(std::ostream& OX) const
  /*!
    Writes out the tallies using a nice boost binding
    construction.
    \param OX :: Output stream
   */
{
  ELog::RegMethod RegA("SimFluka","writeMagField");

  OX<<"* ------------------------------------------------------"<<std::endl;
  OX<<"* ------------------- MAGNETIC CARDS ----------------------"<<std::endl;
  OX<<"* ------------------------------------------------------"<<std::endl;

  std::ostringstream cx;
  if (!MagItem.empty())
    {
      // Need to set mgnfield to zero
      cx<<"MGNFIELD 15.0 0.05 0.1 - - - ";
      StrFunc::writeFLUKA(cx.str(),OX);

      flukaSystem::cellValueSet<2> Steps("stepsize","STEPSIZE");
      for(const OTYPE::value_type& mp : OList)
	{
	  if (mp.second->hasMagField())
	    {
	      const std::pair<double,double> magStep=
		mp.second->getMagStep();
	      Steps.setValues(mp.second->getName(),magStep.first,magStep.second);
	    }
	}
      const std::string fmtSTR("%2 %3 R0 R1 1.0 - ");
      const std::vector<int> cellInfo=this->getCellVector();
      Steps.writeFLUKA(OX,cellInfo,fmtSTR);

      for(const MagTYPE::value_type& MI : MagItem)
	MI.second->writeFLUKA(OX);
    }
  return;
}

void
SimFLUKA::writeTransform(std::ostream& OX) const
  /*!
    Write all the transforms in standard MCNPX output
    type [These should now not be used].
    \param OX :: Output stream
  */

{
  OX<<"* [transform]"<<std::endl;

  TransTYPE::const_iterator vt;
  for(vt=TList.begin();vt!=TList.end();vt++)
    {
      vt->second.write(OX);
    }
  return;
}


void
SimFLUKA::writeCells(std::ostream& OX) const
  /*!
    Write all the cells in standard FLUKA output
    type.
    \param OX :: Output stream
  */
{
  ELog::RegMethod RegA("SimFLUKA","writeCells");
  OX<<"* CELL CARDS "<<std::endl;

  OTYPE::const_iterator mp;
  for(mp=OList.begin();mp!=OList.end();mp++)
    mp->second->writeFLUKA(OX);
  OX<<"END"<<std::endl;
  return;
}

void
SimFLUKA::writeSurfaces(std::ostream& OX) const
  /*!
    Write all the surfaces in standard MCNPX output
    type.
    \param OX :: Output stream
  */
{
  ELog::RegMethod RegA("SimFLUKA","writeSurfaces");

  OX<<"* SURFACE CARDS "<<std::endl;

  const ModelSupport::surfIndex::STYPE& SurMap =
    ModelSupport::surfIndex::Instance().surMap();

  for(const ModelSupport::surfIndex::STYPE::value_type& sm : SurMap)
    sm.second->writeFLUKA(OX);
  OX<<"END"<<std::endl;
  return;
}

void
SimFLUKA::writeElements(std::ostream& OX) const
  /*!
    Write all the used isotopes.
    To be used by material definitions in the COMPOUND cards.
    \param OX :: Output stream
  */
{
  ELog::RegMethod RegA("SimFLUKA","writeElements");

  OX<<"* ELEMENTS "<<std::endl;
  OX<<alignment<<std::endl;

  std::set<MonteCarlo::Zaid> setZA;

  for(const auto& [ cellNum,objPtr]  : OList)  //
    {
      (void) cellNum;        // avoid warning -- fixed c++20
      const MonteCarlo::Material* mPtr = objPtr->getMatPtr();
      const std::vector<MonteCarlo::Zaid>& zaidVec = mPtr->getZaidVec();
      for (const MonteCarlo::Zaid& ZC : zaidVec)
	setZA.insert(ZC);
    }

  std::ostringstream cx,lowmat;
  for (const MonteCarlo::Zaid& za : setZA)
    {
      if (!za.getZaid().empty())
	{
	  cx<<"MATERIAL "<<za.getZ()<<". - "<<" 1."
	    <<" - - "<<za.getIso()<<". "<<za.getFlukaName()<<" ";
	  lowmat<<LowMat::getFLUKA(za.getZ(),za.getIso(),za.getFlukaName());
	}
    }
  
  StrFunc::writeFLUKA(cx.str(),OX);
  if (lowEnergyNeutron)
    StrFunc::writeFLUKA(lowmat.str(),OX);
  
  OX<<alignment<<std::endl;

  return;
}

void
SimFLUKA::writeMaterial(std::ostream& OX) const
  /*!
    Write all the used Materials in fixed FLUKA format
    \param OX :: Output stream
  */
{
  ELog::RegMethod RegA("SimFLUKA","writeMaterial");

  OX<<"* MATERIAL CARDS "<<std::endl;
  OX<<alignment<<std::endl;
  // WRITE OUT ASSIGNMENT:
  bool magField(0);
  for(const OTYPE::value_type& mp : OList)
    {
      mp.second->writeFLUKAmat(OX);
      if (mp.second->hasMagField())
	magField=1;
    }

  writeElements(OX);

  // set ordered otherwize output random [which is annoying]
  const std::map<int,const MonteCarlo::Material*> orderedMat=
    getOrderedMaterial();

  for(const auto& [matID,matPtr] : orderedMat)
    matPtr->writeFLUKA(OX);

  OX<<alignment<<std::endl;

  if (magField)
    writeMagField(OX);
  return;
}

void
SimFLUKA::writeWeights(std::ostream& OX) const
  /*!
    Write all the used Weight in standard FLUKA output
    type.
    \param OX :: Output stream
  */
{
  ELog::RegMethod RegA("SimFLUKA","writeWeights");

  WeightSystem::weightManager& WM=
    WeightSystem::weightManager::Instance();

  OX<<"* WEIGHT CARDS "<<std::endl;

  WM.writeFLUKA(OX);
  return;
}

void
SimFLUKA::prepareImportance()
  /*!
    This needs to set a BIAS card for the appropaiate particles
  */
{
  ELog::RegMethod RegA("SimFLUKA","prepareImportance");


  const double minFlukaImportance(1e-5);
  bool flag;
  double Imp;
  std::vector<std::pair<int,double>> ImpVec;
  ImpVec.push_back(std::pair<int,double>(1,0.0));

  for(const int CN : cellOutOrder)
    {
      const MonteCarlo::Object* OPtr=findObject(CN);
      // flag indicates particles :
      std::tie(flag,Imp)=OPtr->getImpPair();  // returns 0 as well
      ImpVec.push_back(std::pair<int,double>(CN,Imp));
    }

  for(const auto& [CN,V] : ImpVec)
    {
      if (std::abs(V-1.0)>Geometry::zeroTol &&
	  std::abs(V) > minFlukaImportance)         // min for FLUKA

	PhysPtr->setTHR("bias",CN,"3.0",std::to_string(V),"3.0");
    }
  return;
}

void
SimFLUKA::writePhysics(std::ostream& OX) const
  /*!
    Write all the used Weight in standard MCNPX output
    type. Note that it also has to add the rdum cards
    to the physics
    \param OX :: Output stream
  */

{
  ELog::RegMethod RegA("SimFLUKA","writePhysics");
  std::ostringstream cx;

  writeFlags(OX);
  cx<<"START "<<static_cast<double>(nps);
  StrFunc::writeFLUKA(cx.str(),OX);
  cx.str("");
  cx<<"RANDOMIZE 1.0 "<<std::to_string(rndSeed % 1000000);
  StrFunc::writeFLUKA(cx.str(),OX);
  // Remaining Physics cards
  PhysPtr->writeFLUKA(OX);

  return;
}


void
SimFLUKA::writeSource(std::ostream& OX) const
  /*!
    Write the source into standard FLUKA format.
    Most of these sources are limited.
    \param OX :: Output stream
  */
{
  ELog::RegMethod RegA("SimFLUKA","writeSource");

  SDef::sourceDataBase& SDB=
    SDef::sourceDataBase::Instance();

  OX<<"* -------------------------------------------------------"<<std::endl;
  OX<<"* --------------- SOURCE CARDS --------------------------"<<std::endl;
  OX<<"* -------------------------------------------------------"<<std::endl;

  if (!sourceName.empty())
    {
      SDef::SourceBase* SPtr=
	SDB.getSourceThrow<SDef::SourceBase>(sourceName,"Source not known");
      SPtr->writeFLUKA(OX);
    }
  if (!sourceExtraName.empty())
    {
      SDef::SourceBase* SPtr=
	SDB.getSourceThrow<SDef::SourceBase>(sourceExtraName,
					     "SourceExtra not known");
      SPtr->writeFLUKA(OX);
    }

  OX<<"* ++++++++++++++++++++++ END ++++++++++++++++++++++++++++"<<std::endl;
  return;
}

void
SimFLUKA::addMagnetObject
(const std::shared_ptr<magnetSystem::magnetUnit>& MUnit)
  /*!
    Add an object to this data base
    \param MUnit :: Magnetic unit
   */
{
  ELog::RegMethod RegA("SimFLUKA","addMagnetObject");
  addObject(MUnit);
  MUnit->setIndex(MagItem.size());
  MagItem.emplace(MUnit->getKeyName(),MUnit);
  return;
}

void
SimFLUKA::prepareWrite()
  /*!
    Stuff that should be done once before output
   */
{
  ELog::RegMethod RegA("SimFLUKA","prepareWrite");
  Simulation::prepareWrite();

  PhysPtr->setCellNumbers(cellOutOrder);
  std::set<int> matActive=getActiveMaterial();
  matActive.erase(0);
  PhysPtr->setMatNumbers(matActive);
  prepareImportance();

  return;
}

void
SimFLUKA::write(const std::string& Fname) const
  /*!
    Write out all the system (in FLUKA output format)
    \param Fname :: Output file
  */
{
  ELog::RegMethod RegA("SimFLUKA","write");


  std::ofstream OX(Fname.c_str());
  const size_t nCells(OList.size());
  const size_t maxCells(20000);
  if (nCells>maxCells)
    {
      ELog::EM<<"Number of regions in geometry exceeds FLUKA max: "<<nCells
	      <<" > "<<maxCells<<ELog::endCrit;
      ELog::EM<<"See the GLOBAL card documentation"<<ELog::endCrit;
    }

  // cell/names(not numbers)/free geometry
  StrFunc::writeFLUKA("GLOBAL "+std::to_string(nCells)+" - 1.0 1.0",OX);
  OX<<"TITLE"<<std::endl;
  OX<<" Fluka model from CombLayer http://github.com/SAnsell/CombLayer"
    <<std::endl;

  StrFunc::writeMCNPXcomment("RunCmd:",OX,"* ");
  const std::vector<std::string> SCL=
    StrFunc::splitComandLine(cmdLine);
  for(const std::string& cmd : SCL)
    StrFunc::writeMCNPXcomment(cmd,OX,"* ");
  StrFunc::writeMCNPXcomment("",OX,"* ");

  if (writeVariable)
    Simulation::writeVariables(OX,'*');


  StrFunc::writeFLUKA("DEFAULTS - - - - - - PRECISION",OX);
  std::ostringstream cx;
  cx<<"GEOBEGIN";
  cx<<((basicGeom) ? " - " : " 1.0 ");
  cx<<geomPrecision<<" - - - - COMBNAME";
  StrFunc::writeFLUKA(cx.str(),OX);

  OX<<"  0 0 FLUKA Geometry from CombLayer"<<std::endl;
  writeSurfaces(OX);
  writeCells(OX);
  OX<<"GEOEND"<<std::endl;
  writeWeights(OX);
  writeMaterial(OX);
  RadDecayPtr->write(*this,OX);
  writeTally(OX);
  writeSource(OX);
  writePhysics(OX);
  OX<<"STOP"<<std::endl;
  OX.close();
  return;
}
