/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   processInc/volUnit.h
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
#ifndef ModelSupport_volUnit_h
#define ModelSupport_volUnit_h

namespace ModelSupport
{

/*!
  \class volUnit 
  \version 1.0
  \author S. Ansell
  \date May 2011
  \brief A flux-volume tally
*/

class volUnit
{
 private:

  int npts;              ///< Number of contributions
  std::set<int> cells;   ///< Cell units
  double lineSum;        ///< Sum of length

  std::string comment;   ///< Description
  int matNum;            ///< Material number

 public:
  
  volUnit();
  volUnit(const int,const std::string&,const std::vector<int>&);
  volUnit(const volUnit&);
  volUnit& operator=(const volUnit&);
  ~volUnit();
  
  void setCells(const std::vector<int>&);
  void addCell(const int);
  void reset();
  
  double calcVol(const double) const;
  double calcLine(const double) const;
  void addUnit(const int,const double);
  void addFlux(const int,const double,const double);

  /// access material number
  int getMat() const { return matNum; }
  /// Access comment
  const std::string& getComment() const { return comment; }
  void write(std::ostream&) const;
};

std::ostream&
operator<<(std::ostream&,const volUnit&);

}

#endif
