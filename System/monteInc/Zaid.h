/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   monteInc/Zaid.h
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
#ifndef MonteCarlo_Zaid_h
#define MonteCarlo_Zaid_h

namespace MonteCarlo
{

/*!
  \class Zaid 
  \brief A split Zaid (full) with density
  \version 1.0
  \author S. Ansell
  \date September 2009
 */

class Zaid
{
 private:
  
  size_t index;        ///< Index number (ZZZAAA)
  size_t tag;          ///< 2 digit tag (library indentifier)
  char type;           ///< Type letter (data class)
  double density;      ///< Number density
  
 public:
  
  Zaid();
  Zaid(const Zaid&); 
  Zaid& operator=(const Zaid&);
  ~Zaid() { }     ///< Destructor

  bool operator==(const Zaid&) const;
  bool operator<(const Zaid&) const;

  bool isEquivalent(const size_t,const size_t,const char) const;
  size_t setZaid(const std::string&);
  std::string getZaid() const;
  std::string getFlukaName() const;
  void setDensity(const double); 
  /// Set a tag number
  void setTag(const size_t T) { tag=T % 100; }

  /// Get Density
  double getDensity() const { return density; }
  /// Get Isotopic number 
  size_t getIso() const { return index % 1000; }
  /// Get Element Z
  size_t getZ() const { return index / 1000; } 
  /// Get Index
  size_t getZaidNum() const { return index; }
  /// get Tag
  size_t getTag() const { return tag; }
  /// Get Element key
  char getKey() const { return type; } 
  double getAtomicMass() const;

  void write(std::ostream&) const;

};

std::ostream& operator<<(std::ostream&,const Zaid&);

}

#endif
