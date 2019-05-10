/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/cosaxsInc/cosaxsDiffPump.h
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
#ifndef xraySystem_cosaxsDiffPump_h
#define xraySystem_cosaxsDiffPump_h

class Simulation;

namespace xraySystem
{

/*!
  \class cosaxsDiffPump
  \version 1.0
  \author Konstantin Batkov
  \date 3 May 2019
  \brief CoSAXS differential pumping. B310A_E01_VAC_IPX02
*/

class cosaxsDiffPump :
    public attachSystem::ContainedComp,
    public attachSystem::FixedOffset,
    public attachSystem::CellMap,
    public attachSystem::SurfMap,
    public attachSystem::FrontBackCut
{
 private:

  double length;                ///< Total length including void
  double width;                 ///< Width
  double height;                ///< Height
  double apertureHeight;        ///< Vertical dimension of aperture
  double apertureWidth; ///< Horizontal dimension of aperture
  int    mat;                  ///< Material
  double flangeRadius; ///< Flange radius
  double flangeThick; ///< Flange thickness
  int    flangeMat; ///< Flange material

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  cosaxsDiffPump(const std::string&);
  cosaxsDiffPump(const cosaxsDiffPump&);
  cosaxsDiffPump& operator=(const cosaxsDiffPump&);
  virtual cosaxsDiffPump* clone() const;
  virtual ~cosaxsDiffPump();

  void createAll(Simulation&,const attachSystem::FixedComp&,const long int);

};

}

#endif


