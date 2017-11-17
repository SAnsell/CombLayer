/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   physicsInc/PhysCard.h
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
#ifndef physicsSystem_PhysCard_h
#define physicsSystem_PhysCard_h

namespace physicsSystem
{

/*!
  \class PhysCard 
  \version 1.0
  \date June 2015
  \version 1.0
  \author S. Ansell
  \brief Virtual header for Physics card physics card
*/

class PhysCard 
{
 public:

  PhysCard() {}
  virtual PhysCard* clone() const =0;
  virtual ~PhysCard() {}

  virtual void clear() =0;
  virtual std::string getKey() const =0;
  virtual void write(std::ostream&) const =0;

  // NOP FUNCTION that can be specialized
  virtual void setEnergyCut(const double) {};
  /// Element check:
  virtual bool hasElm(const std::string&) const { return 0; }
  virtual void addElm(const std::string&) {}
  
};

std::ostream&
operator<<(std::ostream&,const PhysCard&);

}

#endif
