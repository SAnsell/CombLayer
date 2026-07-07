/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   attachCompInc/FixedUnit.h
 *
 * Copyright (c) 2004-2022 by Stuart Ansell
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

  /// Simple constructor [no objectregister]
  explicit FixedUnit(FixedComp::unregistered_t,const std::string& K) :
    FixedComp(FixedComp::unregistered,K) {}

  // NOTE: because FixedComp(const std::string&,const size_t) is bound
  // to the *legacy* (KeyName,NL) constructor (kept for source/behaviour
  // compatibility with not-yet-migrated (Key,NL) call sites -- NL still
  // pre-sizes LU there), the 2-argument constructor below does NOT set
  // a custom resSize -- it always uses the default (10000). Use the
  // 3-argument constructor for an explicit resSize override.

  /// Simple constructor [with objectregister]
  FixedUnit(const std::string& K,const size_t legacyNL =10000) :
    FixedComp(K,legacyNL) {}

  /// Simple constructor [with objectregister, explicit reserved range]
  FixedUnit(const std::string& K,const size_t legacyNL,
	    const size_t resSize) :
    FixedComp(K,legacyNL,resSize) {}

  /// Simple constructor with full axis [no objectregister]
  FixedUnit(FixedComp::unregistered_t,const std::string& K,
	    const Geometry::Vec3D& OO,const Geometry::Vec3D& XX,
	    const Geometry::Vec3D& YY,const Geometry::Vec3D& ZZ) :
    FixedComp(FixedComp::unregistered,K)
  {
    FixedComp::createUnitVector(OO,XX,YY,ZZ);
  }

  /// System to get axis from existing FC
  FixedUnit(const std::string& K,const FixedComp& FC,
	    const long int index) :
    FixedComp(FixedComp::unregistered,K) { createUnitVector(FC,index); }

  /// System to get axis from existing FC
  FixedUnit(const std::string& K,const FixedComp& FC,
	    const std::string linkName) :
    FixedComp(FixedComp::unregistered,K)
  { createUnitVector(FC,FC.getSideIndex(linkName)); }

  FixedUnit(const FixedUnit& A) : FixedComp(A) {}
  FixedUnit(const FixedComp& A) : FixedComp(A) {}
  ~FixedUnit() override {}     ///< Destructor

};

}

#endif
 
