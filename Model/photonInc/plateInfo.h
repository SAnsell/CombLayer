/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   photonInc/plateInfo.h
 *
 * Copyright (c) 2004-2016 by Stuart Ansell
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
#ifndef photonSystem_plateInfo_h
#define photonSystem_plateInfo_h

class Simulation;

namespace photonSystem
{


/*!
  \struct plateInfo
  \author S. Ansell
  \version 1.0
  \date June 2016
  \brief A simple plate data system
*/
 
struct plateInfo
{
  std::string name;            ///< Name for layer
  double thick;                ///< Thickness
  double vHeight;              ///< void height
  double vWidth;               ///< void width
  int mat;                     ///< Material
  double temp;                 ///< temperature [K]


  plateInfo();
  plateInfo(const std::string&);
  plateInfo(const plateInfo&);
  plateInfo& operator=(const plateInfo&);
  ~plateInfo() {}
  
};

}

#endif
