/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/CylMod.h
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
#ifndef constructSystem_CylMod_h
#define constructSystem_CylMod_h

class Simulation;

namespace constructSystem
{

  class WedgeInsert;

/*!
  \class CylMod
  \author S. Ansell
  \version 1.0
  \date October 2012
  \brief Specialized for a cylinder moderator
*/

class CylMod : public constructSystem::ModBase
{
 private:

  /// type for wedge insert
  typedef std::shared_ptr<WedgeInsert> WTYPE;

  std::vector<double> radius;         ///< cylinder radii
  std::vector<double> height;         ///< Full heights
  std::vector<int> mat;               ///< Materials
  std::vector<double> temp;           ///< Temperatures

  size_t nConic;                     ///< Number of conic segments
  std::vector<ConicInfo> Conics;     ///< Conics [non-intersecting]
  size_t nWedge;                     ///< Number of Wedge segments
  std::vector<WTYPE> Wedges;         ///< Wedges [non-intersecting]

  int mainCell;                      ///< Main cell
  // Functions:

  void populate(const FuncDataBase&);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  void createWedges(Simulation&);
  
 public:

  CylMod(const std::string&);
  CylMod(const CylMod&);
  CylMod& operator=(const CylMod&);
  virtual ~CylMod();
  virtual CylMod* clone() const;
  
  /// Accessor to the main H2 body
  virtual int getMainBody() const { return modIndex+1; }

  virtual Geometry::Vec3D getSurfacePoint(const size_t,const long int) const;
  virtual int getLayerSurf(const size_t,const long int) const;
  virtual int getCommonSurf(const long int) const;
  virtual std::string getLayerString(const size_t,const long int) const;

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const attachSystem::FixedComp*,
		 const long int);
  
};

}

#endif
 
