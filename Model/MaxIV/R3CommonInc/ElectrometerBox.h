/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/commonBeam/ElectrometerBox.cxx
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
#ifndef xraySystem_ElectrometerBox_h
#define xraySystem_ElectrometerBox_h

class Simulation;

namespace xraySystem
{

/*!
  \class ElectrometerBox
  \version 1.0
  \author Stuart Ansell
  \date December 2021
  \brief Electrometer box for R3 ring
*/

class ElectrometerBox :
  public attachSystem::FixedRotate,
  public attachSystem::ContainedComp,
  public attachSystem::ExternalCut,
  public attachSystem::CellMap
{
 private:

  double elecWidth;             ///< full width
  double elecLength;            ///< full length
  double elecHeight;            ///< full height
  double voidSpace;             ///< drop of electromenter in box

  double frontThick;        ///< front lead thickness
  double backThick;         ///< side lead thickness
  double sideThick;         ///< side lead thickness
  
  double skinThick;         ///< skin thickness x 2

  int voidMat;              ///< void material
  int elecMat;              ///< electrometer material
  int skinMat;             ///< skin material material
  int wallMat;             ///< wall material

  void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  ElectrometerBox(const std::string&);
  ElectrometerBox(const ElectrometerBox&);
  ElectrometerBox& operator=(const ElectrometerBox&);
  virtual ~ElectrometerBox();

  using FixedComp::createAll;
  virtual void createAll(Simulation&,
			 const attachSystem::FixedComp&,
			 const long int);

};

}

#endif
