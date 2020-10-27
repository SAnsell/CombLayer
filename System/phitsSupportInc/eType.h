/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   phitsSupportInc/eType.h
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
#ifndef phitsSystem_eType_h
#define phitsSystem_eType_h

namespace phitsSystem
{

/*!
  \class eType
  \version 1.0
  \date October 2020
  \author S. Ansell
  \brief simple etype card for	PHITS tallies
*/

class eType 
{
 private:

  int eIndex;                       ///< Etype
  
  std::vector<double> eValue;       ///< Values (e-type: 1)

  size_t ne;                   ///< Number of points
  double eMin;                 ///< E Min [MeV] [e-type 2,3,4,5]
  double eMax;                 ///< E Max [MeV] [e-type 2,3,4,5]
  double eDel;                 ///< Ewidth or log( M_{i+1} / M_i ) ) [etype 5]

 public:

  eType(const std::vector<double>&);
  eType(const std::string&,const double,const double,const double);
  eType(const std::string&,const size_t,const double,const double);
  
  eType(const eType&);
  eType& operator=(const eType&);
  virtual ~eType() {}   ///< Destructor

  int calcUnit(const std::string&) const;

  void write(std::ostream&) const;

};

}  

#endif
