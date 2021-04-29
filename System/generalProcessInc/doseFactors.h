/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   generalProcessInc/doseFactors.h
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
#ifndef tallySystem_doseFactors_h
#define tallySystem_doseFactors_h

namespace attachSystem
{
  class FixedComp;
}

namespace mainSystem
{
  class inputParam;
}

class objectGroups;
class Simulation;

namespace mainSystem
{

/*!
  \namespace doseFactors
  \version 1.0
  \author S. Ansell
  \date July 2012
  \brief Constructs a mesh from inputParam
*/

namespace doseFactors 
{

   const std::string& getDoseConversion();
   const std::string& getPhotonDoseConversion();
};

}

#endif
 
