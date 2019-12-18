/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   sourceInc/PointSource.h
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#ifndef SDef_PointSource_h
#define SDef_PointSource_h

namespace SDef
{
  class Source;
}

namespace SDef
{

/*!
  \class PointSource
  \version 1.0
  \author S. Ansell
  \date September 2015
  \brief Ponit source
*/

class PointSource : 
  public attachSystem::FixedOffsetUnit,
  public SourceBase
{
 private:
  
  double angleSpread;           ///< Angle spread
  
  void populate(const mainSystem::MITYPE&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);

 public:

  PointSource(const std::string&);
  PointSource(const PointSource&);
  PointSource& operator=(const PointSource&);
  virtual PointSource* clone() const;
  virtual ~PointSource();

  /// accessor to angle
  void setAngleSpread(const double D) { angleSpread=D; }

  void createAll(const mainSystem::MITYPE&,
		 const attachSystem::FixedComp&,
		 const long int);


  virtual void rotate(const localRotate&);
  virtual void createSource(SDef::Source&) const;
  virtual void write(std::ostream&) const;
  virtual void writePHITS(std::ostream&) const;
  virtual void writeFLUKA(std::ostream&) const;
};

}

#endif
 
