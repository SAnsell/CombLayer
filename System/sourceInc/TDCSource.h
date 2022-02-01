/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   sourceInc/TDCSource.h
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
#ifndef SDef_TDCSource_h
#define SDef_TDCSource_h

namespace SDef
{
  class Source;
}

namespace SDef
{

/*!
  \class TDCSource
  \version 1.0
  \author S. Ansell
  \date September 2018
  \brief Fluka external source
*/

class TDCSource : 
  public attachSystem::FixedRotateUnit,
  public SourceBase
{
 private:

  double energyMin;
  double energyMax;
  double radius;
  double length;
  
  void populate(const ITYPE&);
  virtual void createAll(Simulation&,const FixedComp&,const long int) {}
  
 public:

  TDCSource(const std::string&);
  TDCSource(const std::string&,const std::string&);
  TDCSource(const TDCSource&);
  TDCSource& operator=(const TDCSource&);
  virtual TDCSource* clone() const;
  virtual ~TDCSource();

  void createAll(const ITYPE&,const attachSystem::FixedComp&,
		 const long int);
  void createAll(const attachSystem::FixedComp&,
		 const long int);

  virtual void rotate(const localRotate&);
  virtual void createSource(SDef::Source&) const;

  virtual void write(std::ostream&) const;
  virtual void writePHITS(std::ostream&) const;
  virtual void writeFLUKA(std::ostream&) const;
    
};

}

#endif
 
