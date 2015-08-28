/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/DetectorTank.h
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#ifndef essSystem_DetectorTank_h
#define essSystem_DetectorTank_h

class Simulation;

namespace essSystem
{

/*!
  \class DetectorTank
  \version 1.0
  \author S. Ansell
  \date July 2015
  \brief DetectorTank [insert object]
*/

class DetectorTank : public attachSystem::ContainedComp,
  public attachSystem::FixedOffset,
  public attachSystem::CellMap
{
 private:
  
  const int tankIndex;          ///< Index of surface offset
  int cellIndex;                ///< Cell index

  double innerRadius;           ///< Radius of sample area
  double outerRadius;           ///< outer detector radius
  double midAngle;              ///< dividing angle
  double height;                ///< Height of tank

  double innerThick;            ///< Thickness of inner zone
  double frontThick;            ///< Thickness of front wall
  double backThick;             ///< Side thickness
  double roofThick;             ///< roof/base thick

  int wallMat;                  ///< Material for walls

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);

  void createSurfaces();
  void createLinks();
  void createObjects(Simulation&);

 public:

  DetectorTank(const std::string&);
  DetectorTank(const DetectorTank&);
  DetectorTank& operator=(const DetectorTank&);
  ~DetectorTank();

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
