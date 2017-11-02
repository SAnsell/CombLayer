/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   essBuildInc/Linac.h
 *
 * Copyright (c) 2004-2017 by Konstantin Batkov
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
#ifndef essSystem_Linac_h
#define essSystem_Linac_h

class Simulation;

namespace essSystem
{

  class BeamDump;
  class FaradayCup;

  /*!
    \class Linac
    \version 1.0
    \author Konstantin Batkov
    \date January 2017
    \brief Linac building and container for the linac-related components
  */

class Linac : public attachSystem::ContainedComp,
  public attachSystem::FixedOffset,
  public attachSystem::CellMap
{
 private:

  const int surfIndex;             ///< Index of surface offset
  int cellIndex;                ///< Cell index

  int engActive;                ///< Engineering active flag

  double length;                ///< Total length including void
  double widthLeft;             ///< Inner width towards x+
  double widthRight;            ///< inner width towards x-
  double height;                ///< Inner height
  double depth;                 ///< Inner depth

  double wallThick;             ///< Thickness of side walls
  double roofThick;             ///< Roof thickness
  double floorThick;            ///< Thickness of floor
  double floorWidthLeft;        ///< floor width towards x+
  double floorWidthRight;       ///< floor width towards x-
  /// number of layers in the air of the tunnel (along the proton beam)
  size_t  nAirLayers;   

  int airMat;                    ///< air material
  int wallMat;                   ///< wall material

  double tswLength;              ///< Temporary shielding wall length
  double tswWidth;               ///< Temporary shielding wall width
  double tswGap;                 ///< Distance between Temporary shielding walls
  double tswOffsetY;             ///< TSW location on the Y-axis
  size_t tswNLayers;             ///< number of layers in a TSW wall

  std::shared_ptr<BeamDump> beamDump; ///< linac 4 commissionning dump
  std::shared_ptr<FaradayCup> faradayCup; ///< Faraday Cup

  void layerProcess(Simulation&,const std::string&,
		    const long int, const long int,
		    const size_t, const int);

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);

  void createSurfaces();
  void createObjects(Simulation&);

  void createLinks();

 public:

  Linac(const std::string&);
  Linac(const Linac&);
  Linac& operator=(const Linac&);
  virtual ~Linac();

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif


