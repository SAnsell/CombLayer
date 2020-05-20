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

  class L2SPFsegment1;
  class L2SPFsegment2;
  class L2SPFsegment3;
  class L2SPFsegment4;
  class L2SPFsegment5;

  class TDCsegment14;
  class TDCsegment15;
  class TDCsegment16;
  //  class TDCsegment17;
  class TDCsegment18;

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

  std::set<std::string> activeINJ;   ///< active components

  /// All the build units
  std::map<std::string,attachSystem::InnerZone> buildUnits;

  std::shared_ptr<InjectionHall> injectionHall;    ///< in ring front end
  std::shared_ptr<L2SPFsegment1> l2spf1;           ///< segment 1
  std::shared_ptr<L2SPFsegment2> l2spf2;           ///< segment 2
  std::shared_ptr<L2SPFsegment3> l2spf3;           ///< segment 3
  std::shared_ptr<L2SPFsegment4> l2spf4;           ///< segment 4
  std::shared_ptr<L2SPFsegment5> l2spf5;           ///< segment 5
  std::shared_ptr<TDCsegment14>  tdc14;            ///< TDC segment 14
  std::shared_ptr<TDCsegment15>  tdc15;            ///< TDC segment 15
  std::shared_ptr<TDCsegment16>  tdc16;            ///< TDC segment 16
  //  std::shared_ptr<TDCsegment17>  tdc17;            ///< TDC segment 17
  std::shared_ptr<TDCsegment18>  tdc18;            ///< TDC segment 18

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
