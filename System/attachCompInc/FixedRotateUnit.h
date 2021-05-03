/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   attachCompInc/FixedRotateUnit.h
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
#ifndef attachSystem_FixedRotateUnit_h
#define attachSystem_FixedRotateUnit_h

class FuncDataBase;

namespace attachSystem
{
/*!
  \class FixedRotateUnit
  \version 1.0
  \author S. Ansell
  \date  December 2019
  \brief FixedComp with abstract units removed
*/

class FixedRotateUnit : public FixedRotate
{  
 public:

  /// Simple constructor
  FixedRotateUnit(const size_t I,const std::string& K) :
    FixedRotate(I,K) {}
  /// Simple constructor
  FixedRotateUnit(const std::string& K,const size_t I) :
    FixedRotate(K,I) {}
  /// Simple constructor [extra range]
  FixedRotateUnit(const std::string& K,const size_t I,const size_t S) :
    FixedRotate(K,I,S) {}

  /// System to get axis from existing FC
  FixedRotateUnit(const std::string& K,const FixedComp& FC,
		  const long int index) :
    FixedRotate(K,0)
  { createUnitVector(FC,index); }

  /// System to get axis from existing FC and variables
  FixedRotateUnit(const std::string& K,const FuncDataBase& Control,
		  const FixedComp& FC,const long int index) :
    FixedRotate(K,0)
  { FixedRotate::populate(Control); createUnitVector(FC,index); }

  /// System to get axis from existing FC and variables (and no name)
  FixedRotateUnit(const FuncDataBase& Control,
		  const FixedComp& FC,const long int index) :
    FixedRotate(0)
  { FixedRotate::populate(Control); createUnitVector(FC,index); }

  /// System to get axis from existing FC and variables
  FixedRotateUnit(const std::string& K,const FuncDataBase& Control,
		  const FixedComp& FC,const std::string& linkName) :
    FixedRotate(K,0)
  { FixedRotate::populate(Control);
    createUnitVector(FC,FC.getSideIndex(linkName)); }

  /// System to get axis from existing FC and variables [no name]
  FixedRotateUnit(const FuncDataBase& Control,const std::string& K,
		  const FixedComp& FC,const std::string& linkName) :
    FixedRotate(0,K)
  {
    FixedRotate::populate(Control);
    createUnitVector(FC,FC.getSideIndex(linkName));
  }

  FixedRotateUnit(const std::string& K,
		  const Geometry::Vec3D& OG,
		  const Geometry::Vec3D& YAxis,
		  const Geometry::Vec3D& ZAxis) :
    FixedRotate(0,K)
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

  FixedRotateUnit(const FixedRotateUnit& A) : FixedRotate(A) {}
  FixedRotateUnit(const FixedRotate& A) : FixedRotate(A) {}
  virtual ~FixedRotateUnit() {}     ///< Destructor

  void createAll(Simulation&,const FixedComp&,const long int) {}
};

}

#endif
 
