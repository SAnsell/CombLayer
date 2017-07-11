/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/DiskLayerMod.h
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
#ifndef essSystem_DiskLayerMod_h
#define essSystem_DiskLayerMod_h

class Simulation;

namespace essSystem
{
  class CylFlowGuide;
/*!
  \class DiskLayerMod
  \author S. Ansell
  \version 1.0
  \date May 2015
  \brief Specialized for a cylinder pre-mod under moderator
*/

class DiskLayerMod : public attachSystem::ContainedComp,
    public attachSystem::LayerComp,
    public attachSystem::FixedComp,
    public attachSystem::CellMap,
    public attachSystem::SurfMap
{
 private:
  
  const int modIndex;             ///< Index of surface offset
  int cellIndex;                  ///< Cell index

  size_t midIndex;                ///< Mid index
  double midZ;                    ///< step in Z
  double zStep;                   ///< Step away from target
  double outerRadius;             ///< Outer radius of Be Zone
  
  std::vector<double> thick;          ///< Full thickness [additive]
  /// cylinder radii [additive] for each layer
  std::vector<std::vector<double>> radius;  
  std::vector<std::vector<int>> mat;               ///< Materials 
  std::vector<std::vector<double>> temp;           ///< Temperatures

  void populate(const FuncDataBase&,const double,const double);
  void createUnitVector(const attachSystem::FixedComp&,const long int,
			const bool);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  DiskLayerMod(const std::string&);
  DiskLayerMod(const DiskLayerMod&);
  DiskLayerMod& operator=(const DiskLayerMod&);
  virtual DiskLayerMod* clone() const;
  virtual ~DiskLayerMod();

  virtual Geometry::Vec3D getSurfacePoint(const size_t,const long int) const;
  virtual int getLayerSurf(const size_t,const long int) const;
  virtual std::string getLayerString(const size_t,const long int) const;

  /// total height of object
  double getHeight() const;

  /// Step on object
  double getZOffset() const { return zStep; }

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int,const bool,const double,const double);

};

}

#endif
 
