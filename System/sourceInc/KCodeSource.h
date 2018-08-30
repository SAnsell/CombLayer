/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   include/KCodeSource.h
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
#ifndef SDef_KCodeSource_h
#define SDef_KCodeSource_h

class Simulation;

namespace SDef
{
  class Source;
  
/*!
  \class KCodeSource
  \version 1.0
  \date July 2009
  \author S.Ansell
  \brief KCodeSource Term
  
  Generic kcode term 
*/

class KCodeSource :
  public SourceBase
{
 private:

  const std::string keyName;
  /// Kcode :nsrck rkk ikz kct msrk knrm mrkp kc8
  double rkk;                     ///< keff 
  std::vector<int> defFlag;       ///< Default values
  std::vector<int> vals;          ///< Values

  std::vector<Geometry::Vec3D> ksrc;          ///< Ksrc cards

 public:
   
  KCodeSource(const std::string&);
  KCodeSource(const KCodeSource&);
  KCodeSource& operator=(const KCodeSource&);
  virtual KCodeSource* clone() const;
  virtual ~KCodeSource();

  /// accessor
  const std::string& getKeyName() {return keyName;}
  
  void setKSRC(const std::vector<Geometry::Vec3D>&); 
  void setLine(const std::string&);

  virtual void createSource(SDef::Source&) const { }
  virtual void writePHITS(std::ostream&) const;
  virtual void writeFLUKA(std::ostream&) const;
  virtual void write(std::ostream&) const;  
};

}

#endif
