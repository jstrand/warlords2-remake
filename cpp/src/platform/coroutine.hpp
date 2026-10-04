// A computer's turn played on a thread of its own, handing control back and
// forth with the front end so that only one of them ever runs: the Lua's
// coroutine, which the front end resumes once it has shown what the turn
// yielded (a walk, a battle, a pause).
#pragma once

#include <exception>
#include <functional>
#include <string>
#include <vector>

namespace w2 {
struct Army;
}
struct SDL_Thread;
struct SDL_mutex;
struct SDL_cond;

/** What a computer's turn yields to the front end. */
struct Yield {
  std::string what;          // "walk", "pause", "assault", "progress", "turn", "nohumans"
  int ticks = 0;             // "pause": BIOS ticks to wait
  std::vector<w2::Army*> armies;   // "walk": the stack
  std::vector<std::pair<int, int>> tiles;   // "walk": the tiles it covered
};

class Coroutine {
 public:
  using Body = std::function<void(Coroutine&)>;
  explicit Coroutine(Body body);
  ~Coroutine();
  Coroutine(const Coroutine&) = delete;
  Coroutine& operator=(const Coroutine&) = delete;

  /** Run the body until it yields or ends. False once it has ended. Any
   *  exception it threw is thrown again here. */
  bool resume();
  /** From inside the body: hand `y` to the front end and wait to be resumed. */
  void yield(Yield y);
  bool done() const { return finished_; }
  const Yield& value() const { return value_; }

 private:
  static int run(void* self);
  struct Cancelled {};

  Body body_;
  SDL_Thread* thread_ = nullptr;
  SDL_mutex* lock_ = nullptr;
  SDL_cond* cond_ = nullptr;
  bool bodyTurn_ = false;    // whose turn it is to run
  bool started_ = false, finished_ = false, cancel_ = false;
  Yield value_;
  std::exception_ptr error_;
};
