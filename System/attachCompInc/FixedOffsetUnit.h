/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   attachCompInc/FixedOffsetUnit.h
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#ifndef attachSystem_FixedOffsetUnit_h
#define attachSystem_FixedOffsetUnit_h

class FuncDataBase;

namespace attachSystem
{
/*!
  \class FixedOffsetUnit
  \version 1.0
  \author S. Ansell
  \date  December 2019
  \brief FixedComp with abstract units removed
*/

class FixedOffsetUnit : public FixedOffset
{  
 public:

  /// Simple constructor
  FixedOffsetUnit(const std::string& K,const size_t I) :
    FixedOffset(K,I) {}
  /// Simple constructor [extra range]
  FixedOffsetUnit(const std::string& K,const size_t I,const size_t S) :
    FixedOffset(K,I,S) {}

  /// System to get axis from existing FC
  FixedOffsetUnit(const std::string& K,const FixedComp& FC,
		  const long int index) :
    FixedOffset(K,0)
  { createUnitVector(FC,index); }

  /// System to get axis from existing FC and variables
  FixedOffsetUnit(const std::string& K,const FuncDataBase& Control,
		  const FixedComp& FC,const long int index) :
    FixedOffset(K,0)
  { FixedOffset::populate(Control); createUnitVector(FC,index); }

  /// System to get axis from existing FC and variables (and no name)
  FixedOffsetUnit(const FuncDataBase& Control,
		  const FixedComp& FC,const long int index) :
    FixedOffset(0)
  { FixedOffset::populate(Control); createUnitVector(FC,index); }

  /// System to get axis from existing FC and variables
  FixedOffsetUnit(const std::string& K,const FuncDataBase& Control,
		  const FixedComp& FC,const std::string& linkName) :
    FixedOffset(K,0)
  { FixedOffset::populate(Control);
    createUnitVector(FC,FC.getSideIndex(linkName)); }

  /// System to get axis from existing FC and variables [no name]
  FixedOffsetUnit(const FuncDataBase& Control,const std::string& K,
		  const FixedComp& FC,const std::string& linkName) :
    FixedOffset(0,K)
  {
    FixedOffset::populate(Control);
    createUnitVector(FC,FC.getSideIndex(linkName));
  }

  FixedOffsetUnit(const std::string& K,
		  const Geometry::Vec3D& OG,
		  const Geometry::Vec3D& YAxis,
		  const Geometry::Vec3D& ZAxis) :
    FixedOffset(0,K)
    /*
      Create a vector based on existing basis set
      \param K :: keyame
      \param OG :: Origin
      \param YAxis :: Direction for Y
      \param ZAxis :: Direction for Z
    */

  {
    FixedComp::createUnitVector(OG,YAxis,ZAxis);
  }

  FixedOffsetUnit(const FixedOffsetUnit& A) : FixedOffset(A) {}
  FixedOffsetUnit(const FixedOffset& A) : FixedOffset(A) {}
  virtual ~FixedOffsetUnit() {}     ///< Destructor

  void createAll(Simulation&,const FixedComp&,const long int) {}
};

}

#endif
 
