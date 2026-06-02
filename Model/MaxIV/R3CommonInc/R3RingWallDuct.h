/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   R3CommonInc/R3RingWallDuct.h
 *
 * Copyright (c) 2026 by U. Friman-Gayer
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

#ifndef xraySystem_R3RingWallDuct_h
#define xraySystem_R3RingWallDuct_h

namespace xraySystem{

struct R3RingWallDuct{
    double distFromRatchetWall;
    double floorHeight;
    double holeDiameter;
};

}

#endif