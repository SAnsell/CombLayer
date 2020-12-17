/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   tallyInc/PDControl.h
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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
#ifndef tallySystem_PDControl_h
#define tallySystem_PDControl_h

namespace tallySystem
{
/*!
  \class PDControl 
  \version 1.0
  \date June 2006
  \version 1.0
  \author S. Ansell
  \brief Holds imp cards
  
  Holds any card which indexes.
  Has a particle list ie imp:n,h nad
  a cell mapping to number. The map is ordered
  prior to being written.
*/

class PDControl
{
 private:

  const int tallyNumber;                 ///< Tally number
  std::map<int,double> pdFactor;         ///< Scale factors etc

 public:

  PDControl(const int);
  PDControl(const PDControl&);
  PDControl& operator=(const PDControl&);
  ~PDControl();

  void clear();

  void setImp(const int,const double);
  void renumberCell(const int,const int);

  void write(std::ostream&) const;
  
};

}

#endif
