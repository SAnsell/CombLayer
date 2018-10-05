/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   flukaProcessInc/pairValueSet.h
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
#ifndef flukaSystem_strValueSet_h
#define flukaSystem_strValueSet_h

namespace flukaSystem
{
  
/*!
  \class strValueSet
  \version 1.0
  \date March 2018
  \author S.Ansell
  \brief Processes the physics cards in the FLUKA output
*/

template <size_t N>
class pairValueSet 
{
 private:

  /// Data type [-1:string / 0:def / 1 double]
  typedef  std::array<std::pair<int,std::string>,N> valTYPE;  
  /// basic type of all
  typedef std::vector<valTYPE> dataTYPE;   

  const std::string keyName;               ///< Key name
  const std::string outName;               ///< Output name for FLUKA
  const std::string tag;                   ///< Tag name

  std::array<double,N> scaleVec;      ///< Scaling for all values
  dataTYPE dataMap;                   ///< Values for cell

  static std::string pairUnit(const double&,
			      const std::pair<int,std::string>&);
 public:

  pairValueSet(const std::string&,const std::string&);
  pairValueSet(const std::string&,const std::string&,
	       const std::string&);
  pairValueSet(const std::string&,const std::string&,
	       const std::string&,const std::array<double,N>&);

  pairValueSet(const pairValueSet&);
  pairValueSet& operator=(const pairValueSet&);
  virtual ~pairValueSet();

  void clearAll();

  void setValue(const std::string&,const size_t,const double);
  void setValues(const std::string&,const std::string&,const std::string&,
		 const double,const double,const double);
  void setValues(const std::string&,const std::string&,const std::string&,
		 const std::string&,const std::string&,const std::string&);


  void writeFLUKA(std::ostream&,const std::string&) const;

};

}

#endif
