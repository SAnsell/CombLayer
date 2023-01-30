/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   transportInc/countUnit
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
#ifndef Transport_countUnit_h
#define Transport_countUnit_h

namespace MonteCarlo
{
  class particle;
}

namespace Transport
{

/*!  
  \class countUnit
  \brief Defines a detector
  \version 1.0
  \author S. Ansell
  \date December 2022

  This supports simple double** array. This is to avoid
  the nightmare of boost multi_array (and extent and other stuff)
  NOTE : The delete operator is manditory / as is a move operator
  for memleak + performance.
*/

class countUnit
{
 private:

  const size_t nIndex;     ///< Nubmer of indexes
  const size_t nEBin;      ///< Nubmer of energy bins
  double** data;           ///< data [index][energy]
  double* cnt;             ///< Weight of particles [index]

  // zero test functions:
  void setMem();
  void zeroMem();
  void copyMem(const countUnit&);
    
  
 public:
  
  explicit countUnit(const size_t,const size_t);
  countUnit(const countUnit&);
  countUnit(countUnit&&);
  countUnit& operator=(const countUnit&);
  countUnit& operator=(countUnit&&);
  ~countUnit();

  /// accessor function
  const double* operator[](const size_t index) const
    { return data[index]; }

  
};

}  // NAMESPACE Transport

#endif
