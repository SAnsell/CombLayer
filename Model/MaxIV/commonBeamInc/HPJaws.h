/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeamInc/HPJaws.h
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
#ifndef tdcSystem_HPJaws_h
#define tdcSystem_HPJaws_h

class Simulation;


namespace xraySystem
{

  /*!
  \class HPJaws
  \version 1.0
  \author S. Ansell
  \date January 2021

  \brief High Precision torsional haw system  for Max-IV
  
  This is a high precision jaw set for Max IV. Based on 
  the Oxford design
*/

class HPJaws :
  public attachSystem::FixedRotate,
  public attachSystem::ContainedComp,
  public attachSystem::FrontBackCut,
  public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 private:

  bool flangeJoin;         ///< Join to flange of previous object
  
  double radius;           ///< main void radius
  double length;           ///< Void length 
  double sideThick;        ///< Wall thickness
  double wallThick;        ///< Front/Back Wall thickness

  double flangeInnerRadius;     ///< Port radius
  double flangeRadius;          ///< Attached back flange radius
  double flangeLength;          ///<  Attached back flange length 

  double dividerThick;        ///< Divider for jaws
  double dividerGap;          ///< Divider gap

  
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

  HPJaws(const std::string&);
  HPJaws(const std::string&,const std::string&);
  HPJaws(const HPJaws&);
  HPJaws& operator=(const HPJaws&);
  virtual ~HPJaws();

  /// flag setting 
  void setFlangeJoin() { flangeJoin=1; }
  using FixedComp::createAll;
  virtual void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
