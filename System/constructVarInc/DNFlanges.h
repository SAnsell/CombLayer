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
    static constexpr size_t nBolts=8;
  };

  // DN100CF 152mm (6″) OD Flange
  // https://www.lewvac.co.uk/product/dn100cf-flanges-152mm-6-od-flanges/
  struct DN100CF
  {
    static constexpr double outerRadius=10.45/2.0;
    static constexpr double wallThick=0.2;
    static constexpr double innerRadius=outerRadius-wallThick;
    static constexpr double flangeRadius=15.16/2.0;
    static constexpr double flangeLength=1.98;
    static constexpr size_t nBolts=16;
  };

  // DN160CF 203mm (8″) OD Flange
  // https://www.lewvac.co.uk/product/dn160cf-flanges-203mm-8-od-flanges/
  struct DN160CF
  {
    static constexpr double outerRadius=15.24/2.0;
    static constexpr double wallThick=0.135;
    static constexpr double innerRadius=outerRadius-wallThick;
    static constexpr double flangeRadius=20.24/2.0;
    static constexpr double flangeLength=2.24;
    static constexpr size_t nBolts=20;
  };

}  // NAMESPACE setVariables
#endif
