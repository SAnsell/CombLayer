/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/LinkWrapper.h
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
#ifndef constructSystem_LinkWrapper_h
#define constructSystem_LinkWrapper_h

class Simulation;


namespace constructSystem
{

/*!
  \class LinkWrapper
  \version 1.0
  \author S. Ansell
  \date May 2012
  \brief TS1 t1Reflector [insert object]
*/

class LinkWrapper : public attachSystem::ContainedComp,
    public attachSystem::ExcludedComp,
    public attachSystem::FixedComp
{
 protected:
 
  const size_t InOutLinkB;      ///< Boundary between inside/outside links.

  const int refIndex;           ///< Index of surface offset
  int cellIndex;                ///< Cell index

  std::vector<int> surfNum;                 ///< Signed surface numbers
  std::vector<Geometry::Vec3D> surfCent;    ///< Relative to origin
  std::vector<Geometry::Vec3D> surfAxis;    ///< Axis in X:Y:Z
  std::vector<size_t> flag;                 ///< Flag for each component
  
  std::vector<int> surfEntryOrder;    ///< Surface type indexes

  size_t nLayers;                     ///< number of layers?
  std::vector<double> layerThick;     ///< Thickness of layers
  std::vector<int> layerMat;          ///< Layer material
  int defMat;                         ///< Default material
 
  std::map<size_t,size_t> mask;       ///< Mask to be applied 
  
  virtual void populate(const Simulation&);
  virtual void createUnitVector(const attachSystem::FixedComp&);

  ///\cond ABSTRACT
  virtual void createSurfaces() =0;
  virtual void createObjects(Simulation&) =0;
  ///\endcond ABSTRACT
  
  void processMask();
  bool sectorFlag(const size_t,const size_t) const;

 public:

  LinkWrapper(const std::string&);
  LinkWrapper(const LinkWrapper&);
  LinkWrapper& operator=(const LinkWrapper&);
  virtual ~LinkWrapper();

  void addSurface(const Geometry::Vec3D&,const Geometry::Vec3D&);
  void addSurface(const attachSystem::FixedComp&,const long int);
  void addSurface(const attachSystem::FixedComp&,std::string);
  void addSurface(const std::string&,const std::string&);
  void addSurface(const std::string&,const long int);
 
  /// Get Central cell [last built]
  int centralCell() const { return cellIndex-1; }
  std::string getComposite(const std::string&) const;

  void maskSection(const size_t);
  void maskSection(std::string);

  virtual void createAll(Simulation&,const attachSystem::FixedComp&);
};

}

#endif
 
