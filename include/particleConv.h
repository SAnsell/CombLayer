/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   include/particleConv.h
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
#ifndef particleConv_h
#define particleConv_h

/*!
  \struct pName 
  \version 1.0
  \author S. Ansell
  \date October 2017
  \brief interchange names for particles
*/

struct pName
{
  const std::string mcnpName;         ///< MCNP [single character]
  const int mcnpITYP;                 ///< MCNP particle number
  const std::string flukaName;        ///< FLUKA word name
  const int flukaITYP;                ///< iTyp number from fluka
  const std::string phitsName;        ///< PHITS word name 
  const int phitsITYP;                ///< iTyp number from phits
  const int mcplNumber;               ///< MCPL number
  const int nucleon;                  ///< number of nucleons
  
  pName(const std::string&,const int,
	const std::string&,const int,
	const std::string&,const int,
	const int,const int);

  pName(const pName&);
  ~pName() {}          ///< Destructor

};

/*!
  \class particleConv
  \version 1.0
  \author S. Ansell
  \date October 2017
  \brief Singleton for particle name change
*/

class particleConv
{
 private:

  std::vector<pName> particleVec;               ///< Particle vector
  std::map<std::string,size_t> mcnpIndex;       ///< mcnp  index
  std::map<std::string,size_t> flukaIndex;      ///< fluka Index
  std::map<std::string,size_t> phitsIndex;      ///< phits index

  particleConv();  
  particleConv(const particleConv&);
  particleConv& operator=(const particleConv&);

  size_t getMCNPIndex(const int) const;
  size_t getPHITSIndex(const int) const;
  size_t getFLUKAIndex(const int) const;

  const pName& getMCNPpItem(const std::string&) const;
  const pName& getPHITSpItem(const std::string&) const;
  const pName& getFLUKApItem(const std::string&) const;

  size_t getMCNPIndex(const std::string&) const;
  size_t getPHITSIndex(const std::string&) const;
  size_t getFLUKAIndex(const std::string&) const;

  size_t getNameIndex(const std::string&) const;
  const pName& getNamePItem(const std::string&) const;

  
 public:

  static const particleConv& Instance();
  
  /* const std::string& phitsType(const char) const; */
  /* int phitsITYP(const char) const; */
  /* int nucleon(const char) const; */

  //  const std::string& phitsType(const std::string&) const;

  int nucleon(const std::string&) const;
  int mcnpITYP(const std::string&) const;
  int flukaITYP(const std::string&) const;
  int phitsITYP(const std::string&) const;
  //  int flukaITYP(const std::string&) const;
  //  int phitsITYP(const std::string&) const;


  bool hasName(const std::string&) const;

  const std::string& nameToPHITS(const std::string&) const;
  const std::string& nameToFLUKA(const std::string&) const;
  
  const std::string& mcnpToFLUKA(const int) const;
  //  const std::string& mcnpToFluka(const int) const;

};
 


#endif 
