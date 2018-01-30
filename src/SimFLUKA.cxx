/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   src/SimFLUKA.cxx
 *
 * Copyright (c) 2004-2017 by Stuart Ansell / Konstantin Batkov
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
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "mathSupport.h"
#include "support.h"
#include "version.h"
#include "Element.h"
#include "MapSupport.h"
#include "MXcards.h"
#include "Element.h"
#include "Zaid.h"
#include "Material.h"
#include "DBMaterial.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Triple.h"
#include "NList.h"
#include "NRange.h"
#include "Tally.h"
#include "cellFluxTally.h"
#include "pointTally.h"
#include "heatTally.h"
#include "tallyFactory.h"
#include "Transform.h"
#include "Surface.h"
#include "surfIndex.h"
#include "Quadratic.h"
#include "Plane.h"
#include "ArbPoly.h"
#include "Cylinder.h"
#include "Cone.h"
#include "MBrect.h"
#include "NullSurface.h"
#include "Sphere.h"
#include "Torus.h"
#include "General.h"
#include "surfaceFactory.h"
#include "surfProg.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FItem.h"
#include "FuncDataBase.h"
#include "SurInter.h"
#include "Debug.h"
#include "BnId.h"
#include "Acomp.h"
#include "Algebra.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "weightManager.h"
#include "ModeCard.h"
#include "LSwitchCard.h"
#include "PhysCard.h"
#include "PhysImp.h"
#include "PhysicsCards.h"
#include "Simulation.h"
#include "SimFLUKA.h"

SimFLUKA::SimFLUKA() :
  Simulation(),
  alignment("*...+.WHAT....+....1....+....2....+....3....+....4....+....5....+....6....+.SDUM")
  /*!
    Constructor
  */
{}


SimFLUKA::SimFLUKA(const SimFLUKA& A) :
  Simulation(A),
  alignment(A.alignment)
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
    }
  return *this;
}


void
SimFLUKA::writeTally(std::ostream& OX) const
  /*!
    Writes out the tallies using a nice boost binding
    construction.
    \param OX :: Output stream
   */
{
  OX<<"c -----------------------------------------------------------"<<std::endl;
  OX<<"c ------------------- TALLY CARDS ---------------------------"<<std::endl;
  OX<<"c -----------------------------------------------------------"<<std::endl;
  // The totally insane line below does the following
  // It iterats over the Titems and since they are a map
  // uses the mathSupport:::PSecond
  // _1 refers back to the TItem pair<int,tally*>
  for(const TallyTYPE::value_type& TI : TItem)
    TI.second->write(OX);

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
  OX<<"[transform]"<<std::endl;

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
 */
{
  ELog::RegMethod RegA("SimFLUKA","writeElements");

  OX<<"* ELEMENTS "<<std::endl;
  OX<<alignment<<std::endl;
  ModelSupport::DBMaterial& DB=ModelSupport::DBMaterial::Instance();

  std::set<MonteCarlo::Zaid> setZA;
  boost::format FMTstr("%1$d.%2$02d%3$c");

  for(const OTYPE::value_type& mp : OList)
    {
      MonteCarlo::Material m = DB.getMaterial(mp.second->getMat());
      std::vector<MonteCarlo::Zaid> zaidVec = m.getZaidVec();
      for (const MonteCarlo::Zaid& ZC : zaidVec)
	{
	  setZA.insert(ZC);
	}
    }

  std::ostringstream cx,lowmat;
  MonteCarlo::Zaid ZC;
  for (const MonteCarlo::Zaid &za : setZA)
    {
      if (za.getZaid().empty()) continue;
      cx<<"MATERIAL "<<za.getZ()<<". - "<<" 1."<<" - - "<<za.getIso()<<". "<<za.getFlukaName()<<" ";
      lowmat<<getLowMat(za.getZ(),za.getIso(),za.getFlukaName());
    }

  StrFunc::writeFLUKA(cx.str(),OX);
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
  for(const OTYPE::value_type& mp : OList)
    mp.second->writeFLUKAmat(OX);
    
  ModelSupport::DBMaterial& DB=ModelSupport::DBMaterial::Instance();  
  DB.resetActive();

  OTYPE::const_iterator mp;
  for(mp=OList.begin();mp!=OList.end();mp++)
    DB.setActive(mp->second->getMat());

  writeElements(OX);

  DB.writeFLUKA(OX);
  
  OX<<alignment<<std::endl;
  return;
}
  


void
SimFLUKA::writeWeights(std::ostream& OX) const
  /*!
    Write all the used Weight in standard MCNPX output 
    type.
    \param OX :: Output stream
  */

{
  WeightSystem::weightManager& WM=
    WeightSystem::weightManager::Instance();
  
  OX<<"[weight]"<<std::endl;
  WM.write(OX);
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
  // Processing for point tallies
  std::map<int,tallySystem::Tally*>::const_iterator mc;
  std::vector<int> Idum;
  std::vector<Geometry::Vec3D> Rdum;
  for(mc=TItem.begin();mc!=TItem.end();mc++)
    {
      const tallySystem::pointTally* Ptr=
	dynamic_cast<const tallySystem::pointTally*>(mc->second);
      if(Ptr && Ptr->hasRdum())
        {
	  Idum.push_back(Ptr->getKey());
	  for(size_t i=0;i<4;i++)
	    Rdum.push_back(Ptr->getWindowPt(i));
	}
    }
  if (!Idum.empty())
    {
      OX<<"idum "<<Idum.size()<<" ";
      copy(Idum.begin(),Idum.end(),std::ostream_iterator<int>(OX," "));
      OX<<std::endl;
      OX<<"rdum       "<<Rdum.front()<<std::endl;
      for(const Geometry::Vec3D& rN : Rdum)
	OX<<"           "<<rN<<std::endl;	
    }

  // Remaining Physics cards
  PhysPtr->writeFLUKA(OX);
  return;
}

std::string
SimFLUKA::getLowMatName(const size_t& Z) const
/*!
  Return low energy FLUKA material name for the given Z
  \param Z :: Atomic number
  \todo : Currently this function return the standard low material name
    as if standard FLUKA names were used without the LOW-MAT card.
    This is fine for most of the cases.
    However, this name actually sometimes depends on temperature and mass
    number - to be implemented.
 */
{
  ELog::RegMethod RegA("SimFLUKA","getLowMatName");

  std::vector<std::string> lm = {"z=0",
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

  if((Z==34)||(Z==37)||(Z==44)||(Z==45)||(Z==52)||(Z==59)||(Z==61)||
     ((Z>=66)&&(Z<=70))||(Z==76)||(Z==81)||((Z>=84)&&(Z<=89))||(Z==91)||
     (Z==93))
    {
      ELog::EM << "No low energy FLUKA material for Z="<<Z<<ELog::endCrit;
    }

  if (Z>lm.size())
    {
      ELog::EM << "No low energy FLUKA material for Z="<<Z
	       <<"\nZ is too large"<<ELog::endErr;
    }

  std::string name(lm[Z]);

  if (name.empty())
    {
      ELog::EM << "FLUKA name for material with Z="<<Z<<" undefined" << ELog::endCrit;
      name = " - ";
    }

  return name;
}

std::string
SimFLUKA::getLowMat(const size_t& Z,const size_t& A,const std::string& mat) const
/*!
  Return the LOW-MAT card definition for the given Element
  \param Z :: Atomic number
  \param A :: Mass number
  \param mat :: Material name in the MATERIAL card
 */
{
  ELog::RegMethod RegA("SimFLUKA","getLowMat");
  std::string str;

  str = "LOW-MAT "+mat+" - - - - - "+getLowMatName(Z)+" ";

  return str;
}

void
SimFLUKA::write(const std::string& Fname) const
  /*!
    Write out all the system (in PHITS output format)
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

  StrFunc::writeFLUKA("GLOBAL "+std::to_string(nCells),OX);
  OX<<"TITLE"<<std::endl;
  OX<<" Fluka model from CombLayer http://github.com/SAnsell/CombLayer"<<std::endl;
  Simulation::writeVariables(OX,'*');
  StrFunc::writeFLUKA("DEFAULTS - - - - - - PRECISION",OX);
  StrFunc::writeFLUKA("BEAM -2.0 - - 14.0 3.2 1.0 PROTON",OX);
  StrFunc::writeFLUKA("BEAMPOS 0.0 -30.0 0.0 0.0 1.0 0.0",OX);

  StrFunc::writeFLUKA("GEOBEGIN - - - - - - COMBNAME",OX);
  OX<<"  0 0 FLUKA Geometry from CombLayer"<<std::endl;
  writeSurfaces(OX);
  writeCells(OX);
  OX<<"GEOEND"<<std::endl;
  writeMaterial(OX);
  writePhysics(OX);
  OX<<"STOP"<<std::endl;
  OX.close();
  return;
}
