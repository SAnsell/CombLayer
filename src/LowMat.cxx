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

LowMat::LowMat(const std::string& mat,size_t Z) :
  mat(mat), Z(Z)
{
  const static std::string empty(" - ");
  const static std::array<std::string, 96> lm = {"z=0",
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
      name = empty;
    }

  const size_t lmsize = lm.size();

  if (Z>=lmsize)
    throw ColErr::IndexError<size_t>
      (Z,lmsize,"No low energy FLUKA material for Z=" +
       std::to_string(Z) + ">" + std::to_string(lmsize));

  name = lm[Z];
}

std::array<int,3>
LowMat::getID(size_t Z) const
/*!
  Set numerical identifiers for the given Z.
  Note: There can be several sets for IDs for the given material in FLUKA,
  but here we use a single hard-coded set of IDs. FLUKA assumes natural isotopic composition,
  but in MCNP we can associate cross sections for individual isotopes, therefore in this method
  we do not distinguish between different isotopes and use natural composition for all of them.
  TODO: implement an interface to choose the
  ID set for the given material (i.e. the mt card in MCNP slang). This can be done based on the
  'mat' value in SimFLUKA::getLowMat (pass it through constructor)
 */
{
  switch (Z) {
  case 1:
    // other alternative possible e.g. with if (mat == "") {}
    return {1, -5, 296};
  case 6:
    return {6, -2, 296};
  case 8:
    return {8, 16, 296};
  case 11:
    return {11, 23, 296};
  case 12:
    return {12, -2, 296};
  case 13:
    return {13, 27, 296};
  case 14:
    return {14, -2, 296};
  case 15:
    return {15, 31, 296};
  case 16:
    return {16, -2, 296};
  case 19:
    return {19, -2, 296};
  case 20:
    return {20, -2, 296};
  case 22: // Ti
    return {22, -2, 296};
  case 24:
    return {24, -2, 296};
  case 25:
    return {25, 55, 296};
  case 26:
    return {26, -2, 296};
  case 27: // Co
    return {27, 59, 296};
  case 28:
    return {28, -2, 296};
  case 29: // Cu
    return {29, -2, 296};
  case 30: // Zn
    return {30, -2, 296};
  case 74: // W
    return {74, -2, 296};
  case 82:
    return {82, -2, 296};
  }

  return {0, 0, 0}; // default values for the WHAT[1-3] of LOW-MAT
}

std::string LowMat::get() const
/*!
  Return the WHAT/SDUM string: id1 id2 id3 - - name
 */
{
  const auto id = getID(Z);

  std::string strid(" ");
  std::for_each(id.begin(), id.end(),
		[&strid](const auto& i){
		  strid += std::to_string(i) + " ";
		});

  return "LOW-MAT "+mat+strid+" - - "+name+" ";
}
