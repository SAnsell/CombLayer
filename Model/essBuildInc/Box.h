/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   essBuildInc/Box.h
 *
 * Copyright (c) 2004-2022 by Konstantin Batkov
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
  \class Box
  \version 1.0
  \author Konstantin Batkov
  \date 20 Jun 2017
  \brief Box with layers
*/

class Box :
    public attachSystem::FixedRotate,
    public attachSystem::ContainedComp,
    public attachSystem::ExternalCut,
    public attachSystem::LayerComp,
    public attachSystem::CellMap
{
 private:

  std::vector<double> length;    ///< Lengths [additive]
  std::vector<double> width;     ///< Widths  [additive]
  std::vector<double> height;    ///< Heights [additive]
  std::vector<double> depth;     ///< Depths [additive]
  std::vector<int>    mat;       ///< Materials
  std::vector<double> temp;      ///< Temperatures
  
  HeadRule sideRuleHR; ///< Side rule

  void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  Box(const std::string&);
  Box(const Box&);
  Box& operator=(const Box&);
  virtual Box* clone() const;
  virtual ~Box();

  virtual HeadRule getLayerHR(const size_t,const long int) const;
  virtual Geometry::Vec3D getSurfacePoint(const size_t,const long int) const;

  const HeadRule& getSideRule() const { return sideRuleHR; }

  /// total height of object
  double getZOffset() const { return zStep; }
  double getHeight() const
    { return (depth.empty()) ? 0.0 : depth.back()+height.back(); }

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,const long int);

};

}

#endif


