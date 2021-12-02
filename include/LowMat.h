/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   include/LowMat.h
 *
 * Copyright (c) 2004-2021 by Konstantin Batkov
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

#ifndef LowMat_h
#define LowMat_h

/*!
  \class LowMat
  \brief Sets LOW-MAT card for FLUKA materials
  \author K. Batkov
  \version 1.0
  \date June 2021
 */

class LowMat
{
 private:

  static const std::string& getLowName(const size_t);
  static std::tuple<int,int,double> getID(const size_t,const size_t);

public:

  static std::string getFLUKA(const size_t,const size_t,const std::string&);
  
};

#endif
