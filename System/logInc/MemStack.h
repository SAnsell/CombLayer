/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   logInc/MemStack.h
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#ifndef ELog_MemStack_h
#define ELog_MemStack_h

namespace ELog
{
  /*!
    \class MemStack 
    \brief Mem Addresses created / Mem deleted
    \author S. Ansell
    \version 1.0
    \date April 2011
  */
class MemStack
{
 private:

  /// Inner type [string/string]
  typedef std::pair<std::string,std::string> InnerTYPE;
  /// Map type : address : [Units]
  typedef std::map<size_t,InnerTYPE > MTYPE;
  MTYPE Key;                                 ///< Key names
  int verboseFlag;                           ///< Write out each time
  int objCnt;                                ///< Number created

  MemStack();
  /// \cond NOWRITTEN
  MemStack(const MemStack&);
  MemStack& operator=(const MemStack&);
  /// \endcond NOWRITTEN

 public:

  ~MemStack();   

  static MemStack& Instance(); 
  
  void clear();   
  void addMem(const std::string&,const std::string&,const size_t&);
  void delMem(const size_t&);
  /// Set flag
  void setVFlag(const int F) { verboseFlag=F; }
  
  void write(std::ostream&) const;
  
};



}

#endif
