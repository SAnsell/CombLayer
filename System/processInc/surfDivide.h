/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   processInc/surfDivide.h
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
#ifndef ModelSupport_surfDivide_h
#define ModelSupport_surfDivide_h

class Simulation;
class Token;

namespace MonteCarlo
{
  class Qhull;
}

namespace ModelSupport
{

class MergeSurf;
class surfDBase;
class surfRegister;
/*!
  \class surfDivide
  \version 2.0
  \author S. Ansell
  \date March 2010
  \brief Divides objects
*/			
			
class surfDivide
{
 private:
  
  int cellNumber;                   ///< Cell number
  MonteCarlo::Qhull* BaseObj;       ///< BaseObject
  int outCellN;                     ///< Output cell number
  int outSurfN;                     ///< Output surfNum [incremental]

  // Actual changes/removes
  std::vector<surfDBase*> PRules;   ///< Division rules

  std::vector<int> material;        ///< Material list
  std::vector<double> frac;         ///< Fractions
  std::vector<int> cellBuilt;       ///< List of built cells
  
  void preDivide(Simulation&);
  void populateSurfaces();
  void clearRules();

  static std::string writeToken(const std::vector<Token>&);

 public:

  surfDivide();
  surfDivide(const surfDivide&);
  surfDivide& operator=(const surfDivide&);
  ~surfDivide();
  
  void setCellN(const int N) { cellNumber=N; }   ///< master cell number
  void setOutCellN(const int N) { outCellN=N; }  ///< Output number 
  void setOutSurfN(const int N) { outSurfN=N; }  ///< Output surface 

  /// set output number 
  void setOutNum(const int NC,const int NS) 
    { outCellN=NC; outSurfN=NS; } 

  /// Access cell number
  int getCellNum() const { return outCellN; }
  /// accessor to built cells
  const std::vector<int>& getCells() const
  { return cellBuilt; }
  
  void init();
  
  template<typename T> void makePair(const int,const int);
  template<typename T> void makeMulti(const int,const int,const int);
  template<typename T> void makeSignPair(const int,const int,const int);

  template<typename T> void makeTemplate(const int,const int);
  template<typename T> void makeTemplate(const int,const int,const int);
  void addRule(const surfDBase*);

  void addPair(const int,const int);


  template<typename T,typename U> void makePair(const int,const int);

  void addInnerSingle(const int);
  void addOuterSingle(const int);

  /// Add a fraction
  void setBasicSplit(const size_t,const int);
  /// Add a fraction
  void addFrac(const double);
  /// Add a Material
  void addMaterial(const int M) { material.push_back(M); }
  
  void divideAction(std::vector<Token>&,std::vector<Token>&);

  void activeDivide(Simulation&);
  void activeDivideTemplate(Simulation&);

  void addLayers(const size_t,const std::vector<double>&,
		 const std::vector<int>&);
  int procSurfDivide(Simulation&,const ModelSupport::surfRegister&,
		     const int,const int,const int,const int,
		     const std::vector<std::pair<int,int>>&,
		     const std::string&,const std::string&);
  int procSimpleDivide(Simulation&,const ModelSupport::surfRegister&,
		       const int,const int,const int,const int,
		       const std::vector<std::pair<int,int>>&);
  
};


}


#endif
