/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructVarInc/CFFlanges.h
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
#ifndef setVariable_CFFlanges_h
#define setVariable_CFFlanges_h


namespace setVariable
{

/*!
  \struct CF16
  \version 1.0
  \author S. Ansell
  \date November 2019
  \brief CFFlanges for CF16
*/

struct CF16
{
  static constexpr double innerRadius=0.8;
  static constexpr double wallThick=0.10;
  static constexpr double flangeRadius=1.7;
  static constexpr double flangeLength=0.65;
  static constexpr double bellowStep=0.5;
  static constexpr double bellowThick=0.5;
  static constexpr double gasketRadius=0.5;
  static constexpr double gasketThick=0.7;
  static constexpr double boltRadius=0.4;
  static constexpr size_t nBolts=6;
};

/*!
  \struct CF25
  \version 1.0
  \author S. Ansell
  \date November 2019
  \brief CFFlanges for CF25
*/

struct CF25
{
  static constexpr double innerRadius=1.25;
  static constexpr double wallThick=0.12;
  static constexpr double flangeRadius=2.7;
  static constexpr double flangeLength=1.15;
  static constexpr double bellowStep=0.9;
  static constexpr double bellowThick=0.8;
  static constexpr double gasketRadius=1.65;
  static constexpr double gasketThick=0.7;
  static constexpr double boltRadius=0.3;
  static constexpr size_t nBolts=4;
};

  
/*!
  \struct CF40
  \version 2.0
  \author S. Ansell
  \date January 2018
  \brief CFFlanges for CF40
*/

struct CF40
{
  static constexpr double innerRadius=1.9;
  static constexpr double wallThick=0.15;
  static constexpr double flangeRadius=3.5;
  static constexpr double flangeLength=1.3;
  static constexpr double bellowStep=1.0;
  static constexpr double bellowThick=1.0;
  static constexpr double gasketRadius=1.0;
  static constexpr double gasketThick=1.3;
  static constexpr double boltRadius=0.3;
  static constexpr size_t nBolts=6;
};

/*!
  \struct CF50
  \version 1.0
  \author S. Ansell
  \date January 2018
  \brief CFFlanges for CF50
*/
 
struct CF50
{
  static constexpr double innerRadius=2.55;
  static constexpr double wallThick=0.3;
  static constexpr double flangeRadius=4.3;
  static constexpr double flangeLength=0.5;
  static constexpr double bellowStep=1.0;
  static constexpr double bellowThick=1.0;
  static constexpr size_t nBolts=8;
};

/*!
  \struct CF63
  \version 1.0
  \author S. Ansell
  \date January 2018
  \brief CFFlanges for CF63
*/

struct CF63
{
  static constexpr double innerRadius=3.2;
  static constexpr double wallThick=0.4;
  static constexpr double flangeRadius=5.7;
  static constexpr double flangeLength=1.7;
  static constexpr double bellowStep=1.0;
  static constexpr double bellowThick=1.0;
  static constexpr size_t nBolts=8;
};

/*!
  \struct CF100
  \version 1.0
  \author S. Ansell
  \date January 2018
  \brief CFFlanges for CF100
*/

struct CF100
{
  static constexpr double innerRadius=5.1;
  static constexpr double wallThick=0.5;
  static constexpr double flangeRadius=7.7;
  static constexpr double flangeLength=2.0;
  static constexpr double bellowStep=1.5;
  static constexpr double bellowThick=1.5;
  static constexpr size_t nBolts=16;
};

/*!
  \struct CF120
  \version 1.0
  \author S. Ansell
  \date January 2018
  \brief CFFlanges for CF120
*/

struct CF120
{
  static constexpr double innerRadius=6.1;
  static constexpr double wallThick=0.5;
  static constexpr double flangeRadius=8.925;
  static constexpr double flangeLength=2.25;
  static constexpr double bellowStep=1.5;
  static constexpr double bellowThick=1.5;
  static constexpr size_t nBolts=18;
  
};

/*!
  \struct CF150
  \version 1.0
  \author S. Ansell
  \date January 2018
  \brief CFFlanges for CF150
*/

struct CF150
{
  static constexpr double innerRadius=7.55;
  static constexpr double wallThick=0.5;
  static constexpr double flangeRadius=10.15;
  static constexpr double flangeLength=2.5;
  static constexpr double bellowStep=1.5;
  static constexpr double bellowThick=1.5;
  static constexpr size_t nBolts=20;
};

/*!
  \struct CF200
  \version 1.0
  \author S. Ansell
  \date October 2019
  \brief CFFlanges for CF200
*/

struct CF200
{
  static constexpr double innerRadius=10.0;
  static constexpr double wallThick=0.5;
  static constexpr double flangeRadius=12.65;
  static constexpr double flangeLength=3.45;
  static constexpr double bellowStep=2.5;
  static constexpr double bellowThick=2.5;
  static constexpr size_t nBolts=24;
};


/*!
  \struct CF350
  \version 1.0
  \author S. Ansell
  \date June 2019
  \brief CFFlanges for CF350
*/

struct CF350
{
  static constexpr double innerRadius=33.0;
  static constexpr double wallThick=0.6;
  static constexpr double flangeRadius=41.15;  //
  static constexpr double flangeLength=3.5;
  static constexpr double bellowStep=2.5;
  static constexpr double bellowThick=3.5;
  static constexpr size_t nBolts=30;
};

}

#endif
 
