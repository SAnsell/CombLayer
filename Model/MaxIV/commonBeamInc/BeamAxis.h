/*********************************************************************
  CombLayer : MCNP(X) Input builder

  * File:   Model/MaxIV/commonBeamInc/BeamAxis.h
  *
  * Copyright (c) 2004-2025 by Konstantin Batkov
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

#ifndef constructSystem_BeamAxis_h
#define constructSystem_BeamAxis_h

namespace constructSystem {
  /*!
    \class BeamAxis
    \version 1.0
    \author Konstantin Batkov
    \date May 2025
    \brief Beam axis setter (e.g. for the YAG and other screens)
  */

  class BeamAxis
  {
  protected:
    std::unique_ptr<Geometry::Line> beamAxis;
  public:
    BeamAxis();
    BeamAxis(const BeamAxis&);
    BeamAxis& operator=(const BeamAxis&);
    void setBeamAxis(const attachSystem::FixedComp&,const long int);
    void setBeamAxis(const Geometry::Vec3D&,const Geometry::Vec3D&);
  };
}
#endif
