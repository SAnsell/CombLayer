/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   buildInc/BulkShield.h
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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
#ifndef ShutterSystem_BulkShield_h
#define ShutterSystem_BulkShield_h

class Simulation;

namespace mainSystem
{
  class inputParam;
}

namespace shutterSystem
{
  class GeneralShutter;
  class BulkInsert;
  class Torpedo;

/*!
  \class BulkShield
  \author S. Ansell
  \version 1.0
  \date February 2011
  \brief Specialized for for the BulkShield
*/

class BulkShield : public attachSystem::FixedComp,
  public attachSystem::ContainedComp,
  public attachSystem::CellMap,
  public attachSystem::ExternalCut
{
 private:
  
  const size_t numberBeamLines;      ///< Number of beamlines

  /// Data for Torpdeo
  std::vector<std::shared_ptr<shutterSystem::Torpedo> > TData;
  /// Data for shutter
  std::vector<std::shared_ptr<shutterSystem::GeneralShutter> > GData;
  /// Data for Inerts
  std::vector<std::shared_ptr<shutterSystem::BulkInsert> > BData;

  double vXoffset;                ///< Offset of the void vessel centre

  double torpedoRadius;           ///< torpedo radius
  double shutterRadius;           ///< Shutter radius
  double innerRadius;             ///< Inner value
  double outerRadius;             ///< Outer value

  double totalHeight;              ///< Height of bulk shield
  double totalDepth;               ///< Depth of bulk shiedl

  int ironMat;                    ///< Shield material

  std::set<std::string> excludeSet; 
  // Functions:

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const long int);

  void createSurfaces();
  void createObjects(Simulation&);
  void createBulkInserts(Simulation&);
  void createTorpedoes(Simulation&);
  void createShutters(Simulation&);

 public:

  BulkShield(const std::string&);
  BulkShield(const BulkShield&);
  BulkShield& operator=(const BulkShield&);
  virtual ~BulkShield();

  
  static const size_t chipShutter;   ///< Chip shutter number
  static const size_t imatShutter;   ///< Imat shutter number
  static const size_t letShutter;    ///< LET shutter number
  static const size_t zoomShutter;   ///< zoom shutter number

  /// Set excluded items
  void addExlcude(const std::string& A)
    { excludeSet.insert(A); }
  /// Access outer limit
  double getORadius() const { return outerRadius; }

  /// Get Mono Exit surface
  int getMonoSurf() const { return SMap.realSurf(buildIndex+37); }
  /// Get Inner Bulk surface
  int getInnerSurf() const { return SMap.realSurf(buildIndex+27); }
  /// Get  Bulk surface
  int getShutterSurf() const { return SMap.realSurf(buildIndex+17); }
  /// Get Torpedo surface
  int getTorpedoSurf() const { return SMap.realSurf(buildIndex+7); }

  const shutterSystem::Torpedo* getTorpedo(const size_t) const;
  const shutterSystem::GeneralShutter* getShutter(const size_t) const;
  const shutterSystem::BulkInsert* getInsert(const size_t) const;
  void processVoid(Simulation&);

  // This three functions should die:
  int calcTorpedoPlanes(const int,std::vector<int>&,int&) const; 
  int calcShutterPlanes(const int,std::vector<int>&,int&) const; 
  int calcOuterPlanes(const int,std::vector<int>&,int&) const;

  void beamlineZeroImp(Simulation&,const int) const;

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);



};

}

#endif
 
