#ifndef QRK_POINT_H
#define QRK_POINT_H

/*!
  \file
  \brief ˆÊ’u‚ÌŒ^’è‹`

  \author Satofumi KAMIMURA

  $Id: Point.h 1972 2011-11-10 20:57:46Z satofumi $
*/

namespace qrk
{
    /*!
      \brief ˆÊ’u‚ÌŒ^’è‹`
    */
    template <typename T>
    class Point
    {
    public:
        T x;
        T y;


        Point(void) : x(0), y(0)
        {
        }


        Point(T x_, T y_) : x(x_), y(y_)
        {
        }


        bool operator == (const Point<T>& rhs) const
        {
            return ((this->x == rhs.x) && (this->y == rhs.y)) ? true : false;
        }


        bool operator < (const Point<T>& rhs) const
        {
            if (y < rhs.y) {
                return true;

            } else if (x < rhs.x) {
                return true;

            } else {
                return false;
            }
        }


        Point<T>& operator += (const Point<T>& rhs)
        {
            this->x += rhs.x;
            this->y += rhs.y;

            return *this;
        }


        const Point<T> operator + (const Point<T>& rhs) const
        {
            return Point<T>(*this) += rhs;
        }


        Point<T>& operator -= (const Point<T>& rhs)
        {
            this->x -= rhs.x;
            this->y -= rhs.y;

            return *this;
        }


        const Point<T> operator - (const Point<T>& rhs) const
        {
            return Point<T>(*this) -= rhs;
        }


        Point<T>& operator *= (const double& rhs)
        {
            this->x = static_cast<T>(this->x * rhs);
            this->y = static_cast<T>(this->y * rhs);

            return *this;
        }


        Point<T> operator * (const double& rhs) const
        {
            return Point<T>(*this) *= rhs;
        }


        friend const Point<T> operator * (const double lhs, const Point<T>& rhs)
        {
            return Point<T>(rhs) *= lhs;
        }
    };
}

#endif /* !QRK_POINT_H */
