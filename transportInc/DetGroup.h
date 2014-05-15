/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   transportInc/DetGroup.h
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
#ifndef Transport_DetGroup_h
#define Transport_DetGroup_h


namespace Transport 
{

/*!  
  \class DetGroup
  \brief Defines a detector collection
  \version 1.0
  \author S. Ansell
  \date October 2012
*/

class DetGroup
{
 private:
  
  /// Storage of detector
  std::vector<Detector*> DetVec;

  void eraseGrp();

 public:
  
  DetGroup();
  DetGroup(const DetGroup&);
  DetGroup& operator=(const DetGroup&);
  ~DetGroup();

  void clear();
  void manageDetector(Detector*);
  void addDetector(const Detector&);
  
  Detector& getDet(const size_t);
  const Detector& getDet(const size_t) const;

  void write(std::ostream&,const double) const;

};

}

#endif
