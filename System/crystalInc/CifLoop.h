/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   crystalInc/CifLoop.h
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
#ifndef Crystal_CifLoop_h
#define Crystal_CifLoop_h

namespace Crystal
{
  
  /*!
    \class CifLoop
    \author S. Ansell
    \version 1.0
    \date July 2009
    \brief Holds a loop component of a cif- file
  */

class CifLoop
{
 private:

  int status;         ///< 0-empty/1-keys/2-data/3 full/4 full and new loop

  size_t iSize;                         ///< Index size
  std::map<std::string,size_t> Index;   ///< Index to items position
  std::vector<loopItem> Lines;       ///< Keys to data found


  void insertKey(const std::string&);
  int insertData(const std::string&);
  void removeLastLine();

 public:

  CifLoop();
  CifLoop(const CifLoop&);
  CifLoop& operator=(const CifLoop&);
  ~CifLoop() {}                         ///< Destructor
  
  /// Access status
  int getStatus() const { return status; }
  /// Get Line cound
  size_t getLineCnt() const { return Lines.size(); }
  /// Clear the system
  void clearAll();

  int getIntFromKey(const std::vector<std::string>&,
		    std::vector<size_t>&) const;
  bool hasKey(const std::string&) const;
  int registerLine(std::string&);

  template<typename T>
  int getItem(const size_t,const size_t,T&) const;

};

}

#endif
