/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   funcBaseInc/varBlock.h
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
#ifndef varBlock_h
#define varBlock_h

/*!
  \struct varBlock
  \brief Group of variables
  \author S. Ansell
  \version 1.0
  \date September 2011

  Stores a set of names to variables that can be accessed either 
  by index value or by keyname.
 */

class varBlock
{
private:

  static std::string getIndexString(const int); 
  template<typename T> static int getTypeNum();
  template<typename T> static std::string getTypeString();

  typedef std::pair<int,size_t> VTYPE;             ///< Variable type 
  
  int populated;                               ///< Populated flag

  size_t tCnt;                                 ///< Total count
  std::vector<double> dV;                      ///< double items
  std::vector<int> iV;                         ///< integer items
  std::vector<Geometry::Vec3D> vV;             ///< Vec3d items

  std::map<std::string,size_t> dataMap;        ///< Data [name : (type/]
  std::vector<VTYPE> indexVec;                 ///< Data [index : (pos/name)]
  std::vector<int> flag;                       ///< Data [index : (pos/name)]

  template<typename T> T& getItem(const size_t);
  template<typename T> const T& getItem(const size_t) const;

public:

  varBlock();
  varBlock(const size_t,const std::string*,const size_t,const std::string*);
  varBlock(const varBlock&);
  varBlock& operator=(const varBlock&);
  ~varBlock() {}   ///< Destructor

 
  template<typename T>
  T getVar(const size_t) const;
  template<typename T>
  T getVar(const std::string&) const;

  template<typename T>
  void setVar(const std::string&,const T&);
    
};


#endif
