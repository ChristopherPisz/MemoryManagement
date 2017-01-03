
// Project Includes
#include "ComplexNumber.h"

// Standard Includes
#include <iostream>

//------------------------------------------------------------------------------
ComplexNumber::ComplexNumber(double realPart, double complexPart)
    :
    m_realPart(realPart)
  , m_complexPart(complexPart)
{
   // std::cout << "Complex Number constructed" << std::endl;
}

//------------------------------------------------------------------------------
ComplexNumber::ComplexNumber(const ComplexNumber & rhs)
    :
    m_realPart(rhs.m_realPart)
  , m_complexPart(rhs.m_complexPart)
{
    // std::cout << "Complex Number copy constructed" << std::endl;
}

//------------------------------------------------------------------------------
ComplexNumber::~ComplexNumber()
{
    // std::cout << "Complex Number deconstructed" << std::endl;
}