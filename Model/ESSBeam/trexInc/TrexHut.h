/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   trexInc/TrexHut.h
 *
 * Copyright (c) 2004-2017 by Tsitohaina Randiamalala & Stuart Ansell
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

#ifndef essSystem_TrexHut_h
#define essSystem_TrexHut_h

class Simulation;

namespace essSystem
{
/*!
  \class TrexHut
  \version 1.0
  \author T. Randriamalala
  \date March 2017
  \brief SkadiHut unit  
*/
class TrexHut:
  public attachSystem::FixedRotate,
  public attachSystem::ContainedComp,
  public attachSystem::ExternalCut,
  public attachSystem::CellMap
{
 private:
  
  double voidHeight;            ///< void height [top only]
  double voidWidth;             ///< void width [total]
  double voidDepth;             ///< void depth [low only]
  double voidLength;            ///< void length [total]

  double L1Front;               ///< Front extension
  double L1LeftWall;            ///< Left wall
  double L1RightWall;           ///< Right wall
  double L1Roof;                ///< Roof
  double L1Floor;               ///< Token floor depth
  double L1Back;                ///< Back steel extension

  double L2Front;               ///< Front extension
  double L2LeftWall;            ///< Left wall
  double L2RightWall;           ///< Right wall
  double L2Roof;                ///< Roof
  double L2Floor;               ///< Token floor depth
  double L2Back;                ///< Back length

  double L3Front;               ///< Front extension
  double L3LeftWall;            ///< Left wall
  double L3RightWall;           ///< Right wall
  double L3Roof;                ///< Roof
  double L3Floor;               ///< Token floor depth
  double L3Back;                ///< Back length
  
  int L1Mat;                  ///< First layer material 
  int L2Mat;                  ///< Second layer material
  int L3Mat;                  ///< Third layer material
  
  void populate(const FuncDataBase&) override;
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  TrexHut(const std::string&);
  TrexHut(const TrexHut&);
  TrexHut& operator=(const TrexHut&);
  ~TrexHut() override;

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int) override;

};

}//End namespace
#endif
