// ConsoleApplication1.cpp : Defines the entry point for the console application.
//


#include <tuple>
#include <iostream>
#include <memory>
#include <vector>
using namespace std;
#define DEFINE_TASK(x) int __is_task(x&)
#define  TASK_DEINITION(x) template<std::size_t N,typename T> void runTask(x, T&& next)
#define  Template_TASK_DEINITION(x) template<std::size_t N,typename Tuple,typename T> void runTask(x, Tuple&& next)
#define  RUN_NEXT(buffer)  runTask<N+1>(set_buffer(get<N+1>(next),buffer), next)
#define  RUN_NEXT0()  runTask<N+1>(get<N+1>(next), next)


template <typename T,typename... Args>
T __check(T&& t,Args&&... );

template <typename T, typename... Args>
T __check1( Args&&...);


template <typename T>
auto get_buffer(T& t) {
  return &t.out_buffer;
}

template <typename Task_t,typename buffer_t>
auto  set_buffer(Task_t& task_,buffer_t& buffer_) -> decltype (__check1<Task_t&>(task_.buffer = &buffer_)){
  task_.buffer = &buffer_;
  return task_;
}
class taskA {
public:

};
DEFINE_TASK(taskA);

template <typename T>
class TaskA_imple{
public:
  TaskA_imple(T* buffer_) :buffer(buffer_) {}
  T* buffer ;

};

template <typename T> 
TaskA_imple<T> set_buffer(taskA& task_, T& buffer_) {
  return TaskA_imple<T>(&buffer_);
}


Template_TASK_DEINITION(TaskA_imple<T>& t) {
  cout << "task A imple" << N<<endl;
  for (auto& e: *t.buffer)
  {
    //set_buffer(get<0>(next), e);
    RUN_NEXT(e);
    //runTask<N + 1>(set_buffer(get<N + 1>(next), e), next);
  }
}
TASK_DEINITION(taskA& t) {
 // cout << "task A" << endl;
  for (int i = 0; i < *t.buffer;++i)
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
 // get<N >(next);
  RUN_NEXT(*t.buffer);
}


class stop {
  
};
DEFINE_TASK(stop);

template<typename T>
stop& set_buffer(stop& s, T&&) {
  return s;
}

TASK_DEINITION(stop&){
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
void runTask(batch<T>& tup) {
  auto t1 = tup >> stop();
  runTask(t1.m_tuple);
 // runTask<0>(get<0>(t1.m_tuple), t1.m_tuple);

}

template <typename T, typename T1>
auto operator|(T&& t, batch<T1> && batch_) {
  auto task_ = batch_ >> stop();

 auto ta= set_buffer(get<0>(task_.m_tuple), t);
  runTask<0>(ta, task_.m_tuple);
  
}
int main()
{

  auto t = make_tuple(taskA(), taskB());
  auto t2 = tuple_cat(t, t ,make_tuple(stop()));
  
  std::vector<int> vec{ 1,2,344,5,65,435,35,5,345 };
 // set_buffer(taskB(), get_buffer(taskA()));
  vec | taskA() >> taskB()>>taskB();
  // auto t1 = tuple_cat(t, stop());
  //runTask(b);
 // runTask(t2);
    return 0;
}

