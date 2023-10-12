/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   constructVarInc/PipeGenerator.h
 *
 * Copyright (c) 2004-2023 by Stuart Ansell
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
#ifndef setVariable_PipeGenerator_h
#define setVariable_PipeGenerator_h

class FuncDataBase;

namespace setVariable
{

struct segInfo
{
  int type;
  double radius;
  double height;
  double width;
  double thick;
  std::string mat;
};
  
/*!
  \class PipeGenerator
  \version 1.0
  \author S. Ansell
  \date May 2016
  \brief PipeGenerator for variables
*/

class PipeGenerator
{
 private:

  segInfo pipe;

  segInfo flangeA;
  segInfo flangeB;
  segInfo windowA;
  segInfo windowB;

  double claddingThick;         ///< cladding radius
  
  std::string voidMat;          ///< void mat
  std::string claddingMat;      ///< Primary default mat

  int outerVoid;                ///< Flag to build the outer void cell between flanges

 public:

  PipeGenerator();
  PipeGenerator(const PipeGenerator&) =default;
  PipeGenerator& operator=(const PipeGenerator&) =default;
  ~PipeGenerator() =default;

  void setPipe(const double,const double);
  void setRectPipe(const double,const double,const double =-1.0);
  void setNoWindow();
  void setWindow(const double,const double);
  void setRectWindow(const double,const double,const double);
  void setFlange(const double,const double);
  void setRectFlange(const double,const double,const double);
  void setAFlange(const double,const double);
  void setARectFlange(const double,const double,const double);
  void setBFlange(const double,const double);
  void setBRectFlange(const double,const double,const double);
  void setFlangePair(const double,const double,const double,const double);
  void setFlangeLength(const double,const double);

  template<typename CF> void setCF();
  template<typename CF> void setAFlangeCF();
  template<typename CF> void setBFlangeCF();

  /// setter for material name
  void setMat(const std::string&, const std::string& = "");
  void setWindowMat(const std::string&);
  void setWindowMat(const std::string&,const std::string&);
  void setCladding(const double,const std::string&);

  void setOuterVoid(const int val =1) { outerVoid = val; }

  /// accessor
  const std::string& getPipeMat() const { return pipe.mat; }

 
  void generatePipe(FuncDataBase&,const std::string&,
		    const double) const;

};

}

#endif
