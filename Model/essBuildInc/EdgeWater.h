/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/EdgeWater.h
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
#ifndef essSystem_EdgeWater_h
#define essSystem_EdgeWater_h

class Simulation;


namespace essSystem
{

/*!
  \class EdgeWater
  \version 1.0
  \author S. Ansell
  \date April 2015 
  \brief EdgeWater component in the butterfly moderator
*/

class EdgeWater : 
    public attachSystem::FixedComp,
    public attachSystem::ContainedComp,
    public attachSystem::LayerComp,
    public attachSystem::CellMap,
    public attachSystem::ExternalCut
{
 private:

  double width;             ///< Full width
  double wallThick;         ///< Thickness for walls

  double cutAngle;    ///<  Angle cut away from H2 surface
  double cutWidth;    ///< Water thickness at its connection to the H2-lobe

  std::string sideRule;      ///< Side rule ????
    
  int modMat;               ///< Water material
  int wallMat;              ///< Wall material
  double modTemp;           ///< Moderator temperature
  
  void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  EdgeWater(const std::string&);
  EdgeWater(const EdgeWater&);
  EdgeWater& operator=(const EdgeWater&);
  virtual EdgeWater* clone() const;
  ~EdgeWater() override;
  
  Geometry::Vec3D getSurfacePoint(const size_t,const long int) const override;
  HeadRule getLayerHR(const size_t,const long int) const override;

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int) override;
};

}

#endif
 
