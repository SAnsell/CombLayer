/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/boltRing.h
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
#ifndef constructSystem_boltRing_h
#define constructSystem_boltRing_h

class Simulation;

namespace constructSystem
{
  class RingSeal;
  class InnerPort;
  
/*!
  \class boltRing
  \version 1.0
  \author S. Ansell
  \date March 2016
  \brief boltRing unit  
  
  This piece aligns away from the chopper axis. Using
  the chopper origin [bearing position]
*/

class boltRing :
  public attachSystem::FixedOffset,
  public attachSystem::ContainedComp,
  public attachSystem::CellMap,
  public attachSystem::SurfMap,
  public attachSystem::FrontBackCut
{
 private:

  const std::string baseName;  ///< Base name
  const int ringIndex;         ///< Index of surface offset
  int cellIndex;                ///< Cell index  

  size_t populated;             ///< Popoluated variable flag
  bool innerExclude;               ///< Construct inner void
  
  size_t NBolts;                ///< number of motor bolts  
  double boltRadius;            ///< Bolt radius
  double innerRadius;           ///< inner radius
  double outerRadius;           ///< outer Radius
  double thick;                 ///< Thickness of plate
  double angOffset;             ///< Offset from +Z for first bolt
  
  double sealRadius;            ///< Main radius of seal
  double sealThick;             ///< Main thickness of seal
  double sealDepth;             ///< Main depth of seal
  
  int boltMat;                  ///< Bolt material
  int mainMat;                  ///< Main Wall material layer
  int sealMat;                  ///< Seal material
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const long int);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  
 public:

  boltRing(const std::string&,const std::string&);
  boltRing(const boltRing&);
  boltRing& operator=(const boltRing&);
  virtual ~boltRing();

  void setDimensions(const size_t,const double,const double,
		     const double,const double,const double);
  void setMaterials(const std::string&,const std::string&);
  /// set Void
  void setInnerExclude() { innerExclude=1; }
  
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
