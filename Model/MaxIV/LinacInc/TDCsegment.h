/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   LinacInc/TDCsegment.h
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
#ifndef tdcSystem_TDCsegment_h
#define tdcSystem_TDCsegment_h

namespace tdcSystem
{
  /*!
    \class TDCsegment
    \version 1.0
    \author S. Ansell
    \date May 2020
    \brief Virtual top class for TDC segments
  */

class TDCsegment :
  public attachSystem::FixedRotate,
  public attachSystem::ContainedComp,
  public attachSystem::ExternalCut,
  public attachSystem::CellMap
{
 protected:

  /// System for building a divided inner
  attachSystem::InnerZone* buildZone;

  bool lastFlag;      ///< Front valid
  HeadRule lastRule;  ///< Surface for headrule
  
 public:
  
  TDCsegment(const std::string&,const size_t);
  TDCsegment(const TDCsegment&);
  TDCsegment& operator=(const TDCsegment&);
  virtual ~TDCsegment();

  /// set the current inner zone [allows joining of segments]
  void setInnerZone(attachSystem::InnerZone* IZPtr) { buildZone=IZPtr; }

  void setLastSurf(const HeadRule&);
  /// clear front flag
  void clearLastSurf() { lastFlag=0; }
  /// access flag
  bool hasLastSurf() const { return lastFlag; }
  /// access rule
  const HeadRule& getLastSurf() const { return lastRule; }
  
  virtual void createAll(Simulation&,const attachSystem::FixedComp&,
			 const long int) =0;

};

}

#endif
