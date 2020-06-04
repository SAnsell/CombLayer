/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   LinacInc/CeramicSep.h
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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
#ifndef tdcSystem_CeramicSep_h
#define tdcSystem_CeramicSep_h

class Simulation;


namespace tdcSystem
{
/*!
  \class CeramicSep
  \version 1.0
  \author S. Ansell
  \date June 2020

  \brief Ceramic Separator in a beamline
*/

class CeramicSep :
  public attachSystem::FixedOffset,
  public attachSystem::ContainedComp,
  public attachSystem::FrontBackCut,
  public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 private:

  double radius;                ///< void radius
  double length;                ///< void length [total]

  double ceramicALen;           ///< Length after flange
  double ceramicWideLen;        ///< Wide length
  double ceramicBLen;           ///< Length before bellow

  double ceramicThick;          ///< Small wall thick
  double ceramicWideThick;      ///< Wide ceramic extra

  double pipeLen;               ///< Steel connecter after ceramic
  double pipeThick;             ///< Pipe thickness

  double bellowLen;             ///< Bellow length
  double bellowThick;           ///< Bellow thickness
  
  double flangeARadius;         ///< Joining Flange radius
  double flangeALength;         ///< Joining Flange length

  double flangeBRadius;         ///< Joining Flange radius
  double flangeBLength;         ///< Joining Flange length

  int voidMat;                  ///< void material
  int pipeMat;                  ///< main pipe material
  int ceramicMat;               ///< cermaic material
  int bellowMat;                ///< bellow material
  int flangeMat;                ///< flange material
  int outerMat;                 ///< outer material

  void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  CeramicSep(const std::string&);
  CeramicSep(const std::string&,const std::string&);
  CeramicSep(const CeramicSep&);
  CeramicSep& operator=(const CeramicSep&);
  virtual ~CeramicSep();

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
