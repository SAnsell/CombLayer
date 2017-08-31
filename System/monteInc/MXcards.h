/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   monteInc/MXcards.h
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
#ifndef MonteCarlo_MXcards_h
#define MonteCarlo_MXcards_h

namespace MonteCarlo
{
  class Zaid;
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

  std::string particle;               ///< Item point
  std::map<size_t,std::string> items;     ///< Items [Zaid : value]

 public:
  
  MXcards(const std::string&);
  MXcards(const MXcards&);
  MXcards& operator=(const MXcards&);
  bool operator==(const MXcards&) const;
  
  /// Clone function
  MXcards* clone() const { return new MXcards(*this); }
  virtual ~MXcards();

  bool hasZaid(const size_t) const;
  void setZaid(const size_t,const std::string&);
  const std::string& getZaid(const size_t) const;


  void write(std::ostream&,const std::vector<Zaid>&) const;               
  
};

}

#endif
