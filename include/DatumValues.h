/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   include/DatumValues.h
 *
 * Copyright (c) 2004-2016 by Stuart Ansell
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
#ifndef DatumValues_h
#define DatumValues_h

/*!
  \struct pointDataum
  \author S. Ansell
  \date June 2016
  \brief Single data point from a FixedComp
*/
struct pointDatum
{
  int activeFlag;       ///< 0 Null / 1 point / 2 axis [3 both]
  std::string FCName;   ///< FixedComp name
  long int linkIndex;   ///< link point
  std::string Desc;     ///< Description

  pointDatum(const int,const std::string&,const long int);
  pointDatum(const int,const std::string&,
	     const long int,const std::string&);
  pointDatum(const pointDatum&);
  pointDatum& operator=(const pointDatum&);
  
};

/*!
  \class DatumValues
  \version 1.0
  \date June 2016
  \author S. Ansell
  \brief Datum values for final output
*/

class DatumValues
{
 private:

  std::vector<std::string> orderName;          ///< Order to output file

  std::map<std::string,PointDatum> pointMap;   ///< points to outpur

  DatumValues(const DatumValues&);
  DatumValues& operator=(const DatumValues&);
	      
 public:

  /// Destructor
  virtual ~DatumValues() {} 
  static DatumValues& Instance();

  void addPointRequest(const int,const std::string&,
		       const long int,const std::string&);


  void write(std::ostream&) const;

};

#endif
