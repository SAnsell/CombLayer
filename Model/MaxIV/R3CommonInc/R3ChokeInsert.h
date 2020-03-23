/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonBeamInc/R3ChokeInsert.h
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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
#ifndef xraySystem_R3ChokeInsert_h
#define xraySystem_R3ChokeInsert_h

class Simulation;

namespace xraySystem
{

/*!
  \class R3ChokeInsert
  \version 1.0
  \author S. Ansell
  \date January 2018
  \brief R3ChokeInsert unit
*/

class R3ChokeInsert :
  public attachSystem::FixedOffset,
  public attachSystem::ContainedComp,
  public attachSystem::CellMap,
  public attachSystem::SurfMap,
  public attachSystem::ExternalCut
{
 private:

  double plateThick;            ///< Depth
  double plateGap;              ///< gap between plates
  double plateDepth;              ///< gap between plates
  double plateLength;              ///< gap between plates

  int plateMat;                 ///< material for plates
  
  void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  R3ChokeInsert(const std::string&);
  R3ChokeInsert(const R3ChokeInsert&);
  R3ChokeInsert& operator=(const R3ChokeInsert&);
  virtual ~R3ChokeInsert();

  void setBladeCentre(const attachSystem::FixedComp&,const long int);
  void setBladeCentre(const Geometry::Vec3D&);

  using FixedComp::createAll;
  virtual void createAll(Simulation&,const attachSystem::FixedComp&,
			 const long int);

};

}

#endif
