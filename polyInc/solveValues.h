/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   polyInc/solveValues.h
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
#ifndef mathLevel_solveValues_h
#define mathLevel_solveValues_h

namespace mathLevel
{

 /*!
   \class solveValues
   \version 1.0
   \author S. Ansell
   \date February 2011
   \brief Solves real values for the polynomial system
 */
 
class solveValues
{ 
 private:

  const double tol;                  ///< Tolerance
  PolyVar<3> Array[3];            ///< Values to test [not deleted]
  std::vector<Geometry::Vec3D> Ans;     ///< Real solutions only

  size_t finalIndex(const size_t,const size_t) const;
  bool notValid(const Geometry::Vec3D&) const;

  
  void processResolved(PolyVar<1>&,const PolyVar<2>&,const size_t,
		       const size_t,const size_t,const size_t,const double);
  

  void addSolutions(const PolyVar<2>&,const PolyVar<2>&,
		    const size_t,const double);

 public:

  solveValues();
  solveValues(const solveValues&);
  solveValues& operator=(const solveValues&);
  ~solveValues();

  /// Access solutions
  const std::vector<Geometry::Vec3D>& 
    getAnswers() const { return Ans; }

  void setEquations(const PolyVar<3>&,
		    const PolyVar<3>&,
		    const PolyVar<3>&);

  void backSolve(const size_t,const size_t,const double,const double);
  void backSolve(const size_t,const double);

  int checkSolutions();
  int getSolution(const PolyVar<3>&,const PolyVar<3>&,const PolyVar<3>&);
  int getSolution();
  
};

}

#endif
