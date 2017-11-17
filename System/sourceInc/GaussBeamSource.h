 /********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   sourceInc/GaussBeamSource.h
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#ifndef SDef_GaussBeamSource_h
#define SDef_GaussBeamSource_h

namespace SDef
{
  class Source;
}

namespace SDef
{

/*!
  \class GaussBeamSource
  \version 1.0
  \author S. Ansell
  \date September 2015
  \brief Circular Beam source
*/

class GaussBeamSource : 
  public attachSystem::FixedOffset,
  public SourceBase
{
 private:
  
  double xWidth;                ///< X fwhm
  double zWidth;                ///< Z fwhm
  double angleSpread;           ///< Angle spread
  
  void populate(const FuncDataBase& Control);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);
  
 public:

  GaussBeamSource(const std::string&);
  GaussBeamSource(const GaussBeamSource&);
  GaussBeamSource& operator=(const GaussBeamSource&);
  virtual GaussBeamSource* clone() const;
  virtual ~GaussBeamSource();

  void setSize(const double,const double);
  
  void createAll(const FuncDataBase&,const attachSystem::FixedComp&,
		 const long int);

  virtual void rotate(const localRotate&);
  virtual void createSource(SDef::Source&) const;
  virtual void write(std::ostream&) const;
  virtual void writePHITS(std::ostream&) const;
    
};

}

#endif
 
