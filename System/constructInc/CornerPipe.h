/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   constructInc/CornerPipe.h
 *
 * Copyright (c) 2004-2022 by Stuart Ansell
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
#ifndef constructSystem_CornerPipe_h
#define constructSystem_CornerPipe_h

class Simulation;

namespace constructSystem
{

/*!
  \class CornerPipe
  \version 1.0
  \author S. Ansell
  \date July 2015
  \brief CornerPipe unit
*/

class CornerPipe :
  public attachSystem::FixedRotate,
  public attachSystem::ContainedGroup,
  public attachSystem::CellMap,
  public attachSystem::SurfMap,
  public attachSystem::FrontBackCut
{
 private:

  double cornerRadius;          ///< void corner [inner]
  double height;                ///< void height [inner]
  double width;                 ///< void width [inner]

  double length;                ///< void length [total]

  double wallThick;               ///< pipe thickness

  double flangeARadius;          ///< Joining Flange radius [-ve for rect]
  double flangeALength;          ///< Joining Flange length

  double flangeBRadius;          ///< Joining Flange radius [-ve for rect]
  double flangeBLength;          ///< Joining Flange length


  int voidMat;                   ///< Void material
  int wallMat;                   ///< Pipe material
  int flangeMat;                 ///< Flange material

  int outerVoid;                 ///< Flag to build the outer void cell between flanges

  void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  CornerPipe(const std::string&);
  CornerPipe(const CornerPipe&);
  CornerPipe& operator=(const CornerPipe&);
  virtual ~CornerPipe();

  using FixedComp::createAll;
  virtual void createAll(Simulation&,const attachSystem::FixedComp&,
			 const long int);

};

}

#endif
