/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   processInc/sourceDataBase.h
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
#ifndef SDef_sourceDataBase_h
#define SDef_sourceDataBase_h

namespace attachSystem
{
  class FixedComp;
}

namespace SDef
{

/*!
  \class sourceDataBase 
  \version 1.0
  \author S. Ansell
  \date October 2017
  \brief Holds a list of sources that are created
*/

class sourceDataBase
{
 private:

  /// Storage of soures
  typedef std::shared_ptr<SourceBase> STYPE;

  /// Storage type : name : startPt : size 
  typedef std::map<std::string,STYPE> SMAP;

  SMAP Components;             ///< Pointer to real objects


  sourceDataBase();
  ///\cond SINGLETON
  sourceDataBase(const sourceDataBase&);
  sourceDataBase& operator=(const sourceDataBase&);	
  ///\endcond SINGLETON

  
 public:
  
  ~sourceDataBase();

  static sourceDataBase& Instance();

  void reset();
  
  bool hasSource(const std::string&) const;
  void addSource(const std::string&,const SourceBase&);
  
  void writePHITS(const std::string&,
		  std::ostream&) const;
  
  void write(const std::string&,
	     std::ostream&) const;
  
};

}

#endif
