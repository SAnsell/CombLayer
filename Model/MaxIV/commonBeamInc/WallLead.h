/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeamInc/WallLead.h
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
  \brief Extra wall lead/concrete/lead + steel/lead front

  The lead wall is a lead/concrete/lead unit with a beampipe
  through
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

  double steelOutWidth;       ///< Thickness of steel wall side
  double steelRingWidth;      ///< Thickness of steel ring side
  double steelHeight;         ///< Height of steel [up]
  double steelDepth;          ///< Height of steel [down]
  double steelThick;          ///< Depth of steel
  double steelXCut;           ///< Central hole size
  double steelZCut;           ///< Central hole size 

  double extraLeadOutWidth;   ///< Width of lead wall side
  double extraLeadRingWidth;  ///< Width of lead ring side
  double extraLeadHeight;     ///< Height of lead
  double extraLeadDepth;      ///< Depth of lead
  double extraLeadXCut;       ///< Cut out
  
  double voidRadius;          ///< Radius in middle
  
  int voidMat;                 ///< void material
  int midMat;                  ///< mid material
  int wallMat;                 ///< main lead material
  int steelMat;                ///< steel material
  
  void populate(const FuncDataBase&);
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
 
