/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   softimaxInc/M1Connectors.h
 *
 * Copyright (c) 2004-2023 by Stuart Ansell
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
#ifndef xraySystem_M1Connectors_h
#define xraySystem_M1Connectors_h

class Simulation;

namespace xraySystem
{

/*!
  \class M1Connectors
  \author S. Ansell
  \version 1.0
  \date May 2023
  \brief connectors between mirror and stuff
*/

class M1Connectors :
  public attachSystem::FixedRotate,
  public attachSystem::ContainedComp,
  public attachSystem::ExternalCut,
  public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 private:

  double clipYStep;         ///< Step from end
  double clipLen;           ///< Length of clip
  double clipSiThick;       ///< Clip thickness parallel to Si
  double clipAlThick;       ///< Clip thickness parallel to Al
  double clipExtent;        ///< Clip length away from back

  int clipMat;              ///< Material
  int voidMat;              ///< outer pipe material

  // Functions:

  void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  M1Connectors(const std::string&);
  M1Connectors(const M1Connectors&);
  M1Connectors& operator=(const M1Connectors&);
  virtual ~M1Connectors();

  using FixedComp::createAll;
  void createAll(Simulation&,
		 const attachSystem::FixedComp&,
		 const long int);
  
};

}

#endif
 
