/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeamInc/BoxJaws.h
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
#ifndef tdcSystem_BoxJaws_h
#define tdcSystem_BoxJaws_h

class Simulation;


namespace xraySystem
{

  /*!
  \class BoxJaws
  \version 1.0
  \author S. Ansell
  \date January 2021

  \brief High Precision torsional haw system  for Max-IV
  
  This is a high precision jaw set for Max IV. Based on 
  the Oxford design
*/

class BoxJaws :
  public attachSystem::FixedRotate,
  public attachSystem::ContainedComp,
  public attachSystem::FrontBackCut,
  public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 private:

  
  double width;            ///< main void width
  double height;           ///< main void height
  double length;           ///< Void length 
  double wallThick;        ///< Wall thickness
  double sideThick;        ///< Wall thickness

  double flangeInnerRadius;     ///< Port radius
  double flangeRadius;          ///< Attached back flange radius
  double flangeLength;          ///< Attached back flange length 
  
  double jawFarLen;          ///< Length extending away from beam
  double jawEdgeLen;         ///< Length on cutting surface
  double jawThick;           ///< Thickness
  double jawCornerEdge;      ///< Corner cut size
  double jawCornerFar;       ///< Corner cut size

  double jawXGap;            ///< Separation
  double jawZGap;            ///< Separation 
  
  int voidMat;               ///< void material
  int wallMat;               ///< main material
  int jawMat;                ///< jaw material


  void populate(const FuncDataBase&);  
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  BoxJaws(const std::string&);
  BoxJaws(const std::string&,const std::string&);
  BoxJaws(const BoxJaws&);
  BoxJaws& operator=(const BoxJaws&);
  virtual ~BoxJaws();

  using FixedComp::createAll;
  virtual void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
