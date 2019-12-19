/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/softimaxInc/BremOpticsColl.h
 *
 * Copyright (c) 2019 by Konstantin Batkov
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
#ifndef xraySystem_BremOpticsColl_h
#define xraySystem_BremOpticsColl_h

class Simulation;

namespace xraySystem
{

/*!
  \class BremOpticsColl
  \version 1.0
  \author Konstantin Batkov
  \date 14 Nov 2019
  \brief Bremsstrahlung collimator in the optics hutch
*/

class BremOpticsColl :
    public attachSystem::ContainedComp,
    public attachSystem::FixedOffset,
    public attachSystem::CellMap,
    public attachSystem::SurfMap,
    public attachSystem::FrontBackCut
{
 private:

  double length;                ///< Total length including void
  double extWidth;                 ///< External part Width
  double extHeight;                ///< External part Height
  double wallThick;             ///< Wall thickness
  double holeXStep; ///< X-offset of the hole
  double holeZStep; ///< Z-offset of the hole
  double holeWidth; ///< width of the hole
  double holeHeight; ///< height of the hole

  double colYStep; ///< absorber y offset
  double colLength; ///< absorber length
  double colRadius; ///< absorber radius
  int extActive; ///< true if external part should be built
  double extXStep; ///< x offset of external part
  double extZStep; ///< z offset of external part

  double innerRadius;          ///< Inner radius of hole
  double flangeARadius;        ///< Flange A outer radius
  double flangeAInnerRadius;   ///< Flange A inner radius
  double flangeALength;        ///< Flange A length
  double flangeBRadius;        ///< Flange B outer radius
  double flangeBInnerRadius;        ///< Flange B outer radius
  double flangeBLength;        ///< Flange B length

  int voidMat;                  ///< Main material
  int wallMat;                  ///< Wall material
  int colMat; ///< absorber material

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  BremOpticsColl(const std::string&);
  BremOpticsColl(const BremOpticsColl&);
  BremOpticsColl& operator=(const BremOpticsColl&);
  virtual BremOpticsColl* clone() const;
  virtual ~BremOpticsColl();

  void createAll(Simulation&,const attachSystem::FixedComp&,const long int);

};

}

#endif
