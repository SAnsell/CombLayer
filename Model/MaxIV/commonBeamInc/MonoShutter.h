/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeamInc/MonoShutter.h
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
#ifndef xraySystem_MonoShutter_h
#define xraySystem_MonoShutter_h

class Simulation;

namespace xraySystem
{

  class ShutterUnit;
/*!
  \class MonoShutter
  \author S. Ansell
  \version 1.0
  \date February 2019
  \brief Monochromatic shutter system 
*/

class MonoShutter :
  public attachSystem::ContainedGroup,
  public attachSystem::FixedOffset,
  public attachSystem::ExternalCut,
  public attachSystem::SurfMap,
  public attachSystem::CellMap
{
 private:

  double divideBStep;   ///< B divider step
  double divideThick;   ///< Thickness of divider
  double divideRadius;  ///< Inner hole in divider
  
  int dMat;             ///< Divider material
  
  /// Shutter pipe
  std::shared_ptr<constructSystem::PortTube> shutterPipe;
  /// shutter to stop beam
  std::shared_ptr<xraySystem::ShutterUnit> monoShutterA;
  /// shutter to stop beam [second for redundency
  std::shared_ptr<xraySystem::ShutterUnit> monoShutterB;
  // Functions:

  void populate(const FuncDataBase&);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  
  void buildComponents(Simulation&);
  
 public:

  MonoShutter(const std::string&);
  MonoShutter(const MonoShutter&);
  MonoShutter& operator=(const MonoShutter&);
  virtual ~MonoShutter();

  using FixedComp::createAll;
  virtual void createAll(Simulation&,const attachSystem::FixedComp&,
			 const long int);
  
};

}

#endif
 
