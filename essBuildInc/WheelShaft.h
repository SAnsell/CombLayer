/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   essBuildInc/WheelShaft.h
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
#ifndef essSystem_WheelShaft_h
#define essSystem_WheelShaft_h

class Simulation;

namespace essSystem
{

/*!
  \class WheelShaft
  \author S. Ansell
  \version 1.0
  \date October 2012
  \brief Specialized for a cylinder object
*/

class WheelShaft : public attachSystem::ContainedComp,
    public attachSystem::FixedComp
{
 private:
  
  const int shaftIndex;               ///< Index of surface offset
  int cellIndex;                      ///< Cell index

  double height; 
  size_t nLayers;                     ///< number of layers
  double mainRadius;                  ///< Main radius
  int mainMat;                        ///< Main material
  
  std::vector<double> Thick;         ///< cylinder radii
  std::vector<int> mat;               ///< Materials

  // Functions:

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  WheelShaft(const std::string&);
  WheelShaft(const WheelShaft&);
  WheelShaft& operator=(const WheelShaft&);
  virtual ~WheelShaft();

  void createAll(Simulation&,const attachSystem::FixedComp&);
  
};

}

#endif
 
