/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/LinacInc/DipoleDIBMag.h
 *
 * Copyright (c) 2004-2020 by Konstantin Batkov
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
#ifndef xraySystem_DipoleDIBMag_h
#define xraySystem_DipoleDIBMag_h

class Simulation;

namespace xraySystem
{

/*!
  \class DipoleDIBMag
  \version 1.0
  \author Konstantin Batkov
  \date Apr 2020
  \brief DIB Dipole magnet
*/

class DipoleDIBMag :
    public attachSystem::FixedRotate,
    public attachSystem::ContainedComp,
    public attachSystem::CellMap,
    public attachSystem::SurfMap,
    public attachSystem::ExternalCut
{
 private:

  double magOffset;             ///< Magnet offset from pipe centre
  double magHeight;             ///< Magnet thickness
  double magWidth;              ///< Magnet full width
  double magLength;             ///< Magnet full length
  double magInnerWidth;         ///< Magnet inner width
  double frameHeight;           ///< Frame full height
  double frameWidth;            ///< Frame full width
  double frameLength;           ///< Frame full length

  int voidMat;                  ///< Void material
  int coilMat;                  ///< Coil material
  int frameMat; ///< Frame material

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  DipoleDIBMag(const std::string&);
  DipoleDIBMag(const DipoleDIBMag&);
  DipoleDIBMag& operator=(const DipoleDIBMag&);
  virtual DipoleDIBMag* clone() const;
  virtual ~DipoleDIBMag();

  void createAll(Simulation&,const attachSystem::FixedComp&,const long int);

};

}

#endif
