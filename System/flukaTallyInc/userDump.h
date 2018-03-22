/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   flukaTallyInc/userDump.h
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
#ifndef flukaSystem_userDump_h
#define flukaSystem_userDump_h

namespace flukaSystem
{
/*!
  \class userDump
  \version 1.0
  \date Ferbruary 2018
  \author S. Ansell
  \brief userbin for fluka
*/

class userDump : public flukaTally
{
 private:

  int dumpType;                     ///< type +/- 0-7
  std::string outName;              ///< Output tag

 public:

  explicit userDump(const int);
  userDump(const userDump&);
  virtual userDump* clone() const; 
  userDump& operator=(const userDump&);
  virtual ~userDump();

  /// set dump type
  void setDumpType(const int D) { dumpType=D; }
  /// set output tag name
  void setOutName(const std::string& ON) { outName=ON; }
  
  virtual void write(std::ostream&) const;  
};

}

#endif
