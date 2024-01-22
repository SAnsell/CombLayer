/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/PipeTube.h
 *
 * Copyright (c) 2004-2024 by Stuart Ansell
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
#ifndef constructSystem_PipeTube_h
#define constructSystem_PipeTube_h

class Simulation;

namespace constructSystem
{
  
/*!
  \class PipeTube
  \version 1.0
  \author S. Ansell
  \date January 2018
  \brief PipeTube unit  
*/

class PipeTube :
  public VirtualTube
{
 protected:
  
  double flangeARadius;        ///< Joining Flange radius
  double flangeALength;        ///< Joining Flange length
  double flangeBRadius;        ///< Joining Flange radius
  double flangeBLength;        ///< Joining Flange length
  double flangeACapThick;           ///< Thickness of Flange cap if present
  double flangeBCapThick;           ///< Thickness of Flange cap if present
  
  HeadRule makeOuterVoid(Simulation&) override;
  
  void populate(const FuncDataBase&) override;
  void createSurfaces() override;
  void createObjects(Simulation&) override;
  void createLinks() override;

  
 public:

  PipeTube(const std::string&);
  PipeTube(const PipeTube&);
  PipeTube& operator=(const PipeTube&);
  ~PipeTube() override;

  using VirtualTube::createPorts;
  void createPorts(Simulation&) override;
  
};

}

#endif
 
