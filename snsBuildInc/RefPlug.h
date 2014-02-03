/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   snsBuildInc/RefPlug.h
*
 * Copyright (c) 2004-2014 by Stuart Ansell
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNSNS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. 
 *
 ****************************************************************************/
#ifndef snsSystem_RefPlug_h
#define snsSystem_RefPlug_h

class Simulation;

namespace snsSystem
{

/*!
  \class RefPlug
  \author S. Ansell
  \version 1.0
  \date October 2012
  \brief Specialized for a cylinder reflector
*/

class RefPlug : public attachSystem::ContainedComp,
    public attachSystem::LayerComp,
    public attachSystem::FixedComp
{
 private:
  
  const int refIndex;             ///< Index of surface offset
  int cellIndex;                  ///< Cell index

  double xStep;                   ///< X step
  double yStep;                   ///< Y step
  double zStep;                   ///< Z step
  double xyAngle;                 ///< xy rotation angle
  double zAngle;                  ///< z rotation angle
  double height;                  ///< Be height
  double depth;                  ///< Be depth

  std::vector<double> radius;         ///< cylinder radii  
  std::vector<double> temp;           ///< Temperatures
  std::vector<int> mat;               ///< Materials

  // Functions:

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  RefPlug(const std::string&);
  RefPlug(const RefPlug&);
  RefPlug& operator=(const RefPlug&);
  virtual ~RefPlug();
  virtual RefPlug* clone() const;
  

  virtual Geometry::Vec3D getSurfacePoint(const size_t,const size_t) const;
  virtual int getLayerSurf(const size_t,const size_t) const;
  virtual std::string getLayerString(const size_t,const size_t) const;
  virtual int getCommonSurf(const size_t) const;
  
  void createAll(Simulation&,const attachSystem::FixedComp&);
  
};

}

#endif
 
