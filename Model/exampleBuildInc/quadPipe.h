/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeamInc/quadPipe.h
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
#ifndef xraySystem_quadPipe_h
#define xraySystem_quadPipe_h

class Simulation;


namespace exampleSystem
{
/*!
  \class quadPipe
  \version 1.0
  \author S. Ansell
  \date January 2019

  \brief quadPipe for Max-IV
*/

class quadPipe : public attachSystem::FixedRotate,
  public attachSystem::ContainedComp,
  public attachSystem::ExternalCut,
  public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 private:
  
  const std::string baseName;   ///< Base key
  
  double width;                  ///< radius of centre line
  double height;                 ///< height of frame
  double length;                  ///< rotation axis
  
  double radiusX;                 ///< radius of centre line (X)
  double radiusZ;                 ///< radius of centre line (Z)
  
  int outerMat;                     ///< coil material
  
  void populate(const FuncDataBase&);
  
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  quadPipe(const std::string&);
  quadPipe(const std::string&,const std::string&);
  quadPipe(const quadPipe&);
  quadPipe& operator=(const quadPipe&);
  virtual ~quadPipe();

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
