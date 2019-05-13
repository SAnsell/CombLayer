/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/cosaxsInc/DiffPumpXIADP03.h
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
#ifndef constructSystem_DiffPumpXIADP03_h
#define constructSystem_DiffPumpXIADP03_h

class Simulation;

namespace constructSystem
{

/*!
  \class DiffPumpXIADP03
  \version 1.0
  \author Konstantin Batkov
  \date 3 May 2019
  \brief DP-03 differential pump by XIA
         https://www.xia.com/differential_pump.html
*/

class DiffPumpXIADP03 :
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
  double flangeVoidWidth; ///< Flange void width
  double flangeVoidHeight; ///< Flange void height
  double flangeVoidThick; ///< Flange void thickness
  int magnetMat; ///< Magnet material
  double magnetWidth; ///< Magnet width
  double magnetLength; ///< Magnet length
  double magnetThick; ///< Magnet thickness
  double magnetGapThick; ///< Thickness of gap between pump body and magnet

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  DiffPumpXIADP03(const std::string&);
  DiffPumpXIADP03(const DiffPumpXIADP03&);
  DiffPumpXIADP03& operator=(const DiffPumpXIADP03&);
  virtual DiffPumpXIADP03* clone() const;
  virtual ~DiffPumpXIADP03();

  void createAll(Simulation&,const attachSystem::FixedComp&,const long int);

};

}

#endif


