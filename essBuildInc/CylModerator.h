/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   essBuildInc/CylModerator.h
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#ifndef essSystem_CylModerator_h
#define essSystem_CylModerator_h

class Simulation;

namespace essSystem
{

/*!
  \class CylModerator
  \author S. Ansell
  \version 1.0
  \date October 2012
  \brief Specialized for a cylinder moderator
*/

class CylModerator : public essMod
{
 protected:
  
  const int modIndex;             ///< Index of surface offset
  int cellIndex;                  ///< Cell index

  double xStep;                   ///< X step
  double yStep;                   ///< Y step
  double zStep;                   ///< Z step
  double xyAngle;                 ///< xy rotation angle
  double zAngle;                  ///< z rotation angle

  size_t nLayers;                     ///< number of layers
  std::vector<double> radius;         ///< cylinder radii
  std::vector<double> height;         ///< Full heights
  std::vector<int> mat;               ///< Materials
  std::vector<double> temp;           ///< Temperatures

  // Functions:

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  CylModerator(const std::string&);
  CylModerator(const CylModerator&);
  CylModerator& operator=(const CylModerator&);
  virtual ~CylModerator();
  virtual CylModerator* clone() const;
  
  /// Accessor to the main H2 body
  virtual int getMainBody() const { return modIndex+1; }

  virtual Geometry::Vec3D getSurfacePoint(const size_t,const size_t) const;
  virtual int getLayerSurf(const size_t,const size_t) const;
  virtual std::string getLayerString(const size_t,const size_t) const;

  
  void createAll(Simulation&,const attachSystem::FixedComp&);
  
};

}

#endif
 
