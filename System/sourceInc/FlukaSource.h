/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   sourceInc/FlukaSource.h
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
#ifndef SDef_FlukaSource_h
#define SDef_FlukaSource_h

namespace SDef
{
  class Source;
}

namespace SDef
{

  /// [-1:str/0:def/1:double] :: value unit in string form
  typedef std::pair<int,std::string> unitTYPE;
  /// default type : value unit form
  std::ostream& operator<<(std::ostream&,const SDef::unitTYPE&);

/*!
  \class FlukaSource
  \version 1.0
  \author S. Ansell
  \date September 2018
  \brief Fluka external source
*/

class FlukaSource : 
  public attachSystem::FixedRotateUnit,
  public SourceBase
{
 private:

  std::string sourceName;            ///< Additional card for source
  std::array<unitTYPE,12> sValues;   ///< Main values

  
  void populate(const ITYPE&) override;
  void createAll(Simulation&,const FixedComp&,const long int) override {}
  
 public:

  FlukaSource(const std::string&);
  FlukaSource(const std::string&,std::string );
  FlukaSource(const FlukaSource&);
  FlukaSource& operator=(const FlukaSource&);
  FlukaSource* clone() const override;
  ~FlukaSource() override;

  void createAll(const ITYPE&,const attachSystem::FixedComp&,
		 const long int);
  void createAll(const attachSystem::FixedComp&,
		 const long int);

  void setTDC() { sourceName="TDC"; }
  void rotate(const localRotate&) override;
  void createSource(SDef::Source&) const override;

  void write(std::ostream&) const override;
  void writePHITS(std::ostream&) const override;
  void writeFLUKA(std::ostream&) const override;
    
};

}

#endif
 
