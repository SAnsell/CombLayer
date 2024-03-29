/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   sourceInc/ParabolicSource.h
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
#ifndef SDef_ParabolicSource_h
#define SDef_ParabolicSource_h

namespace SDef
{
  class Source;
  class SourceBase;
}

namespace SDef
{

/*!
  \class ParabolicSource
  \version 1.0
  \author S. Ansell
  \date November 2014
  \brief Adds gamma ray circular divergent source
*/

class ParabolicSource : 
  public attachSystem::FixedOffsetUnit,
  public SourceBase
{
 private:

  double decayPower;            ///< Fall off power
  size_t nWidth;                ///< Number of widths
  size_t nHeight;               ///< Number of heights
  double width;                 ///< width
  double height;                ///< height
  double angleSpread;           ///< Angle distribution

  
  void populate(const mainSystem::MITYPE&) override;
  void createUnitVector(const attachSystem::FixedComp&,
			const long int) override;

 public:

  ParabolicSource(const std::string&);
  ParabolicSource(const ParabolicSource&);
  ParabolicSource& operator=(const ParabolicSource&);
  ParabolicSource* clone() const override;
  ~ParabolicSource() override;

  /// accessor to power
  void setPower(const double P) { decayPower=P; }
  void setRectangle(const double,const double);
  void setNPts(const size_t,const size_t);
  

  void rotate(const localRotate&) override;
  void createSource(SDef::Source&) const override;
  void write(std::ostream&) const override;
  void writePHITS(std::ostream&) const override;
  void writeFLUKA(std::ostream&) const override;

  void createAll(const mainSystem::MITYPE&,
		 const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
