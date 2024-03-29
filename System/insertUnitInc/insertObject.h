/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   insertUnitInc/insertObject.h
 *
 * Copyright (c) 2004-2023 by Stuart Ansell
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
#ifndef insertSystem_insertObject_h
#define insertSystem_insertObject_h

class Simulation;

namespace insertSystem
{
/*!
  \class insertObject
  \version 1.0
  \author S. Ansell
  \date November 2011
  \brief Plate inserted in object 
  
  Designed to be a quick plate to put an object into a model
  for fluxes/tallies etc
*/

class insertObject :
    public attachSystem::FixedRotate,
    public attachSystem::ContainedComp,
    public attachSystem::CellMap,
    public attachSystem::SurfMap,
    public attachSystem::FrontBackCut
{
 protected:

  const std::string baseName;   ///< Base key name [if used]
  
  int populated;          ///< externally set values
  
  int defMat;             ///< Material
  bool delayInsert;       ///< Delay insertion         
  
  void populate(const FuncDataBase&) override;
  virtual void findObjects(Simulation&);

  using FixedRotate::createUnitVector;
  
 public:

  insertObject(const std::string&);
  insertObject(std::string ,const std::string&);
  insertObject(const insertObject&);
  insertObject& operator=(const insertObject&);
  ~insertObject() override;

  /// set delay flag
  void setNoInsert() { delayInsert=1; }

  /// Set the material
  void setMat(const int M) { defMat=M; }
  
  void setStep(const double,const double,const double);
  void setStep(const Geometry::Vec3D&);
  void setAngles(const double,const double);

  using FixedComp::createAll;
  void createAll(Simulation&,const FixedComp&,
			 const long int) override =0;
};

}

#endif
 
