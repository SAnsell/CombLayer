/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonBeamInc/RectangularCollimator.h
 *
 * Copyright (c) 2004-2026 by S. Ansell and U. Friman-Gayer
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

#ifndef xraySystem_RectangularCollimator_h
#define xraySystem_RectangularCollimator_h

class Simulation;

namespace xraySystem {
/*!
  \class RectangularCollimator
  \version 0.1
  \author U. Friman-Gayer
  \date October 2026
  \brief
*/

template <std::size_t N>
class RectangularCollimator : public attachSystem::ContainedComp,
                              public attachSystem::FixedRotate,
                              public attachSystem::FrontBackCut,
                              public attachSystem::CellMap,
                              public attachSystem::SurfMap {
private:
  std::array<std::pair<double, double>, N> apertureX;
  std::array<double, N> apertureY;
  std::array<std::pair<double, double>, N> apertureZ;

  double height;
  double width;

  int material;
  int voidMaterial;

  int planeIndex(const unsigned int n_segment, const unsigned int side) const;

  void createSurfaces();
  void createObjects(Simulation &);
  void createLinks();

public:
  RectangularCollimator(const std::string &);
  RectangularCollimator(const RectangularCollimator &);
  RectangularCollimator &operator=(const RectangularCollimator &);
  ~RectangularCollimator() override {}

  void populate(const FuncDataBase &) override;

  void createAll(Simulation &, const attachSystem::FixedComp &,
                 const long int) override;
};

} // namespace xraySystem

#endif
