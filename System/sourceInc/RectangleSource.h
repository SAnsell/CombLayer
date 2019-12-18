/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   sourceInc/RectangleSource.h
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
#ifndef SDef_RectangleSource_h
#define SDef_RectangleSource_h

namespace SDef
{
  class Source;
  class SourceBase;
}

namespace SDef
{

/*!
  \class RectangleSource
  \version 1.0
  \author S. Ansell
  \date November 2014
  \brief Adds gamma ray circular divergent source
*/

class RectangleSource : 
  public attachSystem::FixedOffsetUnit,
  public SourceBase
{
 private:

  double width;                 ///< width
  double height;                ///< height
  double angleSpread;           ///< Angle distribution
  
  void populate(const mainSystem::MITYPE&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);

 public:

  RectangleSource(const std::string&);
  RectangleSource(const RectangleSource&);
  RectangleSource& operator=(const RectangleSource&);
  virtual RectangleSource* clone() const;
  virtual ~RectangleSource();

  /// accessor to power
  void setRectangle(const double,const double);
  
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
 
