/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   logInc/Debug.h
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
#ifndef ELog_Debug_h
#define ELog_Debug_h

namespace ELog
{

/*!
  \class debugStatus
  \version 1.0
  \author S. Ansell
  \brief Simple debug information to control tests 
  \date March 2011
*/

class debugStatus
{
 private:

  int status;      ///< status flag
  int counter;     ///< Counter

  debugStatus();

  ///\cond SINGLETON
  debugStatus(const debugStatus&);
  debugStatus& operator=(const debugStatus&);
  ///\endcond SINGLETON

 public:

    /// Desctructor
  ~debugStatus() {} 
  
  static debugStatus& Instance();
    
  int getFlag() const { return status; }   ///< Get status
  void setFlag(const int I) { status=I; }  ///< set Status 

  void resetCnt() { counter=0; }           ///< Resets counter
  int addCnt() { return ++counter; }       ///< add counter
  int getCnt() const { return counter; }   ///< Get counter
  
};

/*!
  \class ClassCounter 
  \brief Allows the class to be counted
  \author S. Ansell
  \version 1.0
  \date August 2005
  
  Adds a counter for
  the number of instances a class has.
  It is intended to be inherrited into the class
  \todo Required to be templated so that it counts
  each different type
*/

class ClassCounter
{
 public:

  static int classN;         ///< Number of the class

  ClassCounter();
  ClassCounter(const ClassCounter&);
  virtual ~ClassCounter();

  int Ncount() const { return classN; } ///< Return the number of instances
};

}

#endif
