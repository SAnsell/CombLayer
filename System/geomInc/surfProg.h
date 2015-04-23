/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   geomInc/surfProg.h
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#ifndef Geometry_surfProg_h
#define Geometry_surfProg_h


namespace Geometry
{

namespace surfProg
{
  int possibleArbPoly(const std::string&);
  int possibleCone(const std::string&);
  int possibleCylinder(const std::string&);
  int possibleGeneral(const std::string&);
  int possibleMBrect(const std::string&);
  int possiblePlane(const std::string&);
  int possibleSphere(const std::string&);
  int possibleTorus(const std::string&);

}


}

#endif
