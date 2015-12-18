// ConsoleApplication1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <tuple>
#include <iostream>
using namespace std;
#define DEFINE_TASK(x) int __is_task(x&)
template <typename T,typename... Args>
T __check(T&& t,Args&&... );

class taskA {

};
DEFINE_TASK(taskA);
template<std::size_t N,typename T>
void runTask(taskA& t ,T&& next) {
  cout << "task A" << endl;
  runTask<N+1>(get<N+1>(next), next);
}

class taskB {

};
DEFINE_TASK(taskB);
template<std::size_t N, typename T>
void runTask(taskB& t, T&& next) {
  cout << "task B" << endl;
  runTask<N + 1>(get<N + 1>(next), next);
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
void runTask(batch<T> tup) {
  auto t1 = tup >> stop();
  runTask(t1.m_tuple);
 // runTask<0>(get<0>(t1.m_tuple), t1.m_tuple);

}


int main()
{

  auto t = make_tuple(taskA(), taskB());
  auto t2 = tuple_cat(t, t ,make_tuple(stop()));
  
  auto b = taskA() >> taskB();
  // auto t1 = tuple_cat(t, stop());
 // runTask(b);
 // runTask(t2);
    return 0;
}

