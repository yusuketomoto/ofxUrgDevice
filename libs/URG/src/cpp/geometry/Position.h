#ifndef QRK_POSITION_H
#define QRK_POSITION_H

/*!
  \file
  \brief 位置(座標と向き)の型定義

  座標と向きを管理する

  \author Satofumi KAMIMURA

  $Id: Position.h 1811 2010-04-30 16:12:05Z satofumi $
*/

#include "Angle.h"
#include <iostream>


namespace qrk
{
    /*!
      \brief 位置
    */
    template<class T>
    class Position
    {
    public:
        T x;                        //!< X 位置
        T y;                        //!< Y 位置
        Angle angle;                //!< 角度


        Position(void) : x(0), y(0)
        {
        }


        Position(const Position& rhs) : x(rhs.x), y(rhs.y), angle(rhs.angle)
        {
        }


        Position(T x_, T y_, const Angle& angle_) : x(x_), y(y_), angle(angle_)
        {
        }


        int to_deg(void) const
        {
            return angle.to_deg();
        }


        double to_rad(void) const
        {
            return angle.to_rad();
        }


        bool operator == (const Position& rhs) const
        {
            if ((this->x == rhs.x) && (this->y == rhs.y) &&
                (this->angle.to_rad() == rhs.angle.to_rad())) {
                return true;
            } else {
                return false;
            }
        }


        Position& operator = (const Position<T>& rhs)
        {
            this->x = rhs.x;
            this->y = rhs.y;
            this->angle = rhs.angle;

            return *this;
        }

        Position<T>& operator += (const Position<T>& rhs)
        {
            this->x += rhs.x;
            this->y += rhs.y;
            this->angle += rhs.angle;

            return *this;
        }


        const Position<T> operator + (const Position<T>& rhs) const
        {
            return Position<T>(*this) += rhs;
        }


        Position<T>& operator -= (const Position<T>& rhs)
        {
            this->x -= rhs.x;
            this->y -= rhs.y;
            this->angle -= rhs.angle;

            return *this;
        }


        const Position<T> operator - (const Position<T>& rhs) const
        {
            return Position<T>(*this) -= rhs;
        }


        Position<T>& operator *= (const T& rhs)
        {
            this->x *= rhs;
            this->y *= rhs;
            this->angle *= rhs;

            return *this;
        }


        Position<T> operator * (const T& rhs) const
        {
            Position<T> ret(*this);
            return ret *= rhs;
        }


        friend const Position<T> operator * (const T& lhs,
                                             const Position<T>& rhs)
        {
            return Position<T>(rhs) * lhs;
        }


        friend std::ostream& operator << (std::ostream& out,
                                          const Position<T>& rhs)
        {
            out << '(' << rhs.x << ", " << rhs.y
                << ", deg(" << rhs.angle.to_deg() << "))";

            return out;
        }
    };
}

#endif /* !QRK_POSITION_H */
