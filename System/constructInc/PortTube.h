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

  bool portAOuterFlag;     ///< In Port void constructed  
  double portAXStep;       ///< In Port XStep		  
  double portAZStep;       ///< In Port YStep		  
  double portARadius;      ///< In Port Inner Radius	  
  double portALen;         ///< In Port Length inc flange 
  double portAThick;       ///< In Port Pipe thickness    

  bool portBOuterFlag;     ///< In Port  void constructed  
  double portBXStep;       ///< Out Port XStep		  
  double portBZStep;       ///< Out Port YStep		  
  double portBRadius;      ///< Out Port Inner Radius	  
  double portBLen;         ///< Out Port Length inc flange 
  double portBThick;       ///< Out Port Pipe thickness    
  
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
 
