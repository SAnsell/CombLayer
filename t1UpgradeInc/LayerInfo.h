/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   t1UpgradeInc/LayerInfo.h
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
#ifndef ts1System_LayerInfo_h
#define ts1System_LayerInfo_h

class Simulation;

namespace ts1System
{

class LayerInfo
{
 private:
 
  double V[6];     ///< Values
  int mat;         ///< Material
  double Temp;     ///< Temperature

 public:

  LayerInfo(const double,const double,const double,
	    const double,const double,const double);
  LayerInfo(const double,const double,const double);
  LayerInfo(const LayerInfo&);
  LayerInfo& operator=(const LayerInfo&);
  ~LayerInfo() {}

  LayerInfo& operator+=(const LayerInfo&);
  LayerInfo& operator+=(const double);

  double Item(const size_t) const;


  void setMat(const int,const double);

  double front() const { return V[0]; }
  double back() const { return V[1]; }
  double left() const { return V[2]; }  
  double right() const { return V[3]; }  
  double top() const { return V[4]; }    
  double base() const { return V[5]; }    
  int getMat() const { return mat; }
  double getTemp() const { return Temp; }
  
};

}

#endif
 
