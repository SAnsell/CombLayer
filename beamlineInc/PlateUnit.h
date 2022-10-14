/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   beamlineInc/PlateUnit.h
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
#ifndef beamlineSystem_PlateUnit_h
#define beamlineSystem_PlateUnit_h

namespace Geometry
{
  class Convex2D;
}


namespace beamlineSystem
{

/*!
  \class PlateUnit
  \version 1.0
  \author S. Ansell
  \date February 2014
  \brief Points associated with tracked beamline projections
*/

class PlateUnit :
    public GuideUnit
{
 private:

  std::vector<Geometry::Vec3D> APts;  ///< Points of front shape
  std::vector<Geometry::Vec3D> BPts;  ///< Points of back shape 

  Geometry::Convex2D frontCV;
  Geometry::Convex2D backCV;

  Geometry::Vec3D getFrontPt(const size_t,const double) const;
  Geometry::Vec3D getBackPt(const size_t,const double) const;

  
  virtual void populate(const FuncDataBase&);
  virtual void createSurfaces();
  virtual void createObjects(Simulation&);
  
 public:

  PlateUnit(const std::string&);
  PlateUnit(const PlateUnit&);
  PlateUnit& operator=(const PlateUnit&);
  virtual PlateUnit* clone() const;
  virtual ~PlateUnit();

  void setFrontPoints(const std::vector<Geometry::Vec3D>&);
  void setBackPoints(const std::vector<Geometry::Vec3D>&);

  using FixedComp::createAll;
  virtual void createAll(Simulation&,const FixedComp&,
			 const long int);
  
};


}

#endif
 
