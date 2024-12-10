/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/GunTestFacilityInc/SlitsMask.h
 *
 * Copyright (c) 2004-2024 by Konstantin Batkov
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
#ifndef xraySystem_SlitsMask_h
#define xraySystem_SlitsMask_h

class Simulation;

namespace xraySystem
{

/*!
  \class SlitsMask
  \version 2.0
  \author Konstantin Batkov
  \date October 2024
  \brief Slits mask
*/

class SlitsMask : public attachSystem::ContainedComp,
		  public attachSystem::FixedRotate,
		  public attachSystem::CellMap,
		  public attachSystem::SurfMap,
		  public attachSystem::FrontBackCut
{
 private:

  double length;                ///< Total length including void
  double width;                 ///< Width
  double height;                ///< Height
  double chamberLengthBack;     ///< chamber length back
  double chamberLengthFront;    ///< chamber length front
  double chamberDepth;          ///< chamber depth
  double chamberHeight;         ///< chamber height
  double chamberWidth;          ///< chamber width
  double chamberWallThick;      ///< chamber wall thickness
  double portRadius;            ///< port radius
  double portThick;             ///< port thick
  double frontLength;           ///< front length (centre to flange end)
  double backLength;            ///< back length (centre to flange end)
  double leftPortLength;        ///< left port total length
  double rightPortLength;       ///< right port total length
  double bottomPortLength;      ///< bottom port total length
  double topPortLength;         ///< top port total length

  int slitsMat;                 ///< Slits material
  int chamberMat;               ///< chamber wall material
  int voidMat;                  ///< void material

  void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  SlitsMask(const std::string&);
  SlitsMask(const SlitsMask&);
  SlitsMask& operator=(const SlitsMask&);
  virtual SlitsMask* clone() const;
  virtual ~SlitsMask();

  using attachSystem::FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,const long int);

};

}

#endif
