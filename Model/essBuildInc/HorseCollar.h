/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/HorseCollar.h
 *
 * Copyright (c) 2017-2018 by Jimmy Scionti
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>. 
 *
 ****************************************************************************/
#ifndef essSystem_HorseCollar_h
#define essSystem_HorseCollar_h

class Simulation;

namespace essSystem
{

/*!
  \class HorseCollar
  \version 1.0
  \author S. Ansell
  \date January 2018
  \brief Bulk around Reflector
*/

class HorseCollar : public attachSystem::ContainedComp,
                    public attachSystem::FixedOffset,
                    public attachSystem::CellMap
{
 private:
     
  double length;                ///< Length
  double internalRadius;        ///< Void radius
  double mainRadius;            ///< Main radius
  
  int mainMat;                  ///< Main material
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const long int);
  
  void createSurfaces();
  void createLinks();
  void createObjects(Simulation&);
  
  
 public:
  
  HorseCollar(const std::string&);
  HorseCollar(const HorseCollar&);
  HorseCollar& operator=(const HorseCollar&);
  virtual ~HorseCollar();
  
  void createAll(Simulation&,const attachSystem::FixedComp&,const long int);
};

}

#endif
