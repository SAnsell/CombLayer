/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   LinacInc/LSexupole.h
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
#ifndef tdcSystem_LSexupole_h
#define tdcSystem_LSexupole_h

class Simulation;


namespace tdcSystem
{
/*!
  \class LSexupole
  \version 1.0
  \author S. Ansell
  \date January 2019

  \brief LSexupole for Max-IV
*/

class LSexupole :
    public attachSystem::FixedRotate,
    public attachSystem::ContainedComp,
    public attachSystem::ExternalCut,
    public attachSystem::CellMap,
    public attachSystem::SurfMap
{
 private:

  const std::string baseName;   ///< Base key
  
  double length;                ///< frame length

  double frameRadius;           ///< Gap to start of frame
  double frameOuter;            ///< Thikckness of frame [full]

  double poleYAngle;            ///< Rotation of +X Pole about Y
  double poleGap;               ///< Gap from centre point
  double poleRadius;            ///< Radius of pole piece cut
  double poleWidth;             ///< width [in rotated frame]

  double coilRadius;            ///< Radius of coil start
  double coilWidth;             ///< Cross width of coil
  double coilEndExtra;          ///< Coil extra length [round]
  double coilEndRadius;         ///< Coil extra radius [round]
  
  int poleMat;                     ///<` pole piece of magnet
  int coreMat;                     ///< core of magnet 
  int coilMat;                     ///< coil material
  int frameMat;                    ///< Iron material

  void populate(const FuncDataBase&);  
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  LSexupole(const std::string&);
  LSexupole(const std::string&,const std::string&);
  LSexupole(const LSexupole&);
  LSexupole& operator=(const LSexupole&);
  virtual ~LSexupole();

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
