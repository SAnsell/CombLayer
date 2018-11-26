/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   maxpeemInc/WallLead.h
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#ifndef xraySystem_WallLead_h
#define xraySystem_WallLead_h

class Simulation;

namespace xraySystem
{
  
/*!
  \class WallLead
  \version 1.0
  \author S. Ansell
  \date May 2018
  \brief Bremsstralung Collimator unit  
*/

class WallLead :
  public attachSystem::FixedOffset,
  public attachSystem::ContainedComp,
  public attachSystem::CellMap,
  public attachSystem::FrontBackCut
{
 private:


  double frontLength;          ///< Front length
  double backLength;           ///< Back length

  double frontWidth;           ///< Front width of hole
  double frontHeight;          ///< Front height of hole

  double backWidth;           ///< Front width of hole
  double backHeight;          ///< Front height of hole

  double voidRadius;          ///< Radius in middle
  
  int voidMat;                ///< void material
  int midMat;                ///< void material
  int wallMat;                ///< main material

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const long int);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  
 public:

  WallLead(const std::string&);
  WallLead(const WallLead&);
  WallLead& operator=(const WallLead&);
  virtual ~WallLead();

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
