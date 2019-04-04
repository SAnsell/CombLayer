/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   src/SimFLUKA.cxx
 *
 * Copyright (c) 2004-2019 by Stuart Ansell / Konstantin Batkov
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
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "mathSupport.h"
#include "support.h"
#include "writeSupport.h"
#include "version.h"
#include "Element.h"
#include "MXcards.h"
#include "Element.h"
#include "Zaid.h"
#include "Material.h"
#include "DBMaterial.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Transform.h"
#include "Surface.h"
#include "surfIndex.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "weightManager.h"
#include "Source.h"
#include "inputSupport.h"
#include "SourceBase.h"
#include "sourceDataBase.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "HeadRule.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "FixedRotate.h"
#include "pairValueSet.h"
#include "cellValueSet.h"
#include "flukaTally.h"
#include "radDecay.h"
#include "flukaProcess.h"
#include "flukaPhysics.h"
#include "magnetUnit.h"
#include "groupRange.h"
#include "objectGroups.h"

#include "Simulation.h"
#include "SimFLUKA.h"

SimFLUKA::SimFLUKA() :
  Simulation(),
  alignment("*...+.WHAT....+....1....+....2....+....3....+....4....+....5....+....6....+.SDUM"),
  defType("PRECISION"),writeVariable(1),
  lowEnergyNeutron(1),
  nps(1000),rndSeed(2374891),BVec(0,0,0),
  PhysPtr(new flukaSystem::flukaPhysics()),
  RadDecayPtr(new flukaSystem::radDecay())
  /*!
    Constructor
  */
{}

SimFLUKA::SimFLUKA(const SimFLUKA& A) :
  Simulation(A),
  alignment(A.alignment),defType(A.defType),
  writeVariable(A.writeVariable),
  lowEnergyNeutron(A.lowEnergyNeutron),
  nps(A.nps),rndSeed(A.rndSeed),BVec(A.BVec),
  sourceExtraName(A.sourceExtraName),
  PhysPtr(new flukaSystem::flukaPhysics(*PhysPtr)),
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
      writeVariable=A.writeVariable;
      lowEnergyNeutron=A.lowEnergyNeutron;
      nps=A.nps;
      rndSeed=A.rndSeed;
      BVec=A.BVec;
      sourceExtraName=A.sourceExtraName;
      clearTally();
      for(const FTallyTYPE::value_type& TM : A.FTItem)
	FTItem.emplace(TM.first,TM.second->clone());
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
  for(FTallyTYPE::value_type& mc : FTItem)
    delete mc.second;
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
  for(int i=25;i<98;i++)
    {
      if (FTItem.find(i)==FTItem.end())
	return i;
    }
  throw ColErr::InContainerError<int>
    (98,"Tallies have exhaused available ftapes [25-98]");
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
  const int fOutput=std::abs(TI.getOutUnit());
  if (FTItem.find(fOutput)!=FTItem.end())
    throw ColErr::InContainerError<int>(fOutput,"Foutput for tally");
  FTItem.emplace(fOutput,TI.clone());
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

  FTallyTYPE::const_iterator mc=FTItem.find(TI);
  if (mc==FTItem.end())
    throw ColErr::InContainerError<int>(TI,"Fluka Tally");
  return mc->second;
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
SimFLUKA::processActiveMaterials() const
  /*!
    Set materials as active in DBMaterail Database
  */
{
  ELog::RegMethod RegA("SimFLUKA","processActiveMaterials");

  ModelSupport::DBMaterial& DB=ModelSupport::DBMaterial::Instance();  
  DB.resetActive();

  for(const OTYPE::value_type& mc : OList)
    DB.setActive(mc.second->getMat());
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
  ELog::RegMethod RegA("SimFluka","addTally");
  
  OX<<"* ------------------------------------------------------"<<std::endl;
  OX<<"* ------------------- TALLY CARDS ----------------------"<<std::endl;
  OX<<"* ------------------------------------------------------"<<std::endl;

  for(const FTallyTYPE::value_type& TI : FTItem)
    TI.second->write(OX);

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
  // Simple case - flat field
  if (MagItem.empty())
    {
      if (BVec.abs()<Geometry::zeroTol)
	cx<<"MGNFIELD 15.0 0.05 0.1 - - - ";
      else
	cx<<"MGNFIELD 15.0 0.05 0.1 "<<BVec;
      
      StrFunc::writeFLUKA(cx.str(),OX);
      return;
    }

  // Need to set mgnfield to zero
  cx<<"MGNFIELD 15.0 0.05 0.1 - - - ";
  StrFunc::writeFLUKA(cx.str(),OX);
  for(const MagTYPE::value_type& MI : MagItem)
    MI.second->writeFLUKA(OX);

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
  ModelSupport::DBMaterial& DB=ModelSupport::DBMaterial::Instance();

  std::set<MonteCarlo::Zaid> setZA;

  for(const OTYPE::value_type& mp : OList)
    {
      const MonteCarlo::Material& m = DB.getMaterial(mp.second->getMat());
      const std::vector<MonteCarlo::Zaid>& zaidVec = m.getZaidVec();
      for (const MonteCarlo::Zaid& ZC : zaidVec)
	{
	  setZA.insert(ZC);
	}
    }

  std::ostringstream cx,lowmat;
  for (const MonteCarlo::Zaid& za : setZA)
    {
      if (za.getZaid().empty()) continue;
      cx<<"MATERIAL "<<za.getZ()<<". - "<<" 1."
	<<" - - "<<za.getIso()<<". "<<za.getFlukaName()<<" ";
      lowmat<<getLowMat(za.getZ(),za.getIso(),za.getFlukaName());
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
    
  ModelSupport::DBMaterial& DB=ModelSupport::DBMaterial::Instance();  
  DB.resetActive();

  OTYPE::const_iterator mp;
  for(mp=OList.begin();mp!=OList.end();mp++)
    DB.setActive(mp->second->getMat());

  writeElements(OX);

  DB.writeFLUKA(OX);

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


const std::string&
SimFLUKA::getLowMatName(const size_t Z) const
/*!
  Return low energy FLUKA material name for the given Z
  \param Z :: Atomic number
  \return fluka name
  \todo : Currently this function return the standard low material name
    as if standard FLUKA names were used without the LOW-MAT card.
    This is fine for most of the cases.
    However, this name actually sometimes depends on temperature and mass
    number - to be implemented.
 */
{
  ELog::RegMethod RegA("SimFLUKA","getLowMatName");

  const static std::string empty(" - ");
  const static std::vector<std::string> lm = {"z=0",
    "HYDROGEN", "HELIUM",   "LITHIUM",  "BERYLLIU", "BORON",
    "CARBON",   "NITROGEN", "OXYGEN",   "FLUORINE", "NEON",
    "SODIUM",   "MAGNESIU", "ALUMINUM", "SILICON",  "PHOSPHO",
    "SULFUR",   "CHLORINE", "ARGON",    "POTASSIU", "CALCIUM",
    "SCANDIUM", "TITANIUM", "VANADIUM", "CHROMIUM", "MANGANES",
    "IRON",     "COBALT",   "NICKEL",   "COPPER",   "ZINC",
    "GALLIUM",  "GERMANIU", "ARSENIC",  "",         "BROMINE",
    "KRYPTON",  "",         "STRONTIU", "YTTRIUM",  "ZIRCONIU",
    "NIOBIUM",  "MOLYBDEN", "99-TC",    "",         "",
    "PALLADIU", "SILVER",   "CADMIUM",  "INDIUM",   "TIN",     // 50
    "ANTIMONY", "",         "IODINE",   "XENON",    "CESIUM",
    "BARIUM",   "LANTHANU", "CERIUM",   "",         "NEODYMIU",
    "",         "SAMARIUM", "EUROPIUM", "GADOLINI", "TERBIUM", // 65
    "",         "",         "",         "",         "",
    "LUTETIUM", "HAFNIUM",  "TANTALUM", "TUNGSTEN", "RHENIUM",
    "",         "IRIDIUM",  "PLATINUM", "GOLD",     "MERCURY", // 80
    "",         "LEAD",     "BISMUTH",  "",         "",
    "",         "",         "",         "",         "230-TH", // 90
    "",         "233-U",    "",         "239-PU",   "241-AM"
  };

  const static std::set<size_t> excludeLM
    ( { 34,37,44,45,52,59,61,66,67,68,69,70,
	76,81,84,85,86,87,88,89,91,93});

  if (excludeLM.find(Z)!=excludeLM.end())
    {
      ELog::EM << "No low energy FLUKA material for Z="<<Z<<ELog::endCrit;
      return empty;
    }

  if (Z>=lm.size())
    throw ColErr::IndexError<size_t>
      (Z,lm.size(),"No low energy FLUKA material for Z>lm.size");

  return lm[Z];
}

std::string
SimFLUKA::getLowMat(const size_t Z,const size_t A,
		    const std::string& mat) const
  /*!
    Return the LOW-MAT card definition for the given Element
    \param Z :: Atomic number
    \param A :: Mass number
    \param mat :: Material name in the MATERIAL card
    \return fluka ouput card [pre-write format]
  */
{
  ELog::RegMethod RegA("SimFLUKA","getLowMat");

  return std::string("LOW-MAT "+mat+" - - - - - "+getLowMatName(Z)+" ");  
}

void
SimFLUKA::addMagnetObject
(const std::shared_ptr<flukaSystem::magnetUnit>& MUnit)
  /*!
    Add an object to this data base
    \param MUnit :: Magnetic unit
   */
{
  ELog::RegMethod RegA("SimFLUKA","addMagnetObject");
  addObject(MUnit);
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
  const ModelSupport::DBMaterial& DB=
    ModelSupport::DBMaterial::Instance();  
  Simulation::prepareWrite();

  PhysPtr->setCellNumbers(cellOutOrder);
  std::set<int> matActive=DB.getActive();
  matActive.erase(0);
  PhysPtr->setMatNumbers(matActive);

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
  StrFunc::writeFLUKA("GEOBEGIN - - - - - - COMBNAME",OX);
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
