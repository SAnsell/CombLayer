/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   lensModelInc/LensSource.h
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
#ifndef SDef_LensSource_h
#define SDef_LensSource_h

namespace attachSystem
{
  class LinearComp;
}


namespace SDef
{
  class Source;
/*!
  \class LensSource
  \version 1.0
  \author S. Ansell
  \date September 2010
  \brief Adds a new source in the lens system
*/

class LensSource :
  public attachSystem::FixedOffset,
  public SourceBase
{
 private:
  
  double radialArea;          ///< Radial spread

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);
  void rotate(const localRotate&);
  
 public:

  LensSource(const std::string&);
  LensSource(const LensSource&);
  LensSource& operator=(const LensSource&);
  virtual LensSource* clone() const;
  virtual ~LensSource();



  void createAll(const FuncDataBase&,
		 const attachSystem::FixedComp&,
		 const long int);

  virtual void createSource(SDef::Source&) const;
  virtual void writePHITS(std::ostream&) const;
  virtual void write(std::ostream&) const;
  

  
};

}

#endif
 
