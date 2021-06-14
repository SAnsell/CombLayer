/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   src/LowMat.cxx
 *
 * Copyright (c) 2004-2021 by Konstantin Batkov
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

#include <set>
#include <map>
#include <array>
#include <string>
#include <vector>
#include <cmath>
#include <memory>
#include <sstream>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <typeinfo>
#include <stdexcept>

#include "Exception.h"
#include "FileReport.h"
#include "OutputLog.h"
#include "MatrixBase.h"
#include "HeadRule.h"
#include "LowMat.h"


const std::string&
LowMat::getLowName(const size_t Z)
  /*!
    Return the low map name
   */
{
  const static std::string empty("");
  const static size_t lmSize(96);
  const static std::array<std::string,lmSize> lm =
    {"z=0",
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

  //  Z that do not have a low-neutron card
  const static std::set<size_t> excludeLM
    ( { 34,37,44,45,52,59,61,66,67,68,69,70,
	76,81,84,85,86,87,88,89,91,93});

  if (excludeLM.find(Z)!=excludeLM.end())
    {
      ELog::EM << "No low energy FLUKA material for Z="<<Z<<ELog::endCrit;
      return empty;
    }

  if (Z>=lmSize)
    throw ColErr::IndexError<size_t>
      (Z,lmSize,"No low energy FLUKA material for Z=" +
       std::to_string(Z) + ">" + std::to_string(lmSize));

  return lm[Z];
}

std::tuple<int,int,double>
LowMat::getID(const size_t Z,const size_t iso)
/*!
  Set numerical identifiers for the given Z.
  Note: There can be several sets for IDs for the given material in FLUKA,
  but here we use a single hard-coded set of IDs. 
  FLUKA assumes natural isotopic composition, but in MCNP we can 
  associate cross sections for individual isotopes, therefore in this method
  we do not distinguish between different isotopes and use 
  natural composition for all of them.

  \todo{implement an interface to choose the
    ID set for the given material (i.e. the mt card in MCNP slang). 
    This can be done based on the
    'mat' value in SimFLUKA::getLowMat (pass it through constructor)}
  \todo{Do something intellegent with iso [currently assumed natural]}

 */
{
  typedef std::map<size_t,std::tuple<int,int,double>> mapTYPE;

  static const mapTYPE mUnit({
      { 1,{1, -5, 296.0} },
      { 5,{5, -2, 296.0} },
      { 6,{6, -2, 296.0} },
      //      { 8,{8, 16, 296.0} },
      { 11,{11, 23, 296.0} },
      { 12,{12, -2, 296.0} },
      { 13,{13, 27, 296.0} },
      { 14,{14, -2, 296.0} },
      { 15,{15, 31, 296.0} },
      { 16,{16, -2, 296.0} },
      { 17,{17, -2, 296.0} },
      { 19,{19, -2, 296.0} },
      { 20,{20, -2, 296.0} },
      { 22,{22, -2, 296.0} },
      { 24,{24, -2, 296.0} },
      { 25,{25, 55, 296.0} },
      { 26,{26, -2, 296.0} },
      { 27,{27, 59, 296.0} },
      { 28,{28, -2, 296.0} },
      { 29,{29, -2, 296.0} },
      { 30,{30, -2, 296.0} },
      { 74,{74, -2, 296.0} },
      { 82,{82, -2, 296.0} }
    });

  mapTYPE::const_iterator mc=mUnit.find(Z);
  return (mc==mUnit.end()) ?
    std::tuple<int,int,double>(0,0,0.0) : mc->second;
}

std::string
LowMat::getFLUKA(const size_t Z,const size_t iso,
		 const std::string& matName) 
  /*!
    Return the WHAT/SDUM string: id1 id2 id3 - - name
  */
{
  const auto& id = getID(Z,iso);
  const std::string& lowName=getLowName(Z);
  
  std::ostringstream cx;
  if (std::get<0>(id))
    {
      cx<<"LOW-MAT "<<matName<<" "
	<<std::get<0>(id)<<" "
	<<std::get<1>(id)<<" "
	<<std::get<2>(id)
	<<" - - "<<lowName<<" ";
    }
  else
    {
      cx<<"LOW-MAT "<<matName<<" - - - - - "<<lowName<<" ";
    }
  
  return cx.str();
}
