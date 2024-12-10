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

  double slitLength;            ///< Slit length
  double slitWidth;             ///< Slit width
  double slitHeight;            ///< Slit height
  double wallThick;             ///< wall thickness
  double portRadius;            ///< port radius
  double frontPortLength;       ///< front port length (centre to flange end)
  double backPortLength;        ///< back port length (centre to flange end)
  double leftPortLength;        ///< left port length (centre to flange end)
  double rightPortLength;       ///< right port length (centre to flange end)
  double bottomPortLength;      ///< bottom port length (centre to flange end)
  double topPortLength;         ///< top port length (centre to flange end)

  double outerFlangeRadius;     ///< outer flange radius
  double outerFlangeThick;      ///< outer flange thickness
  double outerFlangeCapThick;   ///< outer flange cap thickness
  int leftFlangeCapWindowMat;   ///< left flange cap window material

  int slitsMat;                 ///< Slits material
  int wallMat;                  ///< wall material
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
