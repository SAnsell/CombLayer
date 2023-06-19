/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   essBuildInc/DiskPreMod.h
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
#ifndef essSystem_DiskPreMod_h
#define essSystem_DiskPreMod_h

class Simulation;

namespace essSystem
{
  class CylFlowGuide;
  class OnionCooling;
/*!
  \class DiskPreMod
  \author S. Ansell
  \version 1.0
  \date May 2015
  \brief Specialized for a cylinder pre-mod under moderator
*/

class DiskPreMod :
  public attachSystem::FixedRotate,
  public attachSystem::ContainedComp,
  public attachSystem::LayerComp,
  public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 private:

  double outerRadius;             ///< Outer radius of Be Zone

  std::vector<double> radius;         ///< cylinder radii [additive]
  std::vector<double> height;         ///< Full heights [additive]
  std::vector<double> depth;          ///< full depths [additive]
  std::vector<double> width;          ///< Widths [additive]
  std::vector<int> mat;               ///< Materials
  std::vector<double> temp;           ///< Temperatures
  
  std::string flowGuideType; ///< cooling flow guide type

  size_t NWidth;                      ///< Number of widths active
  int engActive;                  ///< Engineering active flag

  /// Flow guide pattern inside DiskPreMod (engineering detail)
  std::shared_ptr<CylFlowGuide> InnerComp;
  std::shared_ptr<OnionCooling> onion;

  HeadRule sideRuleHR;

  void populate(const FuncDataBase&) override;
  void createUnitVector(const attachSystem::FixedComp&,const long int) override;

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  DiskPreMod(const std::string&);
  DiskPreMod(const DiskPreMod&);
  DiskPreMod& operator=(const DiskPreMod&);
  virtual DiskPreMod* clone() const;
  ~DiskPreMod() override;

  Geometry::Vec3D getSurfacePoint(const size_t,const long int) const override;
  HeadRule getLayerHR(const size_t,const long int) const override;

  // 
  HeadRule getSideRule() const { return sideRuleHR; }

  /// total height of object
  double getZOffset() const { return zStep; }
  double getHeight() const
    { return (depth.empty()) ? 0.0 : depth.back()+height.back(); }

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int) override;

};

}

#endif

