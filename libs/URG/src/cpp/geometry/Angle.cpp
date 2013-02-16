/*!
  \file
  \brief Šp“x‚ÌŒ^ŠÇ—

  \author Satofumi KAMIMURA

  $Id: Angle.cpp 1975 2012-02-17 01:44:20Z satofumi $
*/

#include "Angle.h"
#include "MathUtils.h"

using namespace qrk;


Angle::Angle(void) : radian_angle_(0.0)
{
}


Angle::Angle(const Angle& rhs) : radian_angle_(rhs.radian_angle_)
{
}


Angle& Angle::operator = (const Angle& rhs)
{
    this->radian_angle_ = rhs.radian_angle_;

    return *this;
}


Angle& Angle::operator += (const Angle& rhs)
{
    this->radian_angle_ += rhs.radian_angle_;

    return *this;
}


const Angle Angle::operator + (const Angle& rhs) const
{
    return Angle(*this) += rhs;
}


Angle& Angle::operator -= (const Angle& rhs)
{
    this->radian_angle_ -= rhs.radian_angle_;

    return *this;
}


const Angle Angle::operator - (const Angle& rhs) const
{
    return Angle(*this) -= rhs;
}


Angle& Angle::operator *= (const double rhs)
{
    this->radian_angle_ *= rhs;

    return *this;
}


Angle Angle::operator * (double rhs) const
{
    return Angle(*this) *= rhs;
}


void Angle::normalize(void)
{
    while (radian_angle_ < -M_PI) {
        radian_angle_ += 2.0 * M_PI;
    }
    while (radian_angle_ > M_PI) {
        radian_angle_ -= 2.0 * M_PI;
    }
}


int Angle::to_deg(void) const
{
    return static_cast<int>(180 * radian_angle_ / M_PI);
}


double Angle::to_rad(void) const
{
    return radian_angle_;
}


Angle qrk::rad(double radian)
{
    Angle angle;
    angle.radian_angle_ = radian;

    return angle;
}


Angle qrk::deg(int degree)
{
    Angle angle;
    angle.radian_angle_ = M_PI * degree / 180.0;

    return angle;
}


const Angle operator * (double lhs, const Angle& rhs)
{
    return Angle(rhs) * lhs;
}
