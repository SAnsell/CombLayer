/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   sourceInc/GammaSource.h
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. 
 *
 ****************************************************************************/
#ifndef GammaSource_h
#define GammaSource_h

namespace SDef
{
  class Source;
}

namespace SDef
{

/*!
  \class GammaSource
  \version 1.0
  \author S. Ansell
  \date November 2014
  \brief Adds gamma ray circular divergent source
*/

class GammaSource : 
  public attachSystem::FixedComp
{
 private:
  
  double xStep;                 ///< Step in X
  double yStep;                 ///< Step in Y
  double zStep;                 ///< Step in Z

  double cutEnergy;             ///< Energy cut point
  double radius;
  double angleSpread;           ///< Angle spread

  Geometry::Vec3D FocusPoint;   ///< Focus point
  Geometry::Vec3D Direction;    ///< Beam direction

  double weight;
  std::vector<double> Energy;   ///< Energies [MeV]
  std::vector<double> EWeight;  ///< Weights
  
  void populate(const FuncDataBase& Control);
  void createUnitVector(const attachSystem::FixedComp&);
  void calcPosition();
  void createSource(SDef::Source&) const;

 public:

  GammaSource();
  GammaSource(const GammaSource&);
  GammaSource& operator=(const GammaSource&);
  ~GammaSource();

  /// Set cut energy
  void setCutEnergy(const double E) { cutEnergy=E; }
  void loadEnergy(const std::string&);

  void createAll(const FuncDataBase&,SDef::Source&);
  
};

}

#endif
 
