/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   LinacInc/TDC.h
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
  
  std::set<std::string> activeINJ;   ///< active components

  /// All the build units
  std::map<std::string,attachSystem::InnerZone> buildUnits;
  
  std::shared_ptr<InjectionHall> injectionHall;    ///< in ring front end
  SegTYPE SegMap;

  HeadRule buildSurround(const FuncDataBase&,const std::string&,
			 const std::string&);

  std::unique_ptr<attachSystem::InnerZone>
  buildInnerZone(const FuncDataBase&,const std::string&);

 public:

  TDC(const std::string&);
  TDC(const TDC&);
  TDC& operator=(const TDC&);
  virtual ~TDC();

  /// set active range
  void setActive(const std::set<std::string>& SC) { activeINJ=SC; }

  void createAll(Simulation&,const attachSystem::FixedComp&,
		     const long int);

};

}

#endif
