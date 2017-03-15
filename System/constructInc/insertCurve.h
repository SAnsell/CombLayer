/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/insertCurve.h
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#ifndef constructSystem_insertCurve_h
#define constructSystem_insertCurve_h

class Simulation;

namespace constructSystem
{
/*!
  \class insertCurve
  \version 1.0
  \author S. Ansell
  \date November 2011
  \brief  inserted in curved cutting object
  
  Designed to be a quick curved arc to put an object into a model
  for fluxes/tallies etc
*/

class insertCurve : public attachSystem::ContainedComp,
  public attachSystem::FixedOffset,
  public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 private:
  
  const int ptIndex;             ///< Index of surface offset
  int cellIndex;                 ///< Cell index
  int populated;                 ///< externally set values

  double radius;            ///< Radius of curver
  double width;             ///< full inner-outer radius distance
  double height;            ///< Full Z height

  int defMat;               ///< Material
  bool delayInsert;         ///< Delay insertion         

  Geometry::Vec3D Centre;   ///< Centre of rotation
  Geometry::Vec3D APt;      ///< Start point
  Geometry::Vec3D BPt;      ///< End Point
  
  
  void populate(const FuncDataBase&);
  void createUnitVector(const Geometry::Vec3D&,
			const attachSystem::FixedComp&,
			const long int);

  void createUnitVector(const attachSystem::FixedComp&,
			const long int);


  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

  void mainAll(Simulation&);

  void calcCentre();

 public:

  insertCurve(const std::string&);
  insertCurve(const insertCurve&);
  insertCurve& operator=(const insertCurve&);
  ~insertCurve();

  /// set delay flag
  void setNoInsert() { delayInsert=1; }

  void setFrontSurf(const attachSystem::FixedComp&,const long int);
  void setBackSurf(const attachSystem::FixedComp&,const long int);
  void findObjects(Simulation&);
    
  void setStep(const double,const double,const double);
  void setStep(const Geometry::Vec3D&);
  void setAngles(const double,const double);

  void setValues(const double,const double,const double,
		 const int);
  void setValues(const double,const double,const double,
		 const std::string&);
  void createAll(Simulation&,const Geometry::Vec3D&,

		 const Geometry::Vec3D&);

  void createAll(Simulation&,const Geometry::Vec3D&,
		 const attachSystem::FixedComp&);

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int,const Geometry::Vec3D&,
		 const Geometry::Vec3D&);
  
};

}

#endif
 
