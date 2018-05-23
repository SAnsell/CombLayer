/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   essBuildInc/Linac.h
 *
 * Copyright (c) 2016 by Konstantin Batkov
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
  class DTL;
  class TSW;

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

  double lengthBack;                ///< length backward the proton beam direction
  double lengthFront; ///< length towards the proton beam direction
  double widthLeft;             ///< Inner width towards x+
  double widthRight;            ///< inner width towards x-
  double height;                ///< Inner height
  double depth;                 ///< Inner depth

  double wallThick;             ///< Thickness of side walls
  double roofThick; ///< Roof thickness
  double floorThick; ///< Thickness of floor
  double floorWidthLeft; ///< floor width towards x+
  double floorWidthRight; ///< floor width towards x-
  size_t nAirLayers; ///< number of layers in the air of the tunnel (along the proton beam)

  int airMat;                    ///< air material
  int wallMat;                   ///< wall material
  int soilMat; ///< Soil material

  size_t nTSW;                   ///< Number of TSWs

  std::shared_ptr<BeamDump> beamDump; ///< linac 4 commissionning dump
  std::shared_ptr<FaradayCup> faradayCup; ///< Faraday Cup
  size_t nDTL; ///< number of DTL tanks
  std::vector<std::shared_ptr<DTL> > dtl; ///< array of DTL sections

  void layerProcess(Simulation& System, const std::string& cellName,
		    const long int& lpS, const long int& lsS, const size_t&, const int&);

  void createDTL(Simulation&, const long int);
  void buildTSW(Simulation& system) const;

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


