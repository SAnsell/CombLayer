/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonBeamInc/DiffPumpXIADP03.h
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
#ifndef constructSystem_DiffPumpXIADP03_h
#define constructSystem_DiffPumpXIADP03_h

class Simulation;

namespace xraySystem
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
  public attachSystem::FixedRotate,
  public attachSystem::CellMap,
  public attachSystem::SurfMap,
  public attachSystem::ExternalCut
{
 private:

  double length;                ///< Total length including void
  double width;                 ///< Width
  double height;                ///< Height
  double apertureHeight;        ///< Vertical dimension of aperture
  double apertureWidth;         ///< Horizontal dimension of aperture

  double flangeRadius;        ///< Flange radius
  double flangeThick;         ///< Flange thickness
  double flangeVoidWidth;     ///< Flange void width
  double flangeVoidHeight;    ///< Flange void height
  double flangeVoidThick;     ///< Flange void thickness

  double magnetWidth;         ///< Magnet width
  double magnetLength;        ///< Magnet length
  double magnetThick;         ///< Magnet thickness
  double magnetGap;           ///< Thickness of gap between pump body and magnet

  int mat;                   ///< Material
  int magnetMat;             ///< Magnet material
  int flangeMat;             ///< Flange material

  void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  DiffPumpXIADP03(const std::string&);
  DiffPumpXIADP03(const DiffPumpXIADP03&);
  DiffPumpXIADP03& operator=(const DiffPumpXIADP03&);
  virtual ~DiffPumpXIADP03();

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif


