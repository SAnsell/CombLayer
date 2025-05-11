/*********************************************************************
  CombLayer : MCNP(X) Input builder

  * File:   Model/MaxIV/commonBeamInc/PowerFilter.h
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
#ifndef xraySystem_PowerFilter_h
#define xraySystem_PowerFilter_h

class Simulation;

namespace xraySystem
{

  /*!
    \class PowerFilter
    \version 1.0
    \author Konstantin Batkov
    \date May 2025
    \brief Power filter
  */

  class PowerFilter : public attachSystem::ContainedComp,
		      public attachSystem::FixedRotate,
		      public attachSystem::CellMap,
		      public attachSystem::SurfMap,
		      public constructSystem::BeamAxis
  {
  private:
    // TomoWISE TDR, figure 8.2
    double maxLength;             ///< parameter b
    double baseLength;            ///< parameter a
    double wedgeAngle;            ///< angle of the wedge
    double width;                 ///< Width
    double height;                ///< parameter d
    double baseHeight;            ///< parameter c
    double filterZOffset;         ///< z-offset of the single filter blade

    int mat;                      ///< filter material

    void populate(const FuncDataBase&);
    void createSurfaces();
    void createObjects(Simulation&);
    void createLinks();

  public:

    PowerFilter(const std::string&);
    PowerFilter(const PowerFilter&);
    PowerFilter& operator=(const PowerFilter&);
    virtual PowerFilter* clone() const;
    virtual ~PowerFilter();

    using attachSystem::FixedComp::createAll;
    void createAll(Simulation&,const attachSystem::FixedComp&,const long int);

  };

}

#endif
