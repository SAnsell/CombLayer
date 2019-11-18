/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeamInc/dipolePipe.h
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
#ifndef xraySystem_dipolePipe_h
#define xraySystem_dipolePipe_h

class Simulation;


namespace exampleSystem
{
/*!
  \class dipolePipe
  \version 1.0
  \author S. Ansell
  \date January 2019

  \brief dipolePipe for Max-IV
*/

class dipolePipe : public attachSystem::FixedRotate,
  public attachSystem::ContainedComp,
  public attachSystem::ExternalCut,
  public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 private:
  
  const std::string baseName;   ///< Base key

  double radius;                 ///< radius of centre line
  double width;                  ///< radius of centre line
  double height;                 ///< height of frame
  double angle;                  ///< rotation axis

  double outerHeight;            ///< rotation axis
  double outerWidth;             ///< rotation axis
  
  int outerMat;                     ///< coil material
  
  void populate(const FuncDataBase&);
  
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  dipolePipe(const std::string&);
  dipolePipe(const std::string&,const std::string&);
  dipolePipe(const dipolePipe&);
  dipolePipe& operator=(const dipolePipe&);
  virtual ~dipolePipe();

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
