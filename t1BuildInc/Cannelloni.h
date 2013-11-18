/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   t1BuildInc/Cannelloni.h
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#ifndef ts1System_Cannelloni_h
#define ts1System_Cannelloni_h

class Simulation;

namespace constructSystem
{
  class hexUnit;
}

namespace ts1System
{
  class ProtonVoid;
  class BeamWindow;
}

namespace ts1System
{
/*!
  \class Cannelloni
  \version 1.0
  \author S. Ansell
  \date December 2010
  \brief Creates the TS2 target [with nose cone]

  Provides linkage to its outside on FixedComp[0]
*/

class Cannelloni : public constructSystem::TargetBase
{
 private:
  
  typedef std::map<int,constructSystem::hexUnit*> MTYPE;
  
  const int tarIndex;           ///< Index of surface offset

  int cellIndex;                ///< Cell index

  int frontPlate;               ///< Front Plate
  int backPlate;                ///< Back Plate

  double xStep;               ///< Master offset distance 
  double yStep;               ///< Master offset distance 
  double zStep;               ///< Master offset distance 
  
  double mainLength;            ///< Straight length
  double coreRadius;            ///< Inner W radius [cyl]
  double wallThick;             ///< Wall thickness
  double wallClad;              ///< Wall thickness
  double frontThick;            ///< Front plate thickness
  double voidThick;             ///< Void thickness

  double tubeRadius;            ///< Inner tube radius
  double tubeClad;              ///< Inner tube cladding

  Geometry::Vec3D HexHA;        ///< Hexagonal X 
  Geometry::Vec3D HexHB;        ///< Hexagonal Z 
  Geometry::Vec3D HexHC;        ///< Hexagonal triple 
  MTYPE HVec;    ///< Hex vector

  int wMat;                     ///< Tungsten material
  int taMat;                    ///< Tatalium material
  int waterMat;                 ///< Target coolant material

  double targetTemp;            ///< Target temperature
  double waterTemp;             ///< Water temperature
  double externTemp;            ///< Pressure temperature

  //  std::vector<thexUnit> 
  int mainCell;                 ///< Main tungsten cylinder

  const Geometry::Vec3D& getHexAxis(const size_t) const;
  void createCentres(const Geometry::Plane*);

  void createLinkSurf();
  void populate(const Simulation&);
  void createUnitVector(const attachSystem::FixedComp&);
  
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  void createBeamWindow(Simulation&);
  void createInnerObjects(Simulation&);
  void createInnerCells(Simulation&);

 public:

  Cannelloni(const std::string&);
  Cannelloni(const Cannelloni&);
  Cannelloni& operator=(const Cannelloni&);
  virtual Cannelloni* clone() const; 
  virtual ~Cannelloni();

  /// Main cell body
  int getMainBody() const { return tarIndex+1; }
  void addInnerBoundary(attachSystem::ContainedComp&) const;
  /// Set the extext of the reflector
  void setRefPlates(const int A,const int B) 
    { frontPlate=A; backPlate=B; }

  void addProtonLine(Simulation&,	 
		     const attachSystem::FixedComp& refFC,
		     const long int index);
  virtual void createAll(Simulation&,
			 const attachSystem::FixedComp&);
  

};

}

#endif
 
