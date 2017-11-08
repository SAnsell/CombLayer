/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   include/KCode.h
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#ifndef SDef_KCode_h
#define SDef_KCode_h

class Simulation;

namespace SDef
{
  class Source;
  
/*!
  \class KCode
  \version 1.0
  \date July 2009
  \author S.Ansell
  \brief KCode Term
  
  Generic kcode term 
*/

class KCode :
  public SourceBase
{
 private:

  /// Kcode :nsrck rkk ikz kct msrk knrm mrkp kc8
  double rkk;                     ///< keff 
  std::vector<int> defFlag;       ///< Default values
  std::vector<int> vals;          ///< Values

  std::vector<Geometry::Vec3D> ksrc;          ///< Ksrc cards

 public:
   
  KCode();
  KCode(const KCode&);
  KCode& operator=(const KCode&);
  virtual KCode* clone() const;
  virtual ~KCode();

  void setKSRC(const std::vector<Geometry::Vec3D>&); 
  void setLine(const std::string&);

  virtual void createSource(SDef::Source&) const { }
  virtual void writePHITS(std::ostream&) const;
  virtual void write(std::ostream&) const;  
};

}

#endif
