/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/VacuumPipe.h
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#ifndef constructSystem_VacuumPipe_h
#define constructSystem_VacuumPipe_h

class Simulation;

namespace constructSystem
{

  /*!
    \struct windowInfo
    \version 1.0
    \date July 2015
    \author S. Ansell
    \brief window build data
  */
struct windowInfo
{
  double thick;           ///< Joining Flange length
  double radius;          ///< Window radius
  double height;          ///< Window Height
  double width;           ///< Window Width
  int mat;                ///< Material
};
  
/*!
  \class VacuumPipe
  \version 1.0
  \author S. Ansell
  \date July 2015
  \brief VacuumPipe unit  
*/

class VacuumPipe :
  public attachSystem::FixedOffset,
  public attachSystem::ContainedComp,
  public attachSystem::CellMap,
  public attachSystem::SurfMap,
  public attachSystem::FrontBackCut
{
 private:
  
  const int vacIndex;           ///< Index of surface offset
  int cellIndex;                ///< Cell index  

  bool frontJoin;               ///< Flag for front join
  Geometry::Vec3D FPt;          ///< Front point
  Geometry::Vec3D FAxis;        ///< Front point

  bool backJoin;                ///< Flag for front join
  Geometry::Vec3D BPt;          ///< Front point for join
  Geometry::Vec3D BAxis;        ///< Front axis for join

  double radius;                ///< void radius [inner]
  double height;                ///< void radius [inner]
  double width;                 ///< void radius [inner]
  
  double length;                ///< void length [total]

  double feThick;               ///< pipe thickness
  double claddingThick;         ///< cladding thickness

  double flangeARadius;          ///< Joining Flange radius [-ve for rect]
  double flangeAHeight;          ///< Joining Flange height
  double flangeAWidth;           ///< Joining Flange width
  double flangeALength;          ///< Joining Flange length

  double flangeBRadius;          ///< Joining Flange radius [-ve for rect]
  double flangeBHeight;          ///< Joining Flange height
  double flangeBWidth;           ///< Joining Flange width
  double flangeBLength;          ///< Joining Flange length

  int activeWindow;             ///< Flag on window activity
  windowInfo windowFront;       ///< Front window info
  windowInfo windowBack;        ///< Back window info
    
  int voidMat;                  ///< Void material
  int feMat;                    ///< Pipe material
  int claddingMat;              ///< Pipe cladding material 
  
  size_t nDivision;             ///< Number divisions
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const long int);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

  void applyActiveFrontBack();
  void createDivision(Simulation&);
  
 public:

  VacuumPipe(const std::string&);
  VacuumPipe(const VacuumPipe&);
  VacuumPipe& operator=(const VacuumPipe&);
  virtual ~VacuumPipe();

  void setFront(const attachSystem::FixedComp&,const long int,const bool =0);
  void setBack(const attachSystem::FixedComp&,const long int,const bool =0);
  
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
