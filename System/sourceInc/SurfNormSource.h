/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   sourceInc/SurfNormSource.h
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
#ifndef SDef_SurfNormSource_h
#define SDef_SurfNormSource_h

namespace SDef
{
  class Source;
}

namespace SDef
{

/*!
  \class SurfNormSource
  \version 1.0
  \author S. Ansell
  \date September 2015
  \brief Create a source on the +/- of a surface
*/

class SurfNormSource :
  public attachSystem::FixedOffsetUnit,
  public SourceBase
{
 private:
    
  double angleSpread;           ///< Angle from normal
  int surfNum;                  ///< Surfacte number

  double width;                 ///< Width of source
  double height;                ///< Height of source
    
  void populate(const mainSystem::MITYPE&) override;
  void setSurf(const attachSystem::FixedComp&,
			const long int);

 public:

  SurfNormSource(const std::string&);
  SurfNormSource(const SurfNormSource&);
  SurfNormSource& operator=(const SurfNormSource&);
  SurfNormSource* clone() const override;
  ~SurfNormSource() override;

  void createAll(const mainSystem::MITYPE&,
		 const attachSystem::FixedComp&,
		 const long int);
  void createAll(const attachSystem::FixedComp&,const long int);

  void rotate(const localRotate&) override;
  void createSource(SDef::Source&) const override;
  void writePHITS(std::ostream&) const override;
  void writeFLUKA(std::ostream&) const override;
  void write(std::ostream&) const override;

  
};

}

#endif
 
