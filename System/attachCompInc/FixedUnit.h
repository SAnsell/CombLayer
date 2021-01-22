/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   attachCompInc/FixedUnit.h
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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
#ifndef attachSystem_FixedUnit_h
#define attachSystem_FixedUnit_h

class FuncDataBase;

namespace attachSystem
{
/*!
  \class FixedUnit
  \version 1.0
  \author S. Ansell
  \date  December 2019
  \brief FixedComp with abstract units removed
*/

class FixedUnit : public FixedComp
{  
 public:

  /// Simple constructor
  FixedUnit(const std::string& K) :
    FixedComp(0,K) {}

  /// Simple constructor [no objectregister]
  FixedUnit(const std::string& K,const size_t I) :
    FixedComp(I,K) {}

  /// Simple constructor [with objectregister]
  FixedUnit(const size_t I,const std::string& K) :
    FixedComp(K,I) {}

  /// Simple constructor [extra range]
  FixedUnit(const std::string& K,const size_t I,const size_t S) :
    FixedComp(K,I,S) {}

  /// Simple constructor with full axis
  FixedUnit(const std::string& K,const size_t I,
	    const Geometry::Vec3D& OO,const Geometry::Vec3D& XX, 
	    const Geometry::Vec3D& YY,const Geometry::Vec3D& ZZ) :
    FixedComp(I,K)
  {
    FixedComp::createUnitVector(OO,XX,YY,ZZ);
  }

  /// System to get axis from existing FC
  FixedUnit(const std::string& K,const FixedComp& FC,
	    const long int index) :
    FixedComp(0,K) { createUnitVector(FC,index); }

  /// System to get axis from existing FC
  FixedUnit(const std::string& K,const FixedComp& FC,
	    const std::string linkName) :
    FixedComp(0,K)
  { createUnitVector(FC,FC.getSideIndex(linkName)); }
      
  FixedUnit(const FixedUnit& A) : FixedComp(A) {}
  FixedUnit(const FixedComp& A) : FixedComp(A) {}
  virtual ~FixedUnit() {}     ///< Destructor

  void createAll(Simulation&,const FixedComp&,const long int) {}
};

}

#endif
 
