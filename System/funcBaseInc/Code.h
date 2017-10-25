/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   funcBaseInc/Code.h
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
#ifndef Code_h
#define Code_h

/*!
  \namespace Opcodes
  \brief Holds function operation codes
  \version 1.0
  \author S. Ansell
  \date April 2006
  
  Holds codes needed for determine function names in
  the eval. not very elegant
*/

namespace Opcodes 
{
  /// Enumerations of cmath functions
  enum CodVal
    {
      cNull,
      cAbs,cAcos,cAcosh,cAsin,cAsinh,
      cAtan,cAtan2,cAtanh,cCeil,cCos,
      cCosd,cCosh,cCot,cCotd,cCsc,cCscd,
      cDot,cExp,cFloor,cInt,cInv,cLog,cLog10,cMax,
      cMin,cSec,cSecd,cSin,cSind,cSinh,cSqrt,cTan,
      cTand,cTanh,cVec3D,
      cNeg,cAdd,cSub,cMul,cDiv,cMod,
      cPow,cDeg,cRad,cEqual,cImmed,cImmedVec,
      varBegin
    };
};

class varList;

/*!
  \class Code
  \brief The Byte code for execution
  \author S. Ansell
  \date April 2006
  \version 1.0

*/

class Code
{
 private:
  
  int valid;                           ///< Good code build
  size_t StackPtr;                     ///< Current point in an evaluation
  size_t StackSize;                    ///< Stack size [max]

  std::vector<int> ByteCode;                ///< Code for operations
  std::map<std::string,int> Labels;         ///< Labels
  std::vector<double> Immed;                ///< Immediate numbers 
  std::vector<Geometry::Vec3D> ImmedVec;    ///< Immediate Vector

  template<typename T,typename U>
    static T typeConvert(const U&);
  template<typename T> static T zeroType();

 public:

  Code();
  Code(const Code&);
  Code& operator=(const Code&);
  ~Code();

  template<typename T>
  T Eval(varList*);

  void clear();
  int popByte();
  void addByte(const int);
  /// Add a avalue
  void addImmediate(const double V) { Immed.push_back(V); }
  /// Add a Vector
  void addImmediate(const Geometry::Vec3D& V) 
      { ImmedVec.push_back(V); }
  void addFunctionOpcode(const int);

  int isValid() const { return valid; }  ///< return validity
  size_t decStackPtr() { return (--StackPtr); }  ///< reduce stack
  size_t incStackPtr();
  size_t addStackPtr(const size_t);
  size_t subStackPtr(const size_t);
  /// get Stack Pt
  size_t getStackPtr() const { return StackPtr; }

  /// Accessor to the true-bytecode
  const std::vector<int>& getBC() const { return ByteCode; }
  /// Adaptor to the bytecode
  std::vector<int>& getBC() { return ByteCode; }
  /// Apply - to the values 
  void minusImmed() { Immed.back()*=-1.0; }
  
  void writeCompact(std::ostream&) const;
  void printByteCode(std::ostream&) const;

};

#endif
