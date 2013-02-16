/*!
  \file
  \brief 座標系

  \author Satofumi KAMIMURA

  $Id: Coordinate.cpp 1977 2012-04-05 08:04:51Z satofumi $
*/

#include "Coordinate.h"
#include "MathUtils.h"
#include <map>
#include <cstdio>

using namespace qrk;
using namespace std;


namespace
{
  class Child
  {
  public:
    Coordinate* pointer_;
    Position<long> offset_;


    Child(void) : pointer_(NULL)
    {
    }


    Child(Coordinate* pointer, const Position<long>& offset)
      : pointer_(pointer), offset_(offset)
    {
    }
  };
  typedef map<Coordinate*, Child> Children;


  // Rotate.h の関数を使うようにする
  Position<long> rotate(const Position<long>& point, const double radian)
  {
    long x = static_cast<long>
      (lrint((point.x * cos(radian)) - (point.y * sin(radian))));
    long y = static_cast<long>
      (lrint((point.x * sin(radian)) + (point.y * cos(radian))));

    return Position<long>(x, y, point.angle);
  }
}


struct Coordinate::pImpl
{
  Coordinate* parent_;
  Children children_;


  pImpl(Coordinate* parent)
    : parent_(parent)
  {
  }


  void eraseFromParent(Coordinate* coordinate)
  {
    parent_->pimpl->children_.erase(coordinate);
  }


  Position<long> positionOnChild(const Coordinate* child,
                                 const Position<long>& position)
  {
    Coordinate* parent = child->parent();
    Position<long> offset = parent->offset(child);

    // 子の座標系での位置 x, y を、子の offset の角度 t 向きに回転させる
    // 計算後の x, y と t に 子の offset の x, y, t をそれぞれ加算する
    Position<long> converted_position = rotate(position, offset.to_rad());
    converted_position += offset;

    return converted_position;
  }


  Position<long> positionOnParent(const Coordinate* child,
                                  const Position<long>& position)
  {
    Coordinate* parent = child->parent();
    Position<long> offset = parent->offset(child);

    // 親の座標系での位置 x, y から offset の x, y, t を引く
    // 計算後の x, y を offset の角度 -t 向きに回転させる
    Position<long> converted_position = position - offset;
    return rotate(converted_position, -offset.to_rad());
  }


  Position<long> positionOnRoot(const Coordinate* coordinate,
                                  const Position<long>& position)
  {
    if (coordinate == root()) {
      return position;
    }

    return positionOnParent(coordinate,
                            positionOnRoot(coordinate->parent(), position));
  }
};


// root() 専用のコンストラクタ
Coordinate::Coordinate(Coordinate* parent) : pimpl(new pImpl(parent))
{
}


Coordinate::Coordinate(void) : pimpl(new pImpl(root()))
{
  // root の子の座標系として登録する
  setOriginTo(root(), Position<long>(0, 0, deg(0)));
}


Coordinate::~Coordinate(void)
{
  if (! pimpl->parent_) {
    return;
  }

  // 子の座標系を親座標系に割り振る
  // !!!

  // 親から登録を削除
  pimpl->eraseFromParent(this);
}


Coordinate* Coordinate::root(void)
{
  static Coordinate root_coordinate(NULL);
  return &root_coordinate;
}


void Coordinate::setOriginTo(Coordinate* parent,
                             const Position<long>& position)
{
  pimpl->eraseFromParent(this);

  parent->pimpl->children_[this] = Child(this, position);
  pimpl->parent_ = parent;
}


Coordinate* Coordinate::parent(void) const
{
  return pimpl->parent_;
}


set<Coordinate*> Coordinate::children(void) const
{
  set<Coordinate*> children;
  for (Children::iterator it = pimpl->children_.begin();
       it != pimpl->children_.end(); ++it) {
    children.insert(it->first);
  }
  return children;
}


Position<long> Coordinate::offset(const Coordinate* child) const
{
  Children::iterator it = pimpl->children_.find(const_cast<Coordinate*>(child));
  if (it != pimpl->children_.end()) {
    return it->second.offset_;
  }

  // 例外を投げることを検討する
  // !!!
  fprintf(stderr, "Coordinte::offset(): no child.\n");

  Position<long> dummy;
  return dummy;
}


Position<long> Coordinate::pointPosition(const Coordinate* coordinate,
                                         const Position<long>& position) const
{
  // 座標系に特有な処理を実行
  const_cast<Coordinate*>(this)->beforeEvaluate();
  if (coordinate) {
    const_cast<Coordinate*>(coordinate)->beforeEvaluate();
  }

  // coordinate のグローバル座標系での位置を求める
  Position<long> root_position = position;
  Coordinate* root_coordinate = root();
  for (const Coordinate* p = coordinate;
       p != root_coordinate; p = p->parent()) {
    root_position = pimpl->positionOnChild(p, root_position);
    //fprintf(stderr, "{%ld,%ld,%d}, ", root_position.x, root_position.y, root_position.deg());
  }

  // この座標系での位置を求めて返す
  return pimpl->positionOnRoot(this, root_position);
}
