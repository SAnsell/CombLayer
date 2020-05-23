/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/pikInc/PIKReflector.h
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
#ifndef pikSystem_PIKReflector_h
#define pikSystem_PIKReflector_h

class Simulation;

namespace pikSystem
{

/*!
  \class PIKReflector
  \version 1.0
  \author Konstantin Batkov
  \date May 2020
  \brief PIK reactor reflector
*/

class PIKReflector : public attachSystem::ContainedComp,
		    public attachSystem::FixedRotate,
		    public attachSystem::CellMap
{
 private:

  double roofPitch;             ///< roof deviation from horizontal level [deg]
  double floorPitch;            ///< floor deviation from horizontal level [deg]
  double height;                ///< height between roof and floor
  double radius;                ///< reflector outer radius (obtained from setSide)

  int mat;                      ///< reflector material
  std::string bottom;           ///< bottom surface
  std::string top;              ///< top surface
  std::string side;             ///< side surface

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  PIKReflector(const std::string&);
  PIKReflector(const PIKReflector&);
  PIKReflector& operator=(const PIKReflector&);
  virtual PIKReflector* clone() const;
  virtual ~PIKReflector();

  void setBottom(const attachSystem::FixedComp&,const long int);
  void setTop(const attachSystem::FixedComp&,const long int);
  void setSide(const attachSystem::FixedComp&,const long int);
  void createAll(Simulation&,const attachSystem::FixedComp&,const long int);

};

}

#endif
