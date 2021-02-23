/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonBeamInc/ConnectorTube.h
 *
 * Copyright (c) 2019-2021 by Konstantin Batkov
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
#ifndef xraySystem_ConnectorTube_h
#define xraySystem_ConnectorTube_h

class Simulation;

namespace xraySystem
{

/*!
  \class ConnectorTube
  \version 1.0
  \author Stuart Ansell
  \date February 2021
  \brief Connector tube to diff pumpe
*/

class ConnectorTube :
  public attachSystem::ContainedComp,
  public attachSystem::FixedRotate,
  public attachSystem::CellMap,
  public attachSystem::SurfMap,
  public attachSystem::ExternalCut
{
 private:

  double radius;                ///< radius of main pipe
  double length;                ///< Total length including void
  double wallThick;             ///< wall thickness
 
  double innerRadius;           ///< Radius of inner tube
  double innerLength;           ///< Length of inner tube

  double outerRadius;           ///< outer shield radius
  double outerLength;           ///< outer shield length

  double flangeInner;           ///< Flange inner radius
  double flangeRadius;          ///< Flange radius
  double flangeLength;          ///< Flange thickness

  int voidMat;                  ///< Void Material
  int wallMat;                  ///< Material
  int flangeMat;                ///< Flange material

  void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  ConnectorTube(const std::string&);
  ConnectorTube(const ConnectorTube&);
  ConnectorTube& operator=(const ConnectorTube&);
  virtual ~ConnectorTube();

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif


