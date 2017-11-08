/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   sourceInc/BeamSource.h
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
#ifndef SDef_BeamSource_h
#define SDef_BeamSource_h

namespace SDef
{
  class Source;
}

namespace SDef
{

/*!
  \class BeamSource
  \version 1.0
  \author S. Ansell
  \date September 2015
  \brief Circular Beam source
*/

class BeamSource : 
  public attachSystem::FixedOffset,
  public SourceBase
{
 private:

  double radius;
  double angleSpread;           ///< Angle spread
  
  void populate(const FuncDataBase& Control);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);

 public:

  BeamSource(const std::string&);
  BeamSource(const BeamSource&);
  BeamSource& operator=(const BeamSource&);
  virtual BeamSource* clone() const;
  virtual ~BeamSource();

  void setRadius(const double R) { radius=R; }
  void createAll(const FuncDataBase&,const attachSystem::FixedComp&,
		 const long int);
  void createAll(const attachSystem::FixedComp&,
		 const long int);

  virtual void rotate(const localRotate&);
  virtual void createSource(SDef::Source&) const;
  virtual void write(std::ostream&) const;
  virtual void writePHITS(std::ostream&) const;
    
};

}

#endif
 
