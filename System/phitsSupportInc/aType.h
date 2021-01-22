/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   phitsSupportInc/aType.h
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
#ifndef phitsSystem_aType_h
#define phitsSystem_aType_h

namespace phitsSystem
{

/*!
  \class aType
  \version 1.0
  \date October 2020
  \author S. Ansell
  \brief 
*/

class aType 
{
 private:

  int aIndex;                       ///< A-type 
  
  std::vector<double> aValue;       ///< Values (e-type: 1)

  size_t na;                       ///< Number of points
  double aMin;                     ///< A Min [deg] [a-type 2]
  double aMax;                     ///< A Max [deg] [a-type 2]

 public:

  aType(const std::string&,const std::vector<double>&);
  aType(const std::string&,const double,const double,const double);
  aType(const std::string&,const size_t,const double,const double);
  
  aType(const aType&);
  aType& operator=(const aType&);
  virtual ~aType() {}   ///< Destructor

  int calcUnit(const std::string&) const;
  
  void write(std::ostream&) const;

};

}  

#endif
