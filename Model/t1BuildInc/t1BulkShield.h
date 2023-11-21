/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   t1BuildInc/t1BulkShield.h
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
#ifndef ShutterSystem_t1BulkShield_h
#define ShutterSystem_t1BulkShield_h

class Simulation;

namespace mainSystem
{
  class inputParam;
}

namespace shutterSystem
{
  class GeneralShutter;
  class BulkInsert;
  class t1CylVessel;

/*!
  \class t1BulkShield
  \author S. Ansell
  \version 1.0
  \date February 2011
  \brief Specialized for for the t1BulkShield
*/

class t1BulkShield :
  public attachSystem::FixedComp,
  public attachSystem::ContainedComp,
  public attachSystem::CellMap,
  public attachSystem::ExternalCut
{
 private:
  
  const size_t numberBeamLines;      ///< Number of beamlines

  /// Data for shutter
  std::vector<std::shared_ptr<shutterSystem::GeneralShutter> > GData;
  /// Data for Inerts
  std::vector<std::shared_ptr<shutterSystem::BulkInsert> > BData;

  double vYoffset;                ///< Offset of the void vessel centre

  double voidRadius;              ///< Void radius
  double shutterRadius;           ///< Shutter radius
  double innerRadius;             ///< Inner value
  double outerRadius;             ///< Outer value

  double totalHeight;              ///< Height of bulk shield
  double totalDepth;               ///< Depth of bulk shiedl

  int ironMat;                    ///< Shield material

  // Functions:

  void populate(const FuncDataBase&);
  void createLinks();

  void createSurfaces();
  void createObjects(Simulation&);
  void createBulkInserts(Simulation&);
  void createShutters(Simulation&);

 public:

  t1BulkShield(const std::string&);
  t1BulkShield(const t1BulkShield&);
  t1BulkShield& operator=(const t1BulkShield&);
  ~t1BulkShield() override;

  /// Access outer limit
  double getORadius() const { return outerRadius; }

  /// Get Mono (outer) Exit surface
  int getMonoSurf() const { return SMap.realSurf(buildIndex+37); }
  /// Get Inner Bulk surface
  int getInnerSurf() const { return SMap.realSurf(buildIndex+27); }
  /// Get  Bulk surface
  int getShutterSurf() const { return SMap.realSurf(buildIndex+17); }

  const shutterSystem::GeneralShutter* getShutter(const int) const;
  const shutterSystem::BulkInsert* getInsert(const int) const;
  void processVoid(Simulation&);

  using attachSystem::FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
			 const long int) override;
  


};

}

#endif
 
