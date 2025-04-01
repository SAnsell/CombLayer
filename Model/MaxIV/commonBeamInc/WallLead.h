/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonBeamInc/WallLead.h
 *
 * Copyright (c) 2004-2025 by Stuart Ansell / Konstantin Batkov
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
  \version 2.0
  \author S. Ansell / K. Batkov
  \date Apr 2025
  \brief Ratchet-end wall extra shielding structure

  The lead wall is a lead/concrete/lead unit at the front end - optics
  hutch interface inside ratchet end wall.
*/

class WallLead :
  public attachSystem::FixedRotate,
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
  double steelHeight;         ///< Height of steel [up]
  double steelDepth;          ///< Height of steel [down]
  double steelThick;          ///< Depth of steel
  double steelXCut;           ///< Central hole size
  double steelZCut;           ///< Central hole size

  double extraLeadOutWidth;   ///< Width of lead wall side
  double extraLeadHeight;     ///< Height of lead
  double extraLeadDepth;      ///< Depth of lead
  double extraLeadThick;        ///< extra lead thickness
  double extraLeadXCut;       ///< Cut out
  double preLeadVoidThick;    ///< void tickness in front of lead layer

  double voidRadius;          ///< Radius in middle

  int voidMat;                 ///< void material
  int midMat;                  ///< mid material
  int wallMat;                 ///< main lead material
  int steelMat;                ///< steel material

  void populate(const FuncDataBase&) override;
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  WallLead(const std::string&);
  WallLead(const WallLead&);
  WallLead& operator=(const WallLead&);
  ~WallLead() override;

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int) override;

};

}

#endif
