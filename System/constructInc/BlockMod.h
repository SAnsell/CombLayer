/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/BlockMod.h
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
#ifndef constructSystem_BlockMod_h
#define constructSystem_BlockMod_h

class Simulation;

namespace constructSystem
{

  class WedgeInsert;

/*!
  \class BlockMod
  \author S. Ansell
  \version 1.0
  \date November 2014
  \brief Specialized for a cylinder moderator
*/

class BlockMod : public constructSystem::ModBase
{
 private:
  
  typedef std::shared_ptr<WedgeInsert> WTYPE;

  size_t nLayers;                     ///< Number of layers
  std::vector<double> depth;          ///< Depth along Y direction 
  std::vector<double> width;          ///< Depth across X 
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

  BlockMod(const std::string&);
  BlockMod(const BlockMod&);
  BlockMod& operator=(const BlockMod&);
  virtual ~BlockMod();
  virtual BlockMod* clone() const;
  
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
 
