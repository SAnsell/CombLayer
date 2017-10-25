/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   physicsInc/EUnit.h
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
#ifndef physicsSystem_EUnit_h
#define physicsSystem_EUnit_h

namespace physicsSystem
{
  
/*!
  \class EUnit
  \version 1.0
  \date April 2015
  \author S.Ansell
  \brief EUnit token
  
  Taken from document la-ur-13-23395
*/
  
class EUnit
{
 private:

  int fracType;      ///< Fraction type 0 : Void, +/-1 :S,  +/-2:D, 3: blank
  size_t dirType;    ///< Direction type 0 : particle , (1-3) Axis
  double aFrac;      ///< Number between +/- (0 and 1.0 )
  size_t vID;        ///< Vector ID number

 public:
  
  EUnit(const int,const size_t,const double,const size_t); 
  EUnit(const EUnit&);
  EUnit& operator=(const EUnit&);
  ~EUnit() {}         ///< Destructor
  
  void write(std::ostream&) const;
};

}

#endif
