/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/BeamShutter.h
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
#ifndef constructSystem_BeamShutter_h
#define constructSystem_BeamShutter_h

class Simulation;

namespace constructSystem
{
  /*!
    \class BeamShutter
    \version 1.0
    \author S. Ansell
    \date February 2017
    \brief Movable shutter [keeps link pt fixed]
  */
  
class BeamShutter : public attachSystem::ContainedComp,
  public attachSystem::FixedOffsetGroup,
  public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 private:

  const int collIndex;          ///< Hole index
  int cellIndex;                ///< Cell index

  double liftZStep;             ///< Lift value

  double width;                 ///< width of beamstop
  double height;                ///< height of beamstop
  double length;                ///< thickness of beamstoop

  size_t nLayers;               ///< Number of layers
  std::vector<double> Thick;    ///< Thickness of materials
  std::vector<int> Mat;         ///< Materials in layers

  double surroundThick;         ///< Surround thickness
  double topVoid;               ///< Top void space

  int surroundMat;              ///< Surround material

  
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:
  
  BeamShutter(const std::string&);
  BeamShutter(const BeamShutter&);
  BeamShutter& operator=(const BeamShutter&);
  virtual ~BeamShutter() {}  ///< Destructor

  void populate(const FuncDataBase&);
  
  void setInner(const HeadRule&);
  void setInnerExclude(const HeadRule&);
  void setOuter(const HeadRule&);

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);
  
};

}

#endif
 
