/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   LinacInc/TDC.h
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
#ifndef tdcSystem_TDC_h
#define tdcSystem_TDC_h


namespace tdcSystem
{

  class TDCsegment;

  /*!
    \class TDC
    \version 1.0
    \author S. Ansell
    \date April 2020
    \brief Manager of the TDC build

    Note it is a FixedComp to allow link point transfer.
    -- May need Cellmap/SurfMap etc
  */

class TDC :
    public attachSystem::FixedOffset,
    public attachSystem::CellMap
{
 private:

  // storage of segments
  typedef std::map<std::string,std::shared_ptr<TDCsegment>> SegTYPE;
  typedef std::map<std::string,std::shared_ptr<attachSystem::BlockZone>> IZTYPE;

  /// to stop end-build check (length etc) 
  bool noCheck;
  /// stop producing a full list of objects/datum points
  bool pointCheck;
  
  std::set<std::string> activeINJ;   ///< active components

  std::shared_ptr<InjectionHall> injectionHall;    ///< in ring front end

  /// Map of segmentName : TDCsegment
  SegTYPE SegMap; 

  /// Map of segmentName : BlockZone
  IZTYPE bZone;

  /// HeadRules of original spaces:
  std::map<int,HeadRule> originalSpaces;
  
  HeadRule buildSurround(const FuncDataBase&,const std::string&,
			 const std::string&);
  
  std::shared_ptr<attachSystem::BlockZone>
  buildInnerZone(Simulation&,
		 const std::string&,
		 const std::string&);

  void setVoidSpace(const Simulation&,
		    const std::shared_ptr<attachSystem::BlockZone>&,
		    const std::string&);
  
  void reconstructInjectionHall(Simulation&);
  
  
 public:

  TDC(const std::string&);
  TDC(const TDC&);
  TDC& operator=(const TDC&);
  virtual ~TDC();

  /// Do not carry out end point checks:
  void setNoLengthCheck() { noCheck=1; }
  /// Write out all the end points of internal objects
  void setPointCheck() { pointCheck=1; }
  /// set active range
  void setActive(const std::set<std::string>& SC) { activeINJ=SC; }

  void createAll(Simulation&,const attachSystem::FixedComp&,
		     const long int);

};

}

#endif
