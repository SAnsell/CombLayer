/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   essBuildInc/Box.h
 *
 * Copyright (c) 2017 by Konstantin Batkov
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
#ifndef essSystem_Box_h
#define essSystem_Box_h

class Simulation;

namespace essSystem
{

/*!
  \class BoxShutter
  \version 1.0
  \author Konstantin Batkov
  \date 20 Jun 2017
  \brief BoxShutter with layers
*/

class BoxShutter : public attachSystem::ContainedComp,
  public attachSystem::FixedOffset,
  public attachSystem::LayerComp,
  public attachSystem::CellMap
{
 private:

  //  int engActive;                  ///< Engineering active flag

  size_t nSlices;                ///< Number of different material layers
  std::vector<size_t> nLayers;   ///< Number of layers of each material for variance reduction
  std::vector<double> length;    ///< Lengths [additive]
  std::vector<int>    mat;       ///< Materials
  std::vector<double> temp;      ///< Temperatures
  /*
  std::vector<double> width;     ///< Widths  [additive]
  std::vector<double> height;    ///< Heights [additive]
  std::vector<double> depth;     ///< Depths [additive]
  */
  
  double width;     ///< Widths  [additive]
  double height;    ///< Heights [additive]
  double depth;     ///< Depths [additive]
  
  std::string sideRule; ///< Side rule

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  BoxShutter(const std::string&);
  BoxShutter(const BoxShutter&);
  BoxShutter& operator=(const BoxShutter&);
  virtual ~BoxShutter();

  virtual int getLayerSurf(const size_t,const long int) const;
  virtual std::string getLayerString(const size_t,const long int) const;
  virtual Geometry::Vec3D getSurfacePoint(const size_t,const long int) const {};
  std::string getSideRule() const { return sideRule; }

  /// total height of object
  double getZOffset() const { return zStep; }
  // double getHeight() const
  //   { return (depth.empty()) ? 0.0 : depth.back()+height.back(); }
  void layerProcess(Simulation& );

  void createAll(Simulation&,const attachSystem::FixedComp&,const long int);

};

}

#endif


