/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   include/SimValid.h
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
#ifndef ModelSupport_SimValid_h
#define ModelSupport_SimValid_h


namespace ModelSupport
{

/*!
  \class simPoint
  \brief Simple storage for a point on a track
  \author S. Ansell
  \version 1.0
  \date March 2013
 */
struct simPoint
{
  public:
  
  Geometry::Vec3D Pt;              ///< Surface point
  Geometry::Vec3D Dir;             ///< Surface direction
  int objN;                        ///< Object number
  int surfN;                       ///< Surface crossing 
  MonteCarlo::Object* OPtr;        ///< Object pointer

  
  simPoint(const Geometry::Vec3D& P,const Geometry::Vec3D& D,
	   const int ON,const int SN,MonteCarlo::Object* OP) :
  Pt(P),Dir(D),objN(ON),surfN(SN),OPtr(OP) {}

  simPoint(const simPoint& A) :
  Pt(A.Pt),Dir(A.Dir),objN(A.objN),surfN(A.surfN),OPtr(A.OPtr) {}

  /// assignement operator
  simPoint& operator=(const simPoint& A) 
    {
      if (this!=&A)
	{
	  Pt=A.Pt;
	  Dir=A.Dir;
	  objN=A.objN;
	  surfN=A.surfN;
	  OPtr=A.OPtr;
	}
      return *this;
    }


};

/*!
  \class SimValid
  \brief Applies simple test to a simulation to check validity
  \author S. Ansell
  \version 1.0
  \date March 2013
*/

class SimValid
{
 private:

  Geometry::Vec3D Centre;   // Centre for tracks

  void diagnostics(const Simulation&,
		   const std::vector<simPoint>&) const;
  
 public:
  
  SimValid();
  SimValid(const SimValid&);
  SimValid& operator=(const SimValid&);
  ~SimValid() {}        /// Destructor

  /// Set the centre
  void setCentre(const Geometry::Vec3D& C) { Centre=C;} 

  // MAIN RUN:
  int runPoint(const Simulation&,const Geometry::Vec3D&,const size_t) const;
  
  int runFixedComp(const Simulation&,const size_t) const;

};

}

#endif
