/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/TaperedDiskPreMod.h
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#ifndef essSystem_TaperedDiskPreMod_h
#define essSystem_TaperedDiskPreMod_h

class Simulation;

namespace essSystem
{
  class CylFlowGuide;
/*!
  \class TaperedDiskPreMod
  \author S. Ansell, K. Batkov
  \version 1.0
  \date Aug 2015
  \brief Similar to DiskPreMod but with a possibility of tapering
*/

class TaperedDiskPreMod : public attachSystem::ContainedComp,
    public attachSystem::LayerComp,
    public attachSystem::FixedComp,
    public attachSystem::CellMap
{
 private:
  
  const int modIndex;             ///< Index of surface offset
  int cellIndex;                  ///< Cell index
  
  double zStep;                   ///< Step away from target
  double outerRadius;             ///< Outer radius of Be Zone
  
  std::vector<double> radius;         ///< cylinder radii [additive]
  std::vector<double> height;         ///< Full heights [additive]
  std::vector<double> depth;          ///< full depths [additive]
  std::vector<int> mat;               ///< Materials 
  std::vector<double> temp;           ///< Temperatures

  int engActive;                  ///< Engineering active flag
  /// Flow guide pattern inside TaperedDiskPreMod (engineering detail)
  std::shared_ptr<CylFlowGuide> InnerComp; 

  bool   tiltSide;                    ///< true ? top : bottom   side to be tilted
  double tiltAngle;                   ///< tapering angle
  double tiltRadius;                  ///< radius where tilting starts
  
  void populate(const FuncDataBase&,const double,const double);
  void createUnitVector(const attachSystem::FixedComp&,const long int,
			const bool);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  TaperedDiskPreMod(const std::string&);
  TaperedDiskPreMod(const TaperedDiskPreMod&);
  TaperedDiskPreMod& operator=(const TaperedDiskPreMod&);
  virtual TaperedDiskPreMod* clone() const;
  virtual ~TaperedDiskPreMod();

  virtual Geometry::Vec3D getSurfacePoint(const size_t,const size_t) const;
  virtual int getLayerSurf(const size_t,const size_t) const;
  virtual std::string getLayerString(const size_t,const size_t) const;

  /// total height of object
  double getZOffset() const { return zStep; }
  double getHeight() const
    { return (depth.empty()) ? 0.0 : depth.back()+height.back(); }

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int,const bool,const double,const double, const bool);
};

}

#endif
 
