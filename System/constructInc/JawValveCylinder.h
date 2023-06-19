/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/JawValveCylinder.h
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
#ifndef constructSystem_JawValveCylinder_h
#define constructSystem_JawValveCylinder_h

class Simulation;

namespace constructSystem
{
  
/*!
  \class JawValveCylinder
  \version 1.0
  \author S. Ansell
  \date January 2018
  \brief JawValveCylinder unit  
*/

class JawValveCylinder :
  public JawValveBase
{
 private:
  
  double innerRadius;           ///< Void radius
  
  void populate(const FuncDataBase&) override;
  void createSurfaces() override;
  void createObjects(Simulation&) override;

  
 public:

  JawValveCylinder(const std::string&);
  JawValveCylinder(const JawValveCylinder&);
  JawValveCylinder& operator=(const JawValveCylinder&);
  ~JawValveCylinder() override;


};

}

#endif
 
