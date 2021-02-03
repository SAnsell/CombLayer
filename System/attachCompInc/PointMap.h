/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   attachCompInc/PointMap.h
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
#ifndef attachSystem_PointMap_h
#define attachSystem_PointMap_h

class Simulation;
class HeadRule;

namespace attachSystem
{
  class FixedComp;
  class ContainedComp;

/*!
  \class PointMap
  \version 1.0
  \author S. Ansell
  \date September 2020
  \brief Point mapping by name 
*/

class PointMap 
{
 private:

  ///  Lists
  typedef std::map<std::string,std::vector<Geometry::Vec3D>> LCTYPE;

  LCTYPE Items;
  
  void removeVecUnit(const std::string&,const int);

  std::string getFCKeyName() const;

 public:

  PointMap();         
  PointMap(const PointMap&);
  PointMap& operator=(const PointMap&);
  virtual ~PointMap() {}     ///< Destructor

  
  bool hasPoint(const std::string&,const size_t index =0) const;

  void setPoint(const std::string&,const Geometry::Vec3D&);
      
  void setPoint(const std::string&,const size_t,const Geometry::Vec3D&);

  void addPoint(const std::string&,const Geometry::Vec3D&);

  void addPoints(const std::string&,const std::vector<Geometry::Vec3D>&);
  
  const Geometry::Vec3D&
    getPoint(const std::string&) const;

  const Geometry::Vec3D&
    getPoint(const std::string&,const size_t) const;

  const Geometry::Vec3D&
    getLastPoint(const std::string&) const;

  size_t getNPoints(const std::string&) const;

  const std::vector<Geometry::Vec3D>&
    getPoints(const std::string&) const;

  std::vector<Geometry::Vec3D> getAllPoints() const;

  Geometry::Vec3D removePoint(const std::string&,
			      const size_t =0);

  Geometry::Vec3D
    getSignedPoint(const std::string&,const size_t) const;
  
};

}

#endif
 
