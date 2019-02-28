/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   flukaProcessInc/cellValueSet.h
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
#ifndef flukaSystem_cellValueSet_h
#define flukaSystem_cellValueSet_h

namespace flukaSystem
{
  
/*!
  \class cellValueSet
  \version 1.0
  \date March 2018
  \author S.Ansell
  \brief Processes the physics cards in the FLUKA output
*/

template <size_t N>
class cellValueSet 
{
 private:

  /// Data type [-1:string / 0:def / 1 double]
  typedef  std::array<std::pair<int,std::string>,N> valTYPE;  
  /// map type [-ve int for string type]
  typedef  std::map<int,valTYPE> dataTYPE;

  const std::string keyName;               ///< Key name
  const std::string outName;               ///< Output name for FLUKA
  const std::string tag;                   ///< Tag name

  std::array<double,N> scaleVec;           ///< Scaling for values
  dataTYPE dataMap;                        ///< Values for cell

  std::map<int,std::string> strRegister;   ///< string register
  std::map<std::string,int> intRegister;   ///< string to int regier
  
  bool simpleSplit(std::vector<std::tuple<int,int>>&,
		   std::vector<valTYPE>&) const;
  bool cellSplit(const std::vector<int>&,
		 std::vector<std::tuple<int,int>>&,
		 std::vector<valTYPE>&) const;

  int makeStrIndex(const std::string&);
  const std::string& getStrIndex(const int) const;
  
 public:

  cellValueSet(const std::string&,const std::string&);
  cellValueSet(const std::string&,const std::string&,
	       const std::string&);
  cellValueSet(const std::string&,const std::string&,
	       const std::string&,const std::array<double,N>&);

  cellValueSet(const cellValueSet&);
  cellValueSet& operator=(const cellValueSet&);
  virtual ~cellValueSet();

  void clearAll();

  void setValue(const int,const size_t,const double);
  void setValues(const int);    
  void setValues(const int,const double);    
  void setValues(const int,const double,const double);
  void setValues(const int,const double,const double,const double);

  void setValues(const int,const std::string&);    
  void setValues(const int,const std::string&,const std::string&);
  void setValues(const int,const std::string&,const std::string&,
		 const std::string&);

  void setValues(const std::string&);
  void setValues(const std::string&,const std::string&);    
  void setValues(const std::string&,const std::string&,const std::string&);
  void setValues(const std::string&,const std::string&,const std::string&,
		 const std::string&);

  void writeFLUKA(std::ostream&,const std::string&) const;
  void writeFLUKA(std::ostream&,const std::vector<int>&,
		  const std::string&) const;

};

}

#endif
