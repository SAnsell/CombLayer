/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   sourceInc/FlukaExternal.h
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#ifndef SDef_FlukaExternal_h
#define SDef_FlukaExternal_h

namespace SDef
{
  class Source;
}

namespace SDef
{

  /// [-1:str/0:def/1:double] :: value unit in string form
  typedef std::pair<int,std::string> unitTYPE;
  /// default type : value unit form

/*!
  \class FlukaExternal
  \version 1.0
  \author S. Ansell
  \date September 2015
  \brief Circular Beam source
*/

class FlukaExternal : 
  public attachSystem::FixedOffset,
  public SourceBase
{
 private:

  std::string sourceName;            ///< Additional card for source
  std::array<unitTYPE,12> sValues;   ///< Main values
  
  void populate(const ITYPE&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);

 public:

  FlukaExternal(const std::string&);
  FlukaExternal(const FlukaExternal&);
  FlukaExternal& operator=(const FlukaExternal&);
  virtual FlukaExternal* clone() const;
  virtual ~FlukaExternal();

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
 
