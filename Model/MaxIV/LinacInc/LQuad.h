/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeamInc/LQuad.h
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#ifndef xraySystem_LQuad_h
#define xraySystem_LQuad_h

class Simulation;


namespace xraySystem
{
/*!
  \class LQuad
  \version 1.0
  \author S. Ansell
  \date January 2019

  \brief LQuad for Max-IV
*/

class LQuad : public attachSystem::FixedRotate,
  public attachSystem::ContainedComp,
  public attachSystem::ExternalCut,
  public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 private:

  const size_t NPole; 
  const std::string baseName;   ///< Base key
  
  double length;                ///< frame length

  double frameRadius;           ///< Gap to start of frame
  double frameThick;            ///< Thikckness of frame

  double poleYAngle;            ///< Rotation of +X Pole about Y
  double poleGap;               ///< Gap from centre point
  double poleRadius;            ///< Radius of pole piece cut
  double poleWidth;             ///< width [in rotated frame]

  double coilRadius;            ///< Radius of coil start
  double coilWidth;             ///< Cross width of coil
  
  int poleMat;                     ///< pole piece of magnet
  int coreMat;                     ///< core of magnet 
  int coilMat;                     ///< coil material
  int frameMat;                    ///< Iron material

  void populate(const FuncDataBase&);  
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  LQuad(const std::string&);
  LQuad(const std::string&,const std::string&);
  LQuad(const LQuad&);
  LQuad& operator=(const LQuad&);
  virtual ~LQuad();

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
