/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   constructInc/GeneralPipe.h
 *
 * Copyright (c) 2004-2023 by Stuart Ansell
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
#ifndef constructSystem_GeneralPipe_h
#define constructSystem_GeneralPipe_h

class Simulation;

namespace constructSystem
{

  /*!
    \struct windowInfo
    \version 1.0
    \date July 2015
    \author S. Ansell
    \brief window or flange build data 
  */
  
struct windowInfo
{
  int type;               ///< type 0:none, 1: round 2 : rectangle
  double thick;           ///< Joining Flange length
  double radius;          ///< Window/flange radius
  double height;          ///< Window/flange Height
  double width;           ///< Window/flange Width
  int mat;                ///< Material
};


/*!
  \class GeneralPipe
  \version 1.0
  \author S. Ansell
  \date July 2022
  \brief Holds the general stuff for a pipe
*/
\
class GeneralPipe :
  public attachSystem::FixedRotate,
  public attachSystem::ContainedGroup,
  public attachSystem::CellMap,
  public attachSystem::SurfMap,
  public attachSystem::FrontBackCut
{
 protected:

  double radius;                ///< void radius [inner]
  double length;                ///< void length [total]

  double pipeThick;             ///< pipe thickness

  // window information
  windowInfo flangeA;           ///< Front window info
  windowInfo flangeB;           ///< Back window info

  // window information
  windowInfo windowA;           ///< Front window info
  windowInfo windowB;           ///< Back window info
  
  int voidMat;                  ///< Void material
  int pipeMat;                  ///< Pipe material

  int outerVoid;                ///< Flag to build the outer void cell between f
  int activeFlag;               ///< flag to apply shift

  void populateUnit(const FuncDataBase&,const std::string&,
		    const std::string&,windowInfo&) const;
  
  void applyActiveFrontBack(const double);
  virtual void populate(const FuncDataBase&) override;
  void populateWindows(const FuncDataBase&);
  virtual void createUnitVector(const attachSystem::FixedComp&,
				const long int) override;
  void createCommonSurfaces();
  void createWindowSurfaces();
  void createFlangeSurfaces();
  virtual void createSurfaces();

  void createRectangleSurfaces(const double,const double);

  void createFlange(Simulation&,const HeadRule&);
  void createOuterVoid(Simulation&,const HeadRule&);

  
 public:

  GeneralPipe(const std::string&);
  GeneralPipe(const std::string&,const size_t);
  GeneralPipe(const GeneralPipe& A);
  GeneralPipe& operator=(const GeneralPipe& A);
  virtual ~GeneralPipe() override {}

  /// set merge with front point
  void setJoinFront() { activeFlag |= 1; }
  /// set merge with back point
  void setJoinBack() { activeFlag |= 2; }

  void setJoinFront(const attachSystem::FixedComp&,const long int);
  void setJoinBack(const attachSystem::FixedComp&,const long int);

};

}

#endif
