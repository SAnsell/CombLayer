/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   t1BuildInc/t1BulkShield.h
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

class t1BulkShield : public attachSystem::FixedComp,
    public attachSystem::ContainedComp
{
 private:
  
  const int bulkIndex;            ///< Index of surface offset
  int cellIndex;                  ///< Cell index
  int populated;                  ///< 1:var
  const size_t numberBeamLines;      ///< Number of beamlines

  /// Data for shutter
  std::vector<boost::shared_ptr<shutterSystem::GeneralShutter> > GData;
  /// Data for Inerts
  std::vector<boost::shared_ptr<shutterSystem::BulkInsert> > BData;

  double vYoffset;                ///< Offset of the void vessel centre

  double voidRadius;              ///< Void radius
  double shutterRadius;           ///< Shutter radius
  double innerRadius;             ///< Inner value
  double outerRadius;             ///< Outer value

  double totalHeight;              ///< Height of bulk shield
  double totalDepth;               ///< Depth of bulk shiedl

  int ironMat;                    ///< Shield material

  // Set during build:
  int shutterCell;                ///< Shutter cell 
  int innerCell;                  ///< Inner cell
  int outerCell;                  ///< Outer cell

  // Functions:

  void populate(const Simulation&);
  void createUnitVector();
  void createLinks();

  void createSurfaces(const attachSystem::FixedComp&);
  void createObjects(Simulation&,const attachSystem::ContainedComp&);
  void createBulkInserts(Simulation&,const mainSystem::inputParam&);
  void createShutters(Simulation&,const mainSystem::inputParam&);

 public:

  t1BulkShield(const std::string&);
  t1BulkShield(const t1BulkShield&);
  t1BulkShield& operator=(const t1BulkShield&);
  virtual ~t1BulkShield();

  static const size_t sandalsShutter;  // North 1
  static const size_t prismaShutter;  // North 2
  static const size_t surfShutter;  // North 3
  static const size_t crispShutter;  // North 4
  static const size_t loqShutter;  // North 5
  static const size_t irisShutter;  // North 6
  static const size_t polarisIIShutter;  // North 7
  static const size_t toscaShutter;  // North 8
  static const size_t hetShutter;  // North 9
  static const size_t mapsShutter;  // South 1
  static const size_t vesuvioShutter;  // South 2
  static const size_t sxdShutter;  // South 3
  static const size_t merlinShutter;  // South 4
  static const size_t s5Shutter;  // South 5
  static const size_t mariShutter;  // South 6
  static const size_t gemShutter;  // South 7
  static const size_t hrpdShutter;  // South 8
  static const size_t pearlShutter;  // South 9

  /// Access outer limit
  double getORadius() const { return outerRadius; }

  /// Get Mono (outer) Exit surface
  int getMonoSurf() const { return SMap.realSurf(bulkIndex+37); }
  /// Get Inner Bulk surface
  int getInnerSurf() const { return SMap.realSurf(bulkIndex+27); }
  /// Get  Bulk surface
  int getShutterSurf() const { return SMap.realSurf(bulkIndex+17); }

  const shutterSystem::GeneralShutter* getShutter(const int) const;
  const shutterSystem::BulkInsert* getInsert(const int) const;
  void processVoid(Simulation&);

  void createAll(Simulation&,const mainSystem::inputParam&,
		 const t1CylVessel&);



};

}

#endif
 
