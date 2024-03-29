/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/TubeDetBox.h
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
#ifndef constructSystem_TubeDetBox_h
#define constructSystem_TubeDetBox_h

class Simulation;

namespace constructSystem
{

/*!
  \class TubeDetBox
  \author S. Ansell
  \version 1.0
  \date January 2013
  \brief Grid of detector objects 
*/

class TubeDetBox :
    public attachSystem::FixedRotate,
    public attachSystem::ContainedComp,
    public attachSystem::CellMap,
    public attachSystem::SurfMap
{
 private:

  const std::string baseName;  ///< Base name
  const size_t ID;             ///< ID Number

  size_t nDet;                  ///< Number of detectors
  
  double centRadius;            ///< Radius of from centre
  double tubeRadius;            ///< Radius of detector
  double wallThick;             ///< Outer wall thickness
  double height;                ///< Height/depth

  double gap;                    ///< Gap thickness
  double outerThick;             ///< Wall thickness

  int detMat;                   ///< Detector material
  int wallMat;                  ///< Wall material
  int outerMat;                 ///< Outer material
  int filterMat;                 ///< Outer material


  std::vector<Geometry::Vec3D> DPoints;  ///< Centre points

  // Functions:

  void populate(const FuncDataBase&) override;
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  TubeDetBox(const std::string&,const size_t);
  TubeDetBox(const TubeDetBox&);
  TubeDetBox& operator=(const TubeDetBox&);
  ~TubeDetBox() override;

  using FixedComp::createAll;
  void createAll(Simulation&,
		 const attachSystem::FixedComp&,
		 const long int) override;
  
};

}

#endif
 
