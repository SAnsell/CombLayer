/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   d4cModelInc/AreaDetector.h
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#ifndef instrumentSystem_AreaDetector_h
#define instrumentSystem_AreaDetector_h

class Simulation;


namespace saxsSystem
{

/*!
  \class AreaDetector
  \author S. Ansell
  \version 1.0
  \date March 2019
  \brief Grid of detector objects 
*/

class AreaDetector : public attachSystem::ContainedComp,
    public attachSystem::FixedOffset
{
 private:
  
  double centRadius;            ///< Radius of from centre
  double tubeRadius;            ///< Radius of detector
  double wallThick;             ///< Outer wall thickness
  double height;                ///< Height/depth
  int wallMat;                  ///< Wall material
  int detMat;                   ///< Detector material

  size_t nDet;                  ///< Number of detectors
  double initAngle;             ///< inital angle [deg]
  double finalAngle;            ///< final angle [deg]
  std::vector<Geometry::Vec3D> DPoints;  ///< Centre points

  // Functions:

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  AreaDetector(const std::string&);
  AreaDetector(const AreaDetector&);
  AreaDetector& operator=(const AreaDetector&);
  virtual ~AreaDetector();

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);
  
};

}

#endif
 
