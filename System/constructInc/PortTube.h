/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/PortTube.h
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
#ifndef constructSystem_PortTube_h
#define constructSystem_PortTube_h

class Simulation;

namespace constructSystem
{
  
/*!
  \class PortTube
  \version 1.0
  \author S. Ansell
  \date January 2018
  \brief PortTube unit  
*/

class PortTube :
  public PipeTube
{
 private:
    
  double portAXStep;       ///< In Port
  double portAZStep;       ///< In Port
  double portARadius;      ///< In Port
  double portALen;         ///< In Port
  double portAThick;       ///< In Port

  double portBXStep;       ///< Out Port
  double portBZStep;       ///< Out Port
  double portBRadius;      ///< Out Port
  double portBLen;         ///< Out Port
  double portBThick;       ///< Out Port
  
  void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

  virtual void applyPortRotation();
  
 public:

  PortTube(const std::string&);
  PortTube(const PortTube&);
  PortTube& operator=(const PortTube&);
  virtual ~PortTube();

  using FixedComp::createAll;
  virtual void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
