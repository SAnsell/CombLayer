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

  // rectangular penetration above the door
  double hole1StepY; ///< Y-offset of hole 1
  double hole1StepZ; ///< Z-offset of hole 1
  double hole1Width; ///< width of hole 1
  double hole1Height; ///< height of hole 1

  double hole2StepY; ///< Y-offset of hole 2
  double hole2StepZ; ///< Z-offset of hole 2
  double hole2Radius; ///< Radius of hole 2

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


