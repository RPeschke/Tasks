// ConsoleApplication1.cpp : Defines the entry point for the console application.
//


#include <tuple>
#include <iostream>
#include <memory>
using namespace std;
#define DEFINE_TASK(x) int __is_task(x&)
#define  TASK_DEINITION(x) template<std::size_t N,typename T> void runTask(x, T&& next)
#define  RUN_NEXT(buffer)  runTask<N+1>(set_buffer(get<N+1>(next),buffer), next)
#define  RUN_NEXT0()  runTask<N+1>(get<N+1>(next), next)


template <typename T,typename... Args>
T __check(T&& t,Args&&... );

template <typename T>
auto get_buffer(T& t) {
  return &t.out_buffer;
}

template <typename Task_t,typename buffer_t>
Task_t&  set_buffer(Task_t& task_,buffer_t& buffer_) {
  task_.buffer = &buffer_;
  return task_;
}
class taskA {
public:
  int* buffer = nullptr;
  int out_buffer = 0;
};
DEFINE_TASK(taskA);

TASK_DEINITION(taskA& t) {
  cout << "task A" << endl;
  for (int i = 0; i < 100;++i)
  {
    RUN_NEXT(i);
  }
}

class taskB {
public:
  int* buffer = nullptr;
};
DEFINE_TASK(taskB);


TASK_DEINITION(taskB& t) {
  cout << "task B" << *t.buffer<< endl;
  RUN_NEXT0();
}


class stop {
  
};
DEFINE_TASK(stop);
template<std::size_t N, typename T>
void runTask(stop& t, T&& next) {
  cout << "stop" << endl;
 
}

template<typename T>
class batch {
public:
  batch(T& t) :m_tuple(t) {}
  T m_tuple;
};
template<typename T>
int __is_batch(batch<T>&);
template<typename T>
batch<T> make_batch(T&& t) {
  return batch<T>(t);
}


auto make_batch() {
  return batch<tuple<int>>(make_tuple(0));
}

template <typename T, typename Next_t>
auto operator>>(batch<T> tup, Next_t&& next) 
->decltype(__check(make_batch(tuple_cat(tup.m_tuple, make_tuple(next))),  __is_task(next))) {

  return make_batch(tuple_cat(tup.m_tuple, make_tuple(next)));
}
template <typename Next_t>
auto operator>>(batch<tuple<int>> tup, Next_t&& next) {
  return make_batch(make_tuple(next));
}


template <typename T, typename Next_t>
auto operator>>(T&& first, Next_t&& next) 
->decltype(__check( make_batch(make_tuple(first, next)),__is_task(first),__is_task(next))) {

  return make_batch(make_tuple(first,next));
}

template<typename T>
void runTask(T&& t) {

  runTask<0>(get<0>(t), t);

}

template<typename T>
void runTask(batch<T>&& tup) {
  auto t1 = tup >> stop();
  runTask(t1.m_tuple);
 // runTask<0>(get<0>(t1.m_tuple), t1.m_tuple);

}

// template <typename T, typename T1>
// auto operator>>(T&& t, batch<T1> && batch_) {
//   
//  // set_buffer(get<0>(batch_.m_tuple), t);
//   //runTask(batch_);
// }
int main()
{

  auto t = make_tuple(taskA(), taskB());
  auto t2 = tuple_cat(t, t ,make_tuple(stop()));
  
 // set_buffer(taskB(), get_buffer(taskA()));
  auto b = taskA() >> taskB();
  // auto t1 = tuple_cat(t, stop());
  runTask(b);
 // runTask(t2);
    return 0;
}

