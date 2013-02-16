/*!
  \file
  \brief セマフォ管理

  \author Satofumi KAMIMURA

  $Id: Semaphore.cpp 772 2009-05-05 06:57:57Z satofumi $
*/

#include "Semaphore.h"
#include <SDL_thread.h>

using namespace qrk;


struct Semaphore::pImpl
{
  SDL_sem* sem_;


  pImpl(size_t first_value) : sem_(SDL_CreateSemaphore(first_value))
  {
  }

  ~pImpl(void)
  {
    SDL_DestroySemaphore(sem_);
  }
};



Semaphore::Semaphore(size_t initial_value) : pimpl(new pImpl(initial_value))
{
}


Semaphore::~Semaphore(void)
{
}


void Semaphore::wait(void)
{
  SDL_SemWait(pimpl->sem_);
}


bool Semaphore::tryWait(void)
{
  return (SDL_SemTryWait(pimpl->sem_) == 0) ? true : false;
}


void Semaphore::post(void)
{
  SDL_SemPost(pimpl->sem_);
}


size_t Semaphore::value(void)
{
  return static_cast<size_t>(SDL_SemValue(pimpl->sem_));
}
