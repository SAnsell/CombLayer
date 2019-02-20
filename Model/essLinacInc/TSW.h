/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   essBuildInc/TSW.h
 *
 * Copyright (c) 2017-2018 by Konstantin Batkov
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
#ifndef essSystem_TSW_h
#define essSystem_TSW_h

class Simulation;

namespace essSystem
{

/*!
  \class TSW
  \version 1.1
  \author Konstantin Batkov
  \date 31 Aug 2017
  \brief Temporary shielding wall
*/

class TSW : public attachSystem::ContainedComp,
  public attachSystem::FixedOffset,
  public attachSystem::CellMap
{
 private:

  const std::string baseName;   ///< Base name
  const size_t Index;           ///< Wall index

  double width;                 ///< Width
  size_t nLayers;               ///< Number of layers for variance reduction

  int wallMat;                  ///< Wall material
  int airMat;                   ///< Air material

  int doorMat; ///< Door material
  double doorGap; ///< Gap size around the door
  double doorOffset; ///< Door offset along the wall
  double doorHeightHigh; ///< Door high height
  double doorWidthHigh; ///< Width of the wide part
  double doorThickWide; ///< Thickness of the wide part
  double doorThickHigh; ///< Thickness of the high part
  double doorHeightLow; ///< Height of the low door part
  double doorWidthLow; ///< Width of the narrow part
  double doorLowGapWidth; ///< width of the door lower gap
  double doorLowGapHeight; ///< height of the door lower gap

  // rectangular penetration above the door
  double hole1StepY; ///< Y-offset of hole 1
  double hole1StepZ; ///< Z-offset of hole 1
  double hole1Width; ///< width of hole 1
  double hole1Height; ///< height of hole 1

  double hole2StepY; ///< Y-offset of hole 2
  double hole2StepZ; ///< Z-offset of hole 2
  double hole2Radius; ///< Radius of hole 2

  double hole3StepY; ///< Y-offset of hole 3
  double hole3StepZ; ///< Z-offset of hole 3
  double hole3Radius; ///< Radius of hole 3

  double hole4StepY; ///< Y-offset of hole 4
  double hole4StepZ; ///< Z-offset of hole 4
  double hole4Width; ///< width of hole 4
  double hole4Height; ///< height of hole 4

  double hole5StepY; ///< Y-offset of hole 2
  double hole5StepZ; ///< Z-offset of hole 2
  double hole5Radius; ///< Radius of hole 2

  double hole6StepY; ///< Y-offset of hole 2
  double hole6StepZ; ///< Z-offset of hole 2
  double hole6Radius; ///< Radius of hole 2

  double hole7StepZ; ///< Z-offset of hole 7
  double hole7Width; ///< width of hole 7
  double hole7Height; ///< height of hole 7

  double hole8StepZ; ///< Z-offset of hole 8
  double hole8Width; ///< width of hole 8
  double hole8Height; ///< height of hole 8

  void layerProcess(Simulation& System, const std::string& cellName,
		    const long int& lpS, const long int& lsS, const size_t&,
		    const int&);

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);

  void createSurfaces(const attachSystem::FixedComp&,
		      const long int,const long int,const long int);
  void createObjects(Simulation&,const attachSystem::FixedComp&,
		     const long int,const long int,
		     const long int,const long int);
  void createLinks(const attachSystem::FixedComp&,
		   const long int,const long int,
		   const long int,const long int);

 public:

  TSW(const std::string&,const std::string&,const size_t&);
  TSW(const TSW&);
  TSW& operator=(const TSW&);
  virtual TSW* clone() const;
  virtual ~TSW();

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int,const long int,
		 const long int,const long int);

};

}

#endif


