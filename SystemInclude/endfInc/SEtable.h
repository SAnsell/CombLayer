/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   endfInc/SEtable.h
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
#ifndef SEtable_h
#define SEtable_h

namespace ENDF
{
  /*!						
    \class SEtable
    \version 1.0
    \author S. Ansell
    \date June 2010
    \brief Integrated energy/sigma total table 
  */

class SEtable
{
 private:

  int nE;                    ///< Number of Energy
  std::vector<double> E;     ///< Alpha values
  std::vector<double> sTot;  ///< Sigma total
  
 public:
  
  SEtable();
  SEtable(const SEtable&);
  SEtable& operator=(const SEtable&);
  ~SEtable() {}                       ///< Destructor

  /// Get energy
  const std::vector<double>& getE() const { return E; }

  /// Clear arrays
  void clear() { E.clear(); sTot.clear(); nE=0; }
  void addEnergy(const double,const double);

  double STotal(const double) const;
  
};


}
#endif
