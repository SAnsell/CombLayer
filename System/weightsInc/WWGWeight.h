/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   weightInc/WWGWeight.h
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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

  size_t ID;               ///< Index

  long int WE;             ///< Energy size
  long int WX;             ///< Weight XIndex size
  long int WY;             ///< Weight YIndex size
  long int WZ;             ///< Weight ZIndex size

  std::set<std::string> particles;    ///< Particles
  std::vector<double> EBin;           ///< Energy bins
  Geometry::BasicMesh3D Grid;         ///< Mesh Grid 

  bool logFlag;                       ///< Write output as log
  /// local storage for data [Energy,i,j,k]  in logspace [-ve to zero]
  boost::multi_array<double,4> WGrid; 

  void resize();
  
 public:

  explicit WWGWeight(const size_t);
  WWGWeight(const size_t,const std::vector<double>&);
  WWGWeight(const WWGWeight&);
  WWGWeight(const size_t,const WWGWeight&);
  WWGWeight& operator=(const WWGWeight&);    
  ~WWGWeight() {}          ///< Destructor

  //@{
  /*!
    Accessors				       
    \return named value
  */  

  size_t getID() const { return ID; }
  long int getESize() const { return WE; }
  long int getXSize() const { return WX; }
  long int getYSize() const { return WY; }
  long int getZSize() const { return WZ; }

  /// set log state
  //  void assignLogState(const bool L) { logFlag=L; }
  /// return log state
  //  bool getLogState() const { return logFlag; }

  /// accessor to energy
  const std::vector<double>& getEnergy() const { return EBin; }

  const std::set<std::string>& getParticles() const { return particles; }
  /// access to geometry
  const Geometry::BasicMesh3D& getGeomGrid() const { return Grid; }
  /// accessor to Cells
  const boost::multi_array<double,4>& getGrid() const
    { return WGrid; }

  //@}

  /// setter for ID
  void setID(const size_t A) { ID=A; }
  void setOutLog() { logFlag=1; }
  void setEnergy(const std::vector<double>&);
  void setDefault(const std::vector<double>&);
  void setMesh(const Geometry::Vec3D&,const Geometry::Vec3D&,
	       const long int,const long int,const long int);
  void setMesh(const Geometry::Vec3D&,const Geometry::Vec3D&,
	       const std::array<size_t,3>&);
  void setMesh(const Geometry::BasicMesh3D&);
  void setParticles(const std::set<std::string>& P) {particles=P;}
  
  bool isSized(const long int,const long int,const long int,
	       const long int) const;

  
  void zeroWGrid();
  
  double calcMaxAttn(const long int) const;
  double calcMaxAttn() const;

  void setLogPoint(const long int,const long int,const double);
  void addLogPoint(const long int,const long int,const double);

  void scaleMeshItem(const long int,const long int,const long int,
		     const long int, const double);
  void scaleGrid(const double);
  void scalePower(const double);
  void scaleRange(const size_t eIndex,const double,const double,
		  const double,const double);
  void setMinValue(const double);


  template<typename T>
  double distTrack(const Simulation&,const T&,
		   const double,
		   const Geometry::Vec3D&,
		   const double,const double,
		   const double) const;

  template<typename T>
  void wTrack(const Simulation&,const T&,const size_t,
	      const double,const double,const double);

  template<typename T,typename U>
  void CADISnorm(const Simulation&,const long int,
		 const WWGWeight&,const long int,
		 const WWGWeight&,const long int,
		 const T&,const U&,
		 const double,const double,const double);
  
  void writeCHECK(const size_t) const;

  void writeGrid(std::ostream&) const;
  void writeWWINP(std::ostream&) const;
  void writePHITS(std::ostream&) const;
  void writeFLUKA(std::ostream&) const;
  void writeVTK(std::ostream&,const long int,const bool =0) const;
  void write(std::ostream&) const;
};

}

#endif
