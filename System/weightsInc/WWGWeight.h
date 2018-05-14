/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   weightInc/WWGWeight.h
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
#ifndef WeightSystem_WWGWeight_h
#define WeightSystem_WWGWeight_h

class Simulation;

namespace WeightSystem
{

/*!
  \class WWGWeight
  \version 1.0
  \author S. Ansell
  \date November 2015
  \brief Tracks cell weight in WWG mesh
*/
  
class WWGWeight 
{
 private:

  int zeroFlag;            ///<  set value [0] / zero Value [1]
  
  long int WX;             ///< Weight XIndex size
  long int WY;             ///< Weight YIndex size
  long int WZ;             ///< Weight ZIndex size
  long int WE;             ///< Energy size
  
  /// local storage for data [i,j,k,Energy]
  boost::multi_array<double,4> WGrid; 
  
 public:

  WWGWeight(const size_t,const Geometry::Mesh3D&);
  WWGWeight(const WWGWeight&);
  WWGWeight& operator=(const WWGWeight&);    
  ~WWGWeight() {}          ///< Destructor

  //@{
  /*!
    Accessors				       
    \return named value
  */  
  
  long int getXSize() const { return WX; }
  long int getYSize() const { return WY; }
  long int getZSize() const { return WZ; }
  long int getESize() const { return WE; }

  /// set log state
  //  void assignLogState(const bool L) { logFlag=L; }
  /// return log state
  //  bool getLogState() const { return logFlag; }

  /// accessor to Cells
  const boost::multi_array<double,4>& getGrid() const
    { return WGrid; }

  //@}
  
  bool isSized(const long int,const long int,const long int,
	       const long int) const;
  void resize(const long int,const long int,
	      const long int,const long int);  

  
  void zeroWGrid();
  double calcMaxAttn(const long int) const;
  double calcMaxAttn() const;

  void setLogPoint(const long int,const long int,const double);
  void addLogPoint(const long int,const long int,const double);

  void scaleMeshItem(const long int,const long int,const long int,
		     const long int, const double);
  void scaleGrid(const double);
  void scalePower(const double);
  void scaleRange(const double,const double,const double);
  void setMinSourceValue(const double);


  template<typename T>
  double distTrack(const Simulation&,const T&,
		   const double,
		   const Geometry::Vec3D&,
		   const double,const double,
		   const double) const;

  template<typename T>
  void wTrack(const Simulation&,const T&,
	      const std::vector<double>&,
	      const std::vector<Geometry::Vec3D>&,
	      const double,const double,const double);

  template<typename T,typename U>
  void CADISnorm(const Simulation&,const WWGWeight&,
		 const std::vector<double>&,
		 const std::vector<Geometry::Vec3D>&,
		 const T&,const U&);
  
  
  void writeWWINP(std::ostream&) const;
  void writeVTK(std::ostream&,const long int) const;
  void write(std::ostream&) const;
};

}

#endif
