#pragma once


//------------------------------------------------------------------------------
class ComplexNumber
{
public:

    ComplexNumber(double realPart, double complexPart);
    ComplexNumber(const ComplexNumber & rhs);
    virtual ~ComplexNumber();

protected:

    double m_realPart;
    double m_complexPart;
};

