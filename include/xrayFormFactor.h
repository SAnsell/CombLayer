/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   include/particleConv.h
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
#ifndef particleConv_h
#define particleConv_h

/*!
  \struct formUnit
  \version 1.0
  \author S. Ansell
  \date December 2022
  \brief Form-Factor
*/

struct formUnit
{
  double F[9];
};

/*!
  \class xrayFormFactor
  \version 1.0
  \author S. Ansell
  \date December 2022
  \brief Singleton for x-ray form factor
*/

class xrayFormFactor
{
 private:

  const std::map<int,formUnit> atomF;
    
  xrayFormFactor();  
  xrayFormFactor(const xrayFormFactor&);
  xrayFormFactor& operator=(const xrayFormFactor&);

  static const std::map<int,formUnit> makeAtomF();
  
 public:

  static const xrayFormFactor& Instance();

  double fQ(const double) const;
    
};
 


#endif 
