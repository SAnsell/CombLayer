/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructVarInc/CFFlanges.h
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#ifndef setVariable_CFFlanges_h
#define setVariable_CFFlanges_h


namespace setVariable
{

/*!
  \struct CF40
  \version 1.0
  \author S. Ansell
  \date January 2018
  \brief CFFlanges for CF40
*/

struct CF40
{
  static constexpr double innerRadius=1.25;
  static constexpr double wallThick=0.5;
  static constexpr double flangeRadius=2.7;
  static constexpr double flangeLength=1.0;
  static constexpr double bellowStep=1.0;
  static constexpr double bellowThick=1.0;
};

struct CF50
{
  static constexpr double innerRadius=2.55;
  static constexpr double wallThick=0.5;
  static constexpr double flangeRadius=4.3;
  static constexpr double flangeLength=1.0;
  static constexpr double bellowStep=1.0;
  static constexpr double bellowThick=1.0;
};


struct CF63
{
  static constexpr double innerRadius=3.2;
  static constexpr double wallThick=0.5;
  static constexpr double flangeRadius=5.7;
  static constexpr double flangeLength=1.0;
  static constexpr double bellowStep=1.0;
  static constexpr double bellowThick=1.0;
};

struct CF100
{
  static constexpr double innerRadius=5.1;
  static constexpr double wallThick=0.5;
  static constexpr double flangeRadius=7.7;
  static constexpr double flangeLength=1.0;
  static constexpr double bellowStep=1.5;
  static constexpr double bellowThick=1.5;
};

}

#endif
 
