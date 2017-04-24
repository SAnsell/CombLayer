/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   d4cModelInc/DetectorArray.h
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
#ifndef instrumentSystem_DetectorArray_h
#define instrumentSystem_DetectorArray_h

class Simulation;

/*!
  \namespace d4cSystem
  \version 1.0
  \author S. Ansell
  \date April 2013
  \brief Model component for simulating D4C
*/

namespace d4cSystem
{

/*!
  \class DetectorArray
  \author S. Ansell
  \version 1.0
  \date January 2013
  \brief Grid of detector objects 
*/

class DetectorArray : public attachSystem::ContainedComp,
    public attachSystem::FixedComp
{
 private:
  
  const int detIndex;            ///< Index of surface offset
  int cellIndex;                  ///< Cell index

  double xStep;                   ///< X step
  double yStep;                   ///< Y step
  double zStep;                   ///< Z step
  double xyAngle;
  double zAngle;

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
  void createUnitVector(const attachSystem::FixedComp&);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  DetectorArray(const std::string&);
  DetectorArray(const DetectorArray&);
  DetectorArray& operator=(const DetectorArray&);
  virtual ~DetectorArray();

  void createTally(Simulation&) const;
  void createAll(Simulation&,const attachSystem::FixedComp&);
  
};

}

#endif
 
