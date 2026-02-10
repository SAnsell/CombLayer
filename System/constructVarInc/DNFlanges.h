/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   constructVarInc/DNFlanges.h
 *
 * Copyright (c) 2004-2026 by Konstantin Batkov
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
#ifndef setVariable_DNFlanges_h
#define setVariable_DNFlanges_h

// DN (Nominal diameter) pipe sizes

namespace setVariable
{

  /*!
    \struct DN63
    \version 1.0
    \author K. Batkov
    \date February 2026
    \brief DNFlanges for DN63
  */

  struct DN63
  {
    static constexpr double outerRadius=6.3/2.0;
    static constexpr double wallThick=0.2;
    static constexpr double innerRadius=outerRadius-wallThick;
    static constexpr double flangeRadius=11.35/2;
    static constexpr double flangeLength=1.7;
    /* static constexpr double bellowStep=0.5; */
    /* static constexpr double bellowThick=0.5; */
    /* static constexpr double gasketRadius=0.5; */
    /* static constexpr double gasketThick=0.7; */
    /* static constexpr double boltRadius=0.4; */
    static constexpr size_t nBolts=8;
  };

}  // NAMESPACE setVariables
#endif
