/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   xmlInc/XMLload.h
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
#ifndef XMLload_h
#define XMLload_h


namespace XML
{

/*!
  \class XMLload
  \brief Load an XML file
  \author S. Ansell
  \date October 2008
  \version 1.0
*/

class XMLload 
{
 public:

  typedef std::vector<std::string> CStore;  ///< Storage

 private:

  std::ifstream IX;                      ///< Input stream
  std::string Lines;                     ///< ProcessLine

  long int cPos;                        ///< Current Position

  XMLload(const XMLload&);               ///< Private: copy constructor
  XMLload& operator=(const XMLload&);    ///< Private: assignment

  void getNext();
  int stripComment();

 public:

  XMLload();
  XMLload(const std::string&);
  ~XMLload();

  int operator++();
  int operator--();
  /// operator++ transfer method 
  int operator++(const int) { return ++(*this); } 
  /// operator-- transfer method 
  int operator--(const int) { return --(*this); } 
  int operator+=(const int);
  int operator-=(const int);
  

  int openFile(const std::string&);
  // Stuff that can call getNext:
  int get(char&);
  char current() const;
  /// Access current position
  long int getPos() const { return cPos; }
  void setPos(const long int);

  int pop();
  void eraseSection(const size_t,const size_t);
  std::string getSection();
 
  int good();

};

}    // NAMESPACE XML

#endif
