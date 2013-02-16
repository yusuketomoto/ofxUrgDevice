#ifndef QRK_RING_BUFFER_H
#define QRK_RING_BUFFER_H

/*!
  \file
  \brief リングバッファ

  \author Satofumi KAMIMURA

  $Id: RingBuffer.h 1811 2010-04-30 16:12:05Z satofumi $
*/

#include <deque>


namespace qrk
{
    //! リングバッファ
    template <class T>
    class RingBuffer
    {
    public:
        explicit RingBuffer(void)
        {
        }


        /*!
          \brief バッファサイズの取得
        */
        size_t size(void)
        {
            return ring_buffer_.size();
        }


        /*!
          \brief バッファが空か

          \retval true データなし
          \retval false データあり
        */
        bool empty(void)
        {
            return ring_buffer_.empty();
        }


        /*!
          \brief データの格納

          \param[in] data データ
          \param[in] size データ個数
        */
        void put(const T* data, size_t size)
        {
            const T* last_p = data + size;
            ring_buffer_.insert(ring_buffer_.end(), data, last_p);
        }


        /*!
          \brief データの取り出し

          \param[out] data データ取り出し用バッファ
          \param[in] size 取り出すデータの最大個数

          \return 取り出したデータ個数
        */
        size_t get(T* data, size_t size)
        {
            size_t n = std::min(size, ring_buffer_.size());
            std::copy(ring_buffer_.begin(), ring_buffer_.begin() + n, data);
            ring_buffer_.erase(ring_buffer_.begin(), ring_buffer_.begin() + n);
            return n;
        }


        /*!
          \brief データの書き戻し

          \param[in] ch 書き戻すデータ
        */
        void ungetc(const T ch)
        {
            ring_buffer_.push_front(ch);
        }


        /*!
          \brief 格納データのクリア
        */
        void clear(void)
        {
            ring_buffer_.clear();
        }


    private:
        RingBuffer(const RingBuffer& rhs);
        RingBuffer& operator = (const RingBuffer& rhs);

        std::deque<T> ring_buffer_;
    };
}

#endif /* !QRK_RING_BUFFER_H */
