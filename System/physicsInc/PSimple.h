/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   physicsInc/PSimple.h
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
#ifndef physicsSystem_PSimple_h
#define physicsSystem_PSimple_h

namespace physicsSystem
{

/*!
  \class PSimple 
  \version 1.0
  \date Jully 2010
  \version 1.0
  \author S. Ansell
  \brief Holds cut cards
  
  Holds any card which indexes.
  Has not particles
  a cell mapping to number. The map is ordered
  prior to being written.
*/

class PSimple : public PhysCard
{
 private:

  std::string KeyName;               ///< NameType e.g. cut/phys
  std::array<int,5> defFlag;         ///< Default values
  std::array<double,5> vals;         ///< Values

 public:

  PSimple(const std::string&);
  PSimple(const PSimple&);
  PSimple& operator=(const PSimple&);
  PSimple* clone() const;
  virtual ~PSimple();

  // NOP function from PhysCard
  virtual void setEnergyCut(const double);
  virtual void clear();
  /// Access key
  virtual std::string getKey() const { return KeyName; }   

  double getValue(const size_t) const;
  void setValue(const size_t,const double);
  void setValues(const size_t,const double =0.0,const double =0.0,
		 const double =0.0,const double =0.0,const double =0.0);
  void setValues(const std::string&);
  void setDef(const size_t);

  virtual void write(std::ostream&) const;
  
};

std::ostream&
operator<<(std::ostream&,const PSimple&);

}

#endif
