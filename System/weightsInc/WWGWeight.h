/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   weightInc/WWGWeight.h
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
#ifndef WeightSystem_WWGWeight_h
#define WeightSystem_WWGWeight_h

namespace WeightSystem
{
  class WWG;
  
/*!
  \class WWGWeight
  \version 1.0
  \author S. Ansell
  \date November 2015
  \brief Tracks cell weight in WWG mesh
*/
  
class WWGWeight : public ItemWeight
{

 public:

  WWGWeight();
  WWGWeight(const WWGWeight&);
  WWGWeight& operator=(const WWGWeight&);    
  virtual ~WWGWeight() {}          ///< Destructor

  void updateWM(WWG&,const double,const double,
		const double,const double) const;
  void invertWM(WWG&,const double,const double,
		const double,const double) const;

};

}

#endif
