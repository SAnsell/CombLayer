/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   monteInc/IsoTable.h
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
#ifndef IsoTable_h
#define IsoTable_h

/*!
  \class IsoTable
  \version 1.0
  \author S. Ansell
  \date April 2010
  \brief Holds the true isotope weights
*/

class IsoTable
{
 private:

  typedef std::tuple<size_t,size_t,double> ITYPE;
  const size_t ZNum;                ///< Number of elements in table

  /// Storage of masses
  std::vector<ITYPE> ZTable[119]; 

  IsoTable();
  ///\cond SIGNLETON
  IsoTable(const IsoTable&);
  IsoTable& operator=(const IsoTable);
  ///\endcond SIGNLETON

  void populateIso();

 public:

  ~IsoTable() {}         ///< Destructor

  static const IsoTable& Instance();

  double getMass(const size_t,const size_t) const;
  
};

#endif
