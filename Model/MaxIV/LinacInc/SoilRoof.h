/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File: LinacInc/SoilRoof.h
 *
 * Copyright (c) 2004-2022 by Stuart Ansell 
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
#ifndef tdcSystem_SoilRoof_h
#define tdcSystem_SoilRoof_h

class Simulation;

namespace tdcSystem
{

/*!
  \class SoilRoof
  \version 1.0
  \author S. Ansell 
  \date March 2022
  \brief Extra soil brem roof for the SPF hall.
  
  This class builds the soil roof for the SPF. It needs the 
  top of the concrete roof and the front/back of the SPF area for
  the brem.
*/

class SoilRoof :
  public attachSystem::FixedRotate,
  public attachSystem::ContainedComp,
  public attachSystem::CellMap,
  public attachSystem::SurfMap,
  public attachSystem::ExternalCut
{
 private:

  double height;             ///< full height in middel

  double frontLength;            ///< Optional front cut length
  double ringRadius;             ///< Synchrotron radius (outer building)
  Geometry::Vec3D ringCentre;    ///< Synchrotron centre
  
  double unitGap;                ///< size of gap to allow full surf integration

  int soilMat;                   ///< Earth material

  void layerProcess(Simulation&,const std::string&,
                    const int,const int,const size_t);

  void populate(const FuncDataBase&);
  virtual void createUnitVector(const attachSystem::FixedComp&,
				const long int);
    
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  SoilRoof(const std::string&);
  SoilRoof(const SoilRoof&);
  SoilRoof& operator=(const SoilRoof&);
  virtual ~SoilRoof() =default;

  using FixedComp::createAll;
  virtual void createAll(Simulation&,
		 const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
