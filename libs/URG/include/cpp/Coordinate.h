#ifndef QRC_COORDINATE_H
#define QRC_COORDINATE_H

/*!
  \file
  \brief 座標系

  \author Satofumi KAMIMURA

  $Id: Coordinate.h 1601 2010-01-04 15:09:50Z satofumi $
*/

#include "Position.h"
#include <set>
#include <memory>


namespace qrk
{
  /*!
    \brief 座標系
  */
  class Coordinate
  {
  public:
    Coordinate(void);
    virtual ~Coordinate(void);


    /*!
      \brief グローバル座標系の取得
    */
    static Coordinate* root(void);


    /*!
      \brief 座標系の原点を設定

      指定した座標系に原点位置を割り付ける。

      \param[in,out] parent 親として設定する座標系
      \param[in] position 原点の親座標系での位置
    */
    void setOriginTo(Coordinate* parent, const Position<long>& position);


    /*!
      \brief 親座標系の取得

      \retval 親座標系
    */
    Coordinate* parent(void) const;


    /*!
      \brief 子座標系の取得

      \retval 子座標系のポインタ一覧
    */
    std::set<Coordinate*> children(void) const;


    /*!
      \brief 子座標系の原点位置を取得

      \param[in] child 子座標系
      \retval 子座標系の原点位置

      \todo 余力あらば、任意の座標系を与えて処理できるように変更する
    */
    Position<long> offset(const Coordinate* child) const;


    /*!
      \brief 指定した位置が座標系のどこに相当するかを取得

      \param[in] position 位置
      \param[in] coordinate 位置が存在する座標系
      \retval 自身の座標系での位置
    */
    Position<long> pointPosition(const Coordinate* coordinate,
                                 const Position<long>& position) const;

  protected:
    /*!
      \brief 座標系の評価前に呼ばれるメソッド
    */
    virtual void beforeEvaluate(void)
    {
    }

  private:
    Coordinate(Coordinate* parent);
    Coordinate(const Coordinate& rhs);
    Coordinate& operator = (const Coordinate& rhs);

    struct pImpl;
    std::auto_ptr<pImpl> pimpl;
  };
}

#endif /*! QRC_COORDINATE_H */
