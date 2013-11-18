/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   monteInc/Element.h
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#ifndef Element_h
#define Element_h

class Zaid;

/*!
  \struct Abundance 
  \version 1.9
  \author S. Ansell
  \date August 2007
  \brief Information on natural abundance of an element
*/

struct Abundance
{
  int Z;                               ///< Z of the element
  std::vector<int> WList;              ///< Atomic Weight of the components
  std::vector<double> Frac;            ///< Fractional weight

  Abundance();
  explicit Abundance(const int);
  Abundance(const Abundance&);
  Abundance& operator=(const Abundance&);
  ~Abundance();

  void addIso(const int,const double);
  double meanMass() const;

};

/*!
  \class Element
  \brief Class to store/recall elemental symbols
  \author Stuart Ansell
  \date April 2005
  \version 1.2

  Simple class to return information about elements
  by numbers or symbols .
  Modified in 1.1 to include isotopes and atomic abundancy
  Modified in 1.2 to deal with incoming ion types e.g. Cu2+
*/

class Element
{
 private:

  const int Nelem;                  ///< Max number of elements (approx 94)
  std::map<std::string,int> Nmap;   ///< Map of names to Z
  std::vector<std::string> Sym;     ///< Vector of symbols
  std::vector<double> KEdge;        ///< Vector of k-Edge [keV]
  std::vector<Abundance> Isotopes;  ///< Vector of Isotopes and abunances

  void populate();        ///< Initialise the map and symbol table
  void populateEdge();    ///< Initialise the Edge information
  void populateIso();     ///< Set the abundance and isotopes

  Element();
  ///\cond SINGLETON
  Element(const Element&);
  Element& operator=(const Element&);
  ///\endcond SINGLETON

 public:

  static const Element& Instance();
  ~Element();

  int elm(const char*) const;             ///< Determine Z from string
  int elm(const std::string&) const;      ///< Determine Z from string
  int elmIon(const std::string&) const;   
  std::pair<int,int> elmIonPair(const std::string&) const;   
  std::string elmSym(const int) const;    ///< Element symbol from Z

  double Kedge(const std::string&) const;    ///< Get K-edge from Symbol
  double Kedge(const int) const;             ///< Get K-edge from Z
  /// Process cinder zaids
  void addZaid(const Zaid&,std::vector<int>&,std::vector<double>&) const;

  double mass(const int) const;       
  size_t natIsotopes(const int,std::vector<int>&,std::vector<double>&) const;
};

#endif
