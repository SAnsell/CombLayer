/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   monteInc/Zaid.h
 *
 * Copyright (c) 2004-2016 by Stuart Ansell
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
#ifndef Zaid_h
#define Zaid_h

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
  
  int index;           ///< Index number
  int tag;             ///< 2 digit tag
  char type;           ///< Type letter
  double density;      ///< Number density
  
 public:
  
  Zaid();
  Zaid(const Zaid&); 
  Zaid& operator=(const Zaid&);
  ~Zaid() { }     ///< Destructor

  bool operator==(const Zaid&) const;

  bool isEquavilent(const int,const int,const char) const;
  int setZaid(const std::string&);
  void setDensity(const double); 
  /// Set a tag number
  void setTag(const int T) { tag=T % 100; }

  /// Get Density
  double getDensity() const { return density; }
  /// Get Isotopic number 
  int getIso() const { return index % 1000; }
  /// Get Element Z
  int getZ() const { return index / 1000; } 
  /// Get Index
  int getZaidNum() const { return index; }
  /// get Tag
  int getTag() const { return tag; }
  /// Get Element key
  int getKey() const { return type; } 
  double getAtomicMass() const;

  void write(std::ostream&) const;

};

std::ostream& operator<<(std::ostream&,const Zaid&);

#endif
