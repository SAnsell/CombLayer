/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   src/LowMat.cxx
 *
 * Copyright (c) 2004-2024 by Konstantin Batkov
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
#include "LowMat.h"


std::tuple<int,int,double,std::string>
LowMat::getIDcern(const size_t Z,const size_t iso)
/*!
  Set numerical identifiers for the given Z for cern : set getID for
  system description.
  \param Z :: atomic number
  \param iso :: mass number
  \return lowMat Z,A,name
 */
{
  // List of unique items to INFN data base
  typedef std::map<size_t,std::tuple<int,int,double,std::string>> mapTYPE;

  static const mapTYPE mUnit
	({
	 {    8, {8, 16, 296.0, "OXYGEN"} }
	});

  const size_t zaid=Z*1000+iso;
  mapTYPE::const_iterator mc=mUnit.find(zaid);
  if (mc==mUnit.end()) mc=mUnit.find(Z);

  return (mc==mUnit.end()) ? getID(Z,iso) : mc->second;
}


std::tuple<int,int,double,std::string>
LowMat::getIDinfn(const size_t Z,const size_t iso)
/*!
  Set numerical identifiers for the given Z for INFN : set getID for
  system description.
  \param Z :: atomic number
  \param iso :: mass number
  \return lowMat Z,A,name
 */
{
  // List of unique items to INFN data base
  typedef std::map<size_t,std::tuple<int,int,double,std::string>> mapTYPE;

  static const mapTYPE mUnit
	({
   	 { 8016, {8, 16, 296.0, "OXYGE-16"} },
	 {    8, {8, -2, 296.0, "OXYGEN"} },
  	 {14028, {14, 28, 296.0,"SILIC-28"} },
	 {26056, {26, 56, 296.0,"56-FE"} },
	});

  const size_t zaid=Z*1000+iso;
  mapTYPE::const_iterator mc=mUnit.find(zaid);
  if (mc==mUnit.end()) mc=mUnit.find(Z);

  return (mc==mUnit.end()) ? getID(Z,iso) : mc->second;
}

std::tuple<int,int,double,std::string>
LowMat::getID(const size_t Z,const size_t iso)
/*!
  Set numerical identifiers for the given Z.
  Note: There can be several sets for IDs for the given material in FLUKA,
  but here we use a single hard-coded set of IDs.
  FLUKA assumes natural isotopic composition, but in MCNP we can
  associate cross sections for individual isotopes, therefore in this method
  we do not distinguish between different isotopes and use
  natural composition for all of them.

  \param Z :: atomic number
  \param iso :: mass number

  \todo{implement an interface to choose the
    ID set for the given material (i.e. the mt card in MCNP slang).
    This can be done based on the
    'mat' value in SimFLUKA::getLowMat (pass it through constructor)}
  \todo{Do something intellegent with iso [currently assumed natural]}

 */
{
  typedef std::map<size_t,std::tuple<int,int,double,std::string>> mapTYPE;

  static const mapTYPE mUnit
	({
	 { 1002, {1,  2, 296.0, "DEUTERIU"} },
	 { 1003, {1,  4, 296.0, "TRITIUM"} },
	 {  1,   {1, -5, 296.0, "HYDROGEN"} },
	 { 2003, {2,  3, 296.0, "HELIUM-3"} },
	 { 2004, {2,  3, 296.0, "HELIUM-4"} },
	 {    2, {2, -2, 296.0, "HELIUM"} },
	 {    3, {3, -2, 296.0, "LITHIUM"} },
	 {    4, {4,  9, 296.0, "BERYLLIU"} },
	 { 5010, {5, 10, 296.0, "BORON-10"} },
	 { 5011, {5, 11, 296.0, "BORON-11"} },
	 {    5, {5, -2, 296.0, "BORON"} },
	 { 6012, {6, -2, 296.0, "CARBON"} },
	 {    6, {6, -2, 296.0, "CARBON"} },   // this may not work
	 {    7, {7, -2, 296.0, "NITROGEN"} },
	 {    9, {9,-2,  296.0, "FLUORINE"} },
	 {   10, {10,-2, 296.0, "NEON"} },
	 {   11, {11, 23, 296.0,"SODIUM"} },
	 {   12, {12, -2, 296.0,"MAGNESIU"} },
	 {   13, {13, 27, 296.0,"ALUMINUM"} },
	 {   14, {14, -2, 296.0,"SILICON"} },
	 {   15, {15, 31, 296.0,"PHOSPHO"} },
	 {   16, {16, -2, 296.0,"SULFUR"} },
	 {   17, {17, -2, 296.0,"CHLORINE"} },
	 {18040, {18, 40, 296.0,"ARGON-40"} },
	 {   18, {18, -2, 296.0,"ARGON"} },
	 {   19, {19, -2, 296.0,"POTASSIU"} },
	 {   20, {20, -2, 296.0,"CALCIUM"} },
	 {   21, {21, -2, 296.0,"SCANDIUM"} },
	 {   22, {22, -2, 296.0,"TITANIUM"} },
	 {   23, {23, -2, 296.0,"VANADIUM"} },
	 {   24, {24, -2, 296.0,"CHROMIUM"} },
         {   25, {25, 55, 296.0,"MANGANES"} },
	 {   26, {26, -2, 296.0,"IRON"} },
	 {   27, {27, 59, 296.0,"COBALT"} },
	 {   28, {28, -2, 296.0,"NICKEL"} },
	 {   29, {29, -2, 296.0,"COPPER"} },
	 {   30, {30, -2, 296.0,"ZINC"} },
	 {   31, {31, -2, 296.0,"GALLIUM"} },
	 {   32, {32, -2, 296.0,"GERMANIU"} },
	 {   33, {33, -2, 296.0,"ARSENIC"} },
	 {   34, {34, -2, 296.0,"SELENIUM"} },
	 {   35, {35, -2, 296.0,"BROMINE"} },
	 {   36, {36, -2, 296.0,"KRYPTON"} },
	 {   37, {37, -2, 296.0,"RUBIDIUM"} },
	 {   38, {38, -2, 296.0,"STRONTIU"} },
	 {   39, {39, -2, 296.0,"YTTRIUM"} },
	 {   40, {40, -2, 296.0,"ZIRCONIU"} },
	 {41093, {41, 93, 296.0,"NIOBIUM"} },
	 {   42, {42, -2, 296.0,"MOLYBDEN"} },
	 {43099, {43, 99, 296.0,"99-TC"} },
	 {   44, {44, -2, 296.0,"PALLADIU"} },
	 {   45, {45, -2, 296.0,"RHODIUM"} },
	 {46102, {45,102, 296.0,"102-PD"} },
	 {46104, {45,104, 296.0,"104-PD"} },
	 {46105, {45,105, 296.0,"105-PD"} },
	 {46106, {45,106, 296.0,"106-PD"} },
	 {46108, {45,108, 296.0,"108-PD"} },
	 {46110, {45,110, 296.0,"110-PD"} },
	 {   47, {47, -2, 296.0,"SILVER"} },
	 {   48, {48, -2, 296.0,"CADMIUM"} },
	 {   49, {49, -2, 296.0,"INDIUM"} },
	 {   50, {50, -2, 296.0,"TIN"} },
	 {   51, {51, -2, 296.0,"ANTIMONY"} },
	 {   52, {52, -2, 296.0,"TELLURIUM"} },
	 {53127, {53,127, 296.0,"IODINE"} },
	 {53129, {53,129, 296.0,"129-I"} },
	 {   54, {54, -2, 296.0,"XENON"} },
	 {55133, {55,133, 296.0,"CESIUM"} },
	 {55135, {55,135, 296.0,"135-CS"} },
	 {55137, {55,137, 296.0,"137-CS"} },
	 {   56, {56, -2, 296.0,"BARIUM"} },
	 {   57, {57, -2, 296.0,"LANTHANU"} },
	 {   58, {58, -2, 296.0,"CERIUM"} },
	 {   60, {60, -2, 296.0,"NEODYMIU"} },
	 {   62, {62, -2, 296.0,"SAMARIUM"} },
	 {   63, {63, -2, 296.0,"EUROPIUM"} },
	 {   64, {64, -2, 296.0,"GADOLINI"} },
	 {   65, {65, -2, 296.0,"TERBIUM"} },
	 {   66, {66, -2, 296.0,"DYSPROSI"} },
	 {   71, {71, -2, 296.0,"HAFNIUM"} },
	 {   73, {73, 181, 296.0,"TANTALUM"} },
	 {   74, {74, -2, 296.0,"TUNGSTEN"} },
	 {   75, {75, -2, 296.0,"RHENIUM"} },
	 {   77, {77, -2, 296.0,"IRIDIUM"} },
	 {   78, {78, -2, 296.0,"PLATINUM"} },
	 {79197, {79,197, 296.0,"GOLD"} },
	 {   80, {80, -2, 296.0,"MERCURY"} },
	 {   81, {81, -2, 296.0,"THALLIUM"} },
	 {   82, {82, -2, 296.0,"LEAD"} },
	 {83209, {83,209, 296.0,"BISMUTH"} },
	 {90230, {90,230, 296.0,"230-TH"} },
	 {90232, {90,232, 296.0,"232-TH"} },
	 {92233, {90,233, 296.0,"233-U"} },
	 {92235, {90,235, 296.0,"235-U"} },
	 {92238, {90,238, 296.0,"238-U"} },
	 {94239, {94,239, 296.0,"239-PU"} }
    });

  const size_t zaid=Z*1000+iso;
  mapTYPE::const_iterator mc=mUnit.find(zaid);
  if (mc==mUnit.end()) mc=mUnit.find(Z);

  return (mc==mUnit.end()) ?
    std::tuple<int,int,double,std::string>(0,0,0.0,"") : mc->second;
}

std::string
LowMat::getFLUKAinfn(const size_t Z,
		     const size_t iso,
		     const std::string& matName)
  /*!
    Return the WHAT/SDUM string: id1 id2 id3 - - name
    base on the INFN required iostopes
    \param Z :: atomic number
    \param iso :: isotope number
    \param material name for output
  */
{
  const auto& id = getIDinfn(Z,iso);

  std::ostringstream cx;
  if (std::get<0>(id))
    {
      cx<<"LOW-MAT "<<matName<<" "
	<<std::get<0>(id)<<" "
	<<std::get<1>(id)<<" "
	<<std::get<2>(id)
	<<" - - "<<std::get<3>(id)<<" ";
    }

  return cx.str();
}


std::string
LowMat::getFLUKAcern(const size_t Z,
		     const size_t iso,
		     const std::string& matName)
  /*!
    Return the WHAT/SDUM string: id1 id2 id3 - - name
    base on the INFN required iostopes
    \param Z :: atomic number
    \param iso :: isotope number
    \param material name for output
  */
{
  const auto& id = getIDcern(Z,iso);

  std::ostringstream cx;
  if (std::get<0>(id))
    {
      cx<<"LOW-MAT "<<matName<<" "
	<<std::get<0>(id)<<" "
	<<std::get<1>(id)<<" "
	<<std::get<2>(id)
	<<" - - "<<std::get<3>(id)<<" ";
    }

  return cx.str();
}
