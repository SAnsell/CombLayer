/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   chipInc/LeadPlate.h
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#ifndef shutterSystem_LeadPlate_h
#define shutterSystem_LeadPlate_h

class Simulation;

namespace constructSystem
{
  class tubeUnit;
}

namespace shutterSystem
{

/*!
  \class LeadPlate
  \version 1.0
  \author S. Ansell
  \date January 2011
  \brief Plate inserted in object 
*/

class LeadPlate : public attachSystem::ContainedComp,
    public attachSystem::FixedOffset
{
 private:
  
  const int leadIndex;      ///< Index of surface offset
  int cellIndex;            ///< Cell index
  int activeFlag;           ///< Variables populated

  double thick;             ///< Full Thickness

  int defMat;               ///< Material
  int supportMat;               ///< Material
  Geometry::Vec3D Centre;   ///< Centre of block
  double radius;            ///< Hole radius
  double linerThick;        ///< Hole liner radius [if any]
  double centSpc;           ///< Hole-Hole centre spacing

  std::vector<constructSystem::tubeUnit*> HoleCentre;
  void clearHoleCentre();
  void copyHoleCentre(const std::vector<constructSystem::tubeUnit*>&);
  int checkCorners(const Geometry::Plane*,
		   const Geometry::Vec3D&) const;
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);

  void createSurfaces();
  void createObjects(Simulation&);
  void layerProcess(Simulation&);

  void calcCentre();
  void createCentres(const Geometry::Plane*);
  void joinHoles();

 public:

  LeadPlate(const std::string&);
  LeadPlate(const LeadPlate&);
  LeadPlate& operator=(const LeadPlate&);
  ~LeadPlate();

  void printHoles() const;
  /// Ugly set centre
  void setCentre(const Geometry::Vec3D& C) { Centre=C; }
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
