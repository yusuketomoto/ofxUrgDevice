#ifndef QRK_ANGLE_H
#define QRK_ANGLE_H

/*!
  \file
  \brief 角度の型管理

  \author Satofumi KAMIMURA

  $Id: Angle.h 1811 2010-04-30 16:12:05Z satofumi $
*/


namespace qrk
{
    /*!
      \brief 角度の型管理
    */
    class Angle
    {
    public:
        Angle(void);
        Angle(const Angle& rhs);

        Angle& operator = (const Angle& rhs);
        Angle& operator += (const Angle& rhs);
        const Angle operator + (const Angle& rhs) const;
        Angle& operator -= (const Angle& rhs);
        const Angle operator - (const Angle& rhs) const;
        Angle& operator *= (const double rhs);
        Angle operator * (const double rhs) const;
        friend const Angle operator * (const double lhs, const Angle& rhs);

        void normalize(void);


        /*!
          \brief degree 角度への変換

          \return degree 角度
        */
        int to_deg(void) const;


        /*!
          \brief radian 角度への変換

          \return radian 角度
        */
        double to_rad(void) const;

    private:
        double radian_angle_;

        friend Angle rad(double radian);
        friend Angle deg(int degree);
    };


    /*!
      \brief radian 角度を Angle に変換

      \param[in] radian radian 角度
      \return Angle オブジェクト
    */
    extern Angle rad(double radian);


    /*!
      \brief degree 角度を Angle に変換

      \param[in] degree degree 角度
      \return Angle オブジェクト
    */
    extern Angle deg(int degree);
}

#endif /* !QRK_ANGLE__H */
