/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/MidWaterDivider.h
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
#ifndef essSystem_MidWaterDivider_h
#define essSystem_MidWaterDivider_h

class Simulation;


namespace essSystem
{

/*!
  \class MidWaterDivider
  \version 1.0
  \author S. Ansell
  \date April 2015 
  \brief MidWaterDivider component in the butterfly moderator
*/

class MidWaterDivider : 
  public attachSystem::ContainedComp,
  public attachSystem::LayerComp,
  public attachSystem::FixedComp
{
 private:

  const std::string baseName; ///< Base Name
      
  const int divIndex;       ///< Index of surface offset
  int cellIndex;            ///< Cell index

  size_t cutLayer;          ///< Cut layer into wings
  double midYStep;          ///< Step of centre point
  double midAngle;          ///< Step of centre point

  double length;            ///< Inner length
  double height;            ///< Inner height
  double topThick;          ///< Roof thickness
  double baseThick;         ///< Base thickness
  double wallThick;         ///< Wall thickness
  double cornerRadius;      ///< Corner radius
  
  int modMat;               ///< Water material
  int wallMat;              ///< Wall material
  double modTemp;           ///< Moderator temperature

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&);

  void createSurfaces();
  void createObjects(Simulation&,const H2Wing&,const H2Wing&);
  void cutOuterWing(Simulation&,const H2Wing&,const H2Wing&) const;
  void createLinks(const H2Wing&,const H2Wing&);

 public:

  MidWaterDivider(const std::string&,const std::string&);
  MidWaterDivider(const MidWaterDivider&);
  MidWaterDivider& operator=(const MidWaterDivider&);
  virtual MidWaterDivider* clone() const;
  virtual ~MidWaterDivider();

  virtual Geometry::Vec3D getSurfacePoint(const size_t,const long int) const;
  virtual std::string getLayerString(const size_t,const long int) const;
  virtual int getLayerSurf(const size_t,const long int) const;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const H2Wing&,const H2Wing&);
};

}

#endif
 
