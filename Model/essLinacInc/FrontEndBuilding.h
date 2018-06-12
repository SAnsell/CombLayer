/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   essBuildInc/FrontEndBuilding.h
 *
 * Copyright (c) 2018 by Konstantin Batkov
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
#ifndef essSystem_FrontEndBuilding_h
#define essSystem_FrontEndBuilding_h

class Simulation;

namespace essSystem
{

/*!
  \class FrontEndBuilding
  \version 1.0
  \author Konstantin Batkov
  \date 28 May 2018
  \brief Front end building
*/

class FrontEndBuilding : public attachSystem::ContainedComp,
  public attachSystem::FixedOffset,
  public attachSystem::CellMap
{
 private:

  const int surfIndex;             ///< Index of surface offset
  int cellIndex;                ///< Cell index

  double length;                ///< Total length including void
  double widthLeft;             ///< Width left x-
  double widthRight; ///< Width right x+

  double wallThick;             ///< Thickness of wall
  double shieldWall1Offset; ///< Shield wall 1 offset
  double shieldWall1Thick;  ///< Shield wall 1 thick
  double shieldWall1Length; ///< Shield wall 1 length
  double shieldWall2Offset; ///< Shield wall 2 offset
  double shieldWall2Thick;  ///< Shield wall 2 thick
  double shieldWall2Length; ///< Shield wall 2 length
  double shieldWall3Offset; ///< Shield wall 3 offset
  double shieldWall3Thick;  ///< Shield wall 3 thickness
  double shieldWall3Length; ///< Shield wall 3 length
  double dropHatchLength;   ///< Drop Hatch length
  double dropHatchWidth;    ///< Drop Hatch width
  double dropHatchWallThick;///< Drop Hatch wall thick

  int mainMat;                   ///< main material
  int wallMat;                   ///< wall material

  int gapMat; ///< Penetration material
  double gapALength; ///< Penetration A length
  double gapBLength; ///< Penetration B length

  void layerProcess(Simulation& System, const std::string& cellName,
		    const long int& lpS, const long int& lsS,
		    const std::vector<double>&,
		    const std::vector<int>&);

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);

  void createSurfaces();
  void createObjects(Simulation&,const attachSystem::FixedComp&,
		     const long int,const long int,
		     const long int,const long int);
  void createLinks(const attachSystem::FixedComp&,
		     const long int,const long int,
		     const long int,const long int);

 public:

  FrontEndBuilding(const std::string&);
  FrontEndBuilding(const FrontEndBuilding&);
  FrontEndBuilding& operator=(const FrontEndBuilding&);
  virtual FrontEndBuilding* clone() const;
  virtual ~FrontEndBuilding();

  void createAll(Simulation&,const attachSystem::FixedComp&,const long int,
		 const long int,const long int,
		 const long int,const long int);

};

}

#endif


