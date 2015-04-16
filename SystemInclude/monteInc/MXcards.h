/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   monteInc/MXcards.h
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
#ifndef MonteCarlo_MXcards_h
#define MonteCarlo_MXcards_h

namespace MonteCarlo
{

/*!
  \class MXcards
  \version 1.0
  \date June 2011
  \author S. Ansell
  \brief Holds individual MXcards components
*/

class MXcards 
{
 private:
  
  int Mnum;                           ///< Material number
  size_t NZaid;                       ///< Number of Zaids
  std::string particle;               ///< Item point
  std::vector<std::string> items;     ///< Items

 public:
  
  MXcards();
  MXcards(const int,const size_t,const std::string&);
  MXcards(const MXcards&);
  MXcards& operator=(const MXcards&);
  /// Clone function
  MXcards* clone() const { return new MXcards(*this); }
  virtual ~MXcards();

  void setNumber(const int nA) { Mnum=nA; }  ///< set the MXcards number
  int getNumber() const { return Mnum; }     ///< Assesor function to Number


  void setCard(const int,const std::string&,const size_t);
  void setItem(const std::vector<std::string>&);
  void setItem(const size_t,const std::string&);


  void write(std::ostream&) const;               
  
};

}

#endif
