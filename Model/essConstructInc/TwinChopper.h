/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/TwinChopper.h
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#ifndef constructSystem_TwinChopper_h
#define constructSystem_TwinChopper_h

class Simulation;

namespace constructSystem
{
  class boxPort;
/*!
  \class TwinChopper
  \version 1.0
  \author S. Ansell
  \date January 2017
  \brief TwinChopper unit  
  
  This piece aligns away from the chopper axis. Using
  the chopper origin [bearing position]
*/

class TwinChopper : public TwinBase
{
 private:
  
  std::shared_ptr<boltRing> frontFlange;   ///< Front flange
  std::shared_ptr<boltRing> backFlange;    ///< Back flange
  std::shared_ptr<InnerPort> IPA;          ///< inner port
  std::shared_ptr<InnerPort> IPB;          ///< inner port

  
  void buildPorts(Simulation&);
  
 public:

  TwinChopper(const std::string&);
  TwinChopper(const TwinChopper&);
  TwinChopper& operator=(const TwinChopper&);
  virtual ~TwinChopper();

  virtual void createAll(Simulation&,const attachSystem::FixedComp&,
			 const long int);

};

}

#endif
 
