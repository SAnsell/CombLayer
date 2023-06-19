/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   constructInc/WindowPipe.h
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
#ifndef constructSystem_WindowPipe_h
#define constructSystem_WindowPipe_h

class Simulation;

namespace constructSystem
{

/*!
  \class WindowPipe
  \version 1.0
  \author S. Ansell
  \date July 2015
  \brief WindowPipe unit
*/

class WindowPipe :
  public attachSystem::FixedRotate,
  public attachSystem::ContainedGroup,
  public attachSystem::CellMap,
  public attachSystem::SurfMap,
  public attachSystem::FrontBackCut
{
 private:

  double radius;                ///< void radius [inner]
  double length;                ///< void length [total]

  double feThick;               ///< pipe thickness
  double claddingThick;         ///< cladding thickness

  double flangeARadius;          ///< Joining Flange radius [-ve for rect]
  double flangeALength;          ///< Joining Flange length

  double flangeBRadius;          ///< Joining Flange radius [-ve for rect]
  double flangeBLength;          ///< Joining Flange length

  unsigned int activeWindow;     ///< Flag for windows
  windowInfo windowA;           ///< Front window info
  windowInfo windowB;           ///< Back window info

  int voidMat;                  ///< Void material
  int feMat;                    ///< Pipe material
  int claddingMat;              ///< Pipe cladding material
  int flangeMat;                ///< Flange material

  int outerVoid;                ///< Flag to build the outer void cell between flanges

  void populate(const FuncDataBase&) override;
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

  void applyActiveFrontBack();

 public:

  WindowPipe(const std::string&);
  WindowPipe(const WindowPipe&);
  WindowPipe& operator=(const WindowPipe&);
  ~WindowPipe() override;

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
			 const long int) override;

};

}

#endif
