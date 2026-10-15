/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonBeamInc/ProjectionCollimator.h
 *
 * Copyright (c) 2026 by U. Friman-Gayer
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

#ifndef xraySystem_ProjectionCollimator_h
#define xraySystem_ProjectionCollimator_h

class Simulation;

namespace xraySystem {
/*!
  \class ProjectionCollimator
  \version 0.1
  \author U. Friman-Gayer
  \date October 2026
  \brief
*/

template <std::size_t NPoints, std::size_t NSegments>
class ProjectionCollimator : public attachSystem::ContainedComp,
                             public attachSystem::FixedRotate,
                             public attachSystem::FrontBackCut,
                             public attachSystem::CellMap,
                             public attachSystem::SurfMap {
private:
  std::array<std::array<double, NPoints>, NSegments + 1> apertureX;
  std::array<double, NSegments + 1> apertureY;
  std::array<std::array<double, NPoints>, NSegments + 1> apertureZ;

  double height;
  double width;

  int material;
  int voidMaterial;

  void createSurfaces();
  void createObjects(Simulation &);
  void createLinks();

public:
  ProjectionCollimator(const std::string &);
  ~ProjectionCollimator() override {}

  void populate(const FuncDataBase &) override;

  void initializePoints(
      std::array<std::array<Geometry::Vec3D, NPoints + 1>, NSegments + 1> &);

  void createAll(Simulation &, const attachSystem::FixedComp &,
                 const long int) override;
};

} // namespace xraySystem

#endif
