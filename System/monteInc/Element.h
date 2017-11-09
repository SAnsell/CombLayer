/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   monteInc/Element.h
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
#ifndef MonteCarlo_Element_h
#define MonteCarlo_Element_h

namespace MonteCarlo
{

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
  size_t Z;                            ///< Z of the element
  std::vector<size_t> WList;           ///< Atomic Weight of the components
  std::vector<double> Frac;            ///< Fractional weight

  Abundance();
  explicit Abundance(const size_t);
  Abundance(const Abundance&);
  Abundance& operator=(const Abundance&);
  ~Abundance();

  void addIso(const size_t,const double);
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

  const size_t Nelem;                  ///< Max number of elements (approx 94)
  std::map<std::string,size_t> Nmap;   ///< Map of names to Z
  std::vector<std::string> Sym;        ///< Vector of symbols
  std::vector<double> KEdge;           ///< Vector of k-Edge [keV]
  std::vector<Abundance> Isotopes;     ///< Vector of Isotopes and abunances

  void populate();     
  void populateEdge(); 
  void populateIso();  

  Element();
  ///\cond SINGLETON
  Element(const Element&);
  Element& operator=(const Element&);
  ///\endcond SINGLETON

 public:

  static const Element& Instance();
  ~Element();

  size_t elm(const std::string&) const;    
  size_t elmIon(const std::string&) const;   
  std::pair<size_t,int> elmIonPair(const std::string&) const;   
  const std::string& elmSym(const size_t) const;    

  double Kedge(const std::string&) const;    ///< Get K-edge from Symbol
  double Kedge(const size_t) const;             ///< Get K-edge from Z
  /// Process cinder zaids
  void addZaid(const Zaid&,std::vector<size_t>&,
	       std::vector<double>&) const;

  double mass(const size_t) const;       
  size_t natIsotopes(const size_t,std::vector<size_t>&,
		     std::vector<double>&) const;
};

}

#endif
