/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   buildInc/BulkShield.h
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
    public attachSystem::ContainedComp
{
 private:
  
  const int bulkIndex;            ///< Index of surface offset
  int cellIndex;                  ///< Cell index
  int populated;                  ///< 1:var
  const size_t numberBeamLines;      ///< Number of beamlines

  /// Data for Torpdeo
  std::vector<boost::shared_ptr<shutterSystem::Torpedo> > TData;
  /// Data for shutter
  std::vector<boost::shared_ptr<shutterSystem::GeneralShutter> > GData;
  /// Data for Inerts
  std::vector<boost::shared_ptr<shutterSystem::BulkInsert> > BData;

  double vXoffset;                ///< Offset of the void vessel centre

  double torpedoRadius;           ///< torpedo radius
  double shutterRadius;           ///< Shutter radius
  double innerRadius;             ///< Inner value
  double outerRadius;             ///< Outer value

  double totalHeight;              ///< Height of bulk shield
  double totalDepth;               ///< Depth of bulk shiedl

  int ironMat;                    ///< Shield material

  // Set during build:
  int torpedoCell;                ///< torpedo cell
  int shutterCell;                ///< Shutter cell 
  int innerCell;                  ///< Inner cell
  int outerCell;                  ///< Outer cell

  // Functions:

  void populate(const Simulation&);
  void createUnitVector(const attachSystem::FixedComp&);

  void createSurfaces();
  void createObjects(Simulation&,const attachSystem::ContainedComp&);
  void createBulkInserts(Simulation&,const mainSystem::inputParam&);
  void createTorpedoes(Simulation&,const attachSystem::ContainedComp&);
  void createShutters(Simulation&,const mainSystem::inputParam&);

 public:

  BulkShield(const std::string&);
  BulkShield(const BulkShield&);
  BulkShield& operator=(const BulkShield&);
  virtual ~BulkShield();

  
  static const size_t chipShutter;   ///< Chip shutter number
  static const size_t imatShutter;   ///< Imat shutter number
  static const size_t zoomShutter;   ///< zoom shutter number


  /// Access outer limit
  double getORadius() const { return outerRadius; }

  /// Get Mono Exit surface
  int getMonoSurf() const { return SMap.realSurf(bulkIndex+37); }
  /// Get Inner Bulk surface
  int getInnerSurf() const { return SMap.realSurf(bulkIndex+27); }
  /// Get  Bulk surface
  int getShutterSurf() const { return SMap.realSurf(bulkIndex+17); }
  /// Get Torpedo surface
  int getTorpedoSurf() const { return SMap.realSurf(bulkIndex+7); }

  const shutterSystem::Torpedo* getTorpedo(const size_t) const;
  const shutterSystem::GeneralShutter* getShutter(const size_t) const;
  const shutterSystem::BulkInsert* getInsert(const size_t) const;
  void processVoid(Simulation&);

  // This three functions should die:
  int calcTorpedoPlanes(const int,std::vector<int>&,int&) const; 
  int calcShutterPlanes(const int,std::vector<int>&,int&) const; 
  int calcOuterPlanes(const int,std::vector<int>&,int&) const;

  void beamlineZeroImp(Simulation&,const int) const;

  void createAll(Simulation&,const mainSystem::inputParam&,
		 const attachSystem::FixedComp&,
		 const attachSystem::ContainedComp&);



};

}

#endif
 
