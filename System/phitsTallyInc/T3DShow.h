/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   phitsTallyInc/T3DShow.h
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#ifndef phitsSystem_T3DShow_h
#define phitsSystem_T3DShow_h

namespace phitsSystem
{
/*!
  \class T3DShow
  \version 1.0
  \date Ferbruary 2018
  \author S. Ansell
  \brief userbin for phits
*/

class T3DShow : public phitsTally
{
 private:

  MeshXYZ xyz; 
  
 public:

  explicit T3DShow(const int);
  T3DShow(const T3DShow&);
  virtual T3DShow* clone() const; 
  T3DShow& operator=(const T3DShow&);
  virtual ~T3DShow();
     

  virtual void write(std::ostream&,const std::string&) const;
  
};

}

#endif
