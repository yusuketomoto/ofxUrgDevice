#ifndef QRK_CONNECTION_H
#define QRK_CONNECTION_H

/*!
  \file
  \brief 通信インターフェース

  \author Satofumi KAMIMURA

  $Id: Connection.h 1988 2012-05-15 07:04:56Z satofumi $
*/

#include <cstddef>


namespace qrk
{
    enum {
        ReceiveTimeout = -1,
        ErrorLastIndex = -2,
    };


    //! 通信インターフェース
    class Connection
    {
    public:
        virtual ~Connection(void) {}


        /*!
          \brief 内部状態を返す

          \return 内部状態を示す文字列
        */
        virtual const char* what(void) const = 0;


        /*!
          \brief デバイスへの接続

          \param[in] device 接続デバイス名
          \param[in] baudrate 接続ボーレート

          \retval true 成功
          \retval false 失敗
        */
        virtual bool connect(const char* device, long baudrate) = 0;


        /*!
          \brief 切断
        */
        virtual void disconnect(void) = 0;


        /*!
          \brief ボーレートの変更

          \param[in] baudrate ボーレート

          \retval 0 正常
          \retval < 0 エラー
        */
        virtual bool setBaudrate(long baudrate) = 0;


        /*!
          \brief ボーレートを返す

          \retval ボーレート

          \attention SerialDevice でのみ有効
        */
        virtual long baudrate(void) const = 0;


        /*!
          \brief 接続状態を返す

          \retval true 接続中
          \retval false 切断中
        */
        virtual bool isConnected(void) const = 0;


        /*!
          \brief 送信

          \param[in] data 送信データ
          \param[in] count 送信バイト数

          \return 送信したバイト数
        */
        virtual int send(const char* data, size_t count) = 0;


        /*!
          \brief 受信

          \param[out] data 受信用バッファ
          \param[in] count 受信バッファの最大サイズ
          \param[in] timeout タイムアウト時間 [msec]
        */
        virtual int receive(char* data, size_t count, int timeout) = 0;


        /*!
          \brief 受信済みデータのバイト数を返す

          \return 受信済みデータのバイト数
        */
        virtual size_t size(void) const = 0;


        /*!
          \brief バッファの内容を強制的に出力する
        */
        virtual void flush(void) = 0;


        /*!
          \brief 送受信データのクリア

          送信バッファ、受信済みデータをクリアする
        */
        virtual void clear(void) = 0;


        /*!
          \brief １文字書き戻す

          \param[in] ch 書き戻す文字
        */
        virtual void ungetc(const char ch) = 0;
    };
}

#endif /* !QRK_CONNECTION_H */
