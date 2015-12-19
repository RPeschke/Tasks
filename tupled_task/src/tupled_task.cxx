// ConsoleApplication1.cpp : Defines the entry point for the console application.
//


#include <tuple>
#include <iostream>
#include <memory>
#include <vector>
#include <future>

#include <string>
#include <condition_variable>
#include <chrono>

using namespace std;

#define MULTI_THREADED___
#define DEFINE_TASK(x) int __is_task(x&)
#define  TASK_DEINITION(x) template<std::size_t N,typename T> void runTask(x, T&& next)
#define  Template_TASK_DEINITION(x) template<std::size_t N,typename Tuple,typename T> void runTask(x, Tuple&& next)
#define  RUN_NEXT(buffer)  runTask<N+1>(set_buffer(get<N+1>(next),buffer), next)
#define  RUN_NEXT0()  runTask<N+1>(get<N+1>(next), next)


template<class C, class T>
auto contains(const C& v, const T& x)
-> decltype(end(v), true)
{
  return end(v) != std::find(begin(v), end(v), x);
}


template <typename T, typename... Args>
T __check(T&& t, Args&&...);

template <typename T, typename... Args>
T __check1(Args&&...);


template <typename T>
auto get_buffer(T& t) {
  return &t.out_buffer;
}

template <typename Task_t, typename buffer_t>
auto  set_buffer(Task_t& task_, buffer_t& buffer_) -> decltype (__check1<Task_t&>(task_.buffer = &buffer_)) {
  task_.buffer = &buffer_;
  return task_;
}
class taskA {
public:

};
DEFINE_TASK(taskA);

template <typename T>
class TaskA_imple {
public:
  TaskA_imple(T* buffer_) :buffer(buffer_) {}
  T* buffer;

};

template <typename T>
TaskA_imple<T> set_buffer(taskA& task_, T& buffer_) {
  return TaskA_imple<T>(&buffer_);
}


Template_TASK_DEINITION(TaskA_imple<T>& t) {
  cout << "task A imple" << N << endl;
  for (auto& e : *t.buffer)
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

class Display {
public:
  Display(std::string& prefix) :m_prefix(prefix) {}
  std::string m_prefix;
  int* buffer = nullptr;
};
DEFINE_TASK(Display);


TASK_DEINITION(Display& t) {
  cout << t.m_prefix << " " << this_thread::get_id() << "  " << *t.buffer << endl;
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

TASK_DEINITION(stop&) {
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
->decltype(__check(make_batch(tuple_cat(tup.m_tuple, make_tuple(next))), __is_task(next))) {

  return make_batch(tuple_cat(tup.m_tuple, make_tuple(next)));
}
template <typename Next_t>
auto operator>>(batch<tuple<int>> tup, Next_t&& next) {
  return make_batch(make_tuple(next));
}


template <typename T, typename Next_t>
auto operator>>(T&& first, Next_t&& next)
->decltype(__check(make_batch(make_tuple(first, next)), __is_task(first), __is_task(next))) {

  return make_batch(make_tuple(first, next));
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

  auto ta = set_buffer(get<0>(task_.m_tuple), t);
  runTask<0>(ta, task_.m_tuple);

}

class count_to {
public:
  int * buffer = nullptr;
};
DEFINE_TASK(count_to);

TASK_DEINITION(count_to& task_) {

  int i = 0;
  for (;i <= *task_.buffer;++i)
  {
  }
  RUN_NEXT(i);
}
std::vector<thread::id> m_done;
vector<thread> m_threads;
condition_variable con;
mutex m, m_done_mutex;
size_t get_thread_size() {
  unique_lock<mutex> lock(m);
  return m_threads.size();
}
class thread_pool {
public:
     
     ~thread_pool()
     {
//        for (auto& e : m_threads) {
//          e.join();
//        }
     }
     void join() {
    //   unique_lock<mutex> lock(m);
       while (true) {
         
         for (int i = 0; i < get_thread_size();++i) {

           auto&e = m_threads[i];
           if (contains(m_done, e.get_id())) {
             e.join();
             unique_lock<mutex> lock(m);
             m_threads.erase(std::remove_if(m_threads.begin(), m_threads.end(), [](auto& e) {return e.get_id() == thread::id();}), m_threads.end());
           }
         }

        if (m_threads.empty()){
          break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));   
         //con.wait(lock);
       } 
     }
     template <typename T>
     auto push_thread(T&& f) {
       lock_guard<mutex> lock(m);
       auto* th = this;
   
       m_threads.push_back(thread(f));
       return m_threads.back().get_id();
     }
     void done() {
   //    lock_guard<mutex> lock(m_done_mutex);
       m_done.push_back(this_thread::get_id());
     }

    // std::vector<thread::id> m_done;
   //  mutex m,m_done_mutex;
};
class start_async {
public:
  start_async(thread_pool* tp_) :m_tp(tp_) {}
  start_async(const start_async& async_) :m_tp(async_.m_tp) {}
  int* buffer;
  thread_pool* m_tp = nullptr;

};
DEFINE_TASK(start_async);

class notify {
public:
  notify() {}
  ~notify() {
    {
      unique_lock<mutex> lock(m_done_mutex);
      m_done.push_back(this_thread::get_id());
    }
    con.notify_all();
  }
};
TASK_DEINITION(start_async& tast_) {

  auto buffer = *tast_.buffer;
  auto l = [&, buffer,next]() {

    auto lbuffer = buffer;
    auto local_copy = next;

    notify n;
    runTask<N + 1>(set_buffer(get<N + 1>(local_copy), lbuffer), local_copy);
  };
    
  tast_.m_tp->push_thread(l);
  
  
}

mutex mutex_;
class mutex_task {
public:
  mutex_task(const mutex_task& task_) :m_mutex(task_.m_mutex) {}
  mutex_task(mutex* mutex_) :m_mutex(mutex_) {}
  mutex_task() {}
  mutex* m_mutex;
  int* buffer = nullptr;
};

DEFINE_TASK(mutex_task);

TASK_DEINITION(mutex_task& task_) {
  lock_guard<mutex> lock(mutex_);
  RUN_NEXT(*task_.buffer);
}



class de_randomize {
public:
  de_randomize(thread_pool* tp_) :m_tp(tp_) {}
  thread_pool* m_tp;
  int* buffer;
};
DEFINE_TASK(de_randomize);



bool current_thread() {
  unique_lock<mutex> lock(m_done_mutex);
 // m_threads.erase(std::remove_if(m_threads.begin(), m_threads.end(), [](auto& e) {return e.get_id() == thread::id();}), m_threads.end());
  int i = 0;
  for (auto& e : m_threads) {
    
    if (e.get_id()==thread::id()) {
      continue;
    }
    if ( !contains(m_done,e.get_id())) {
      
      if (e.get_id()== this_thread::get_id()) {
        
        return true;
      }
      break;
    }
  }
  
  return false;
}

TASK_DEINITION(de_randomize& task_) {
  std::unique_lock<std::mutex> lock(m);
  while (!current_thread()) {
    con.wait(lock);
  }
  RUN_NEXT(*task_.buffer);
 // con.notify_all();
}



int main()
{

  //   auto t = make_tuple(taskA(), Display());
  //   auto t2 = tuple_cat(t, t, make_tuple(stop()));
  auto start = clock();
  std::vector<int> vec{1000000000,2000000000,300000000,400000000,500000000,600000000,700000000,1000000000,2000000000,300000000,400000000,500000000,600000000,700000000, 1000000000,2000000000,300000000,400000000,500000000,600000000,700000000, 1000000000,2000000000,300000000,400000000,500000000,600000000,700000000, 1000000000,2000000000,300000000,400000000,500000000,600000000,700000000, 1000000000,2000000000,300000000,400000000,500000000,600000000,700000000, 1000000000,2000000000,300000000,400000000,500000000,600000000,700000000, 1000000000,2000000000,300000000,400000000,500000000,600000000,700000000, 1000000000,2000000000,300000000,400000000,500000000,600000000,700000000, 1000000000,2000000000,300000000,400000000,500000000,600000000,700000000, 1000000000,2000000000,300000000,400000000,500000000,600000000,700000000, 1000000000,2000000000,300000000,400000000,500000000,600000000,700000000, 1000000000,2000000000,300000000,400000000,500000000,600000000,700000000, 1000000000,2000000000,300000000,400000000,500000000,600000000,700000000, 1000000000,2000000000,300000000,400000000,500000000,600000000,700000000, 1000000000,2000000000,300000000,400000000,500000000,600000000,700000000, 1000000000,2000000000,300000000,400000000,500000000,600000000,700000000, 1000000000,2000000000,300000000,400000000,500000000,600000000,700000000, 1000000000,2000000000,300000000,400000000,500000000,600000000,700000000, 1000000000,2000000000,300000000,400000000,500000000,600000000,700000000, 1000000000,2000000000,300000000,400000000,500000000,600000000,700000000, 1000000000,2000000000,300000000,400000000,500000000,600000000,700000000, 1000000000,2000000000,300000000,400000000,500000000,600000000,700000000, 1000000000,2000000000,300000000,400000000,500000000,600000000,700000000, 1000000000,2000000000,300000000,400000000,500000000,600000000,700000000, 1000000000,2000000000,300000000,400000000,500000000,600000000,700000000, 1000000000,2000000000,300000000,400000000,500000000,600000000,700000000, 1000000000,2000000000,300000000,400000000,500000000,600000000,700000000, 1000000000,2000000000,300000000,400000000,500000000,600000000,700000000, 1000000000,2000000000,300000000,400000000,500000000,600000000,700000000, 1000000000,2000000000,300000000,400000000,500000000,600000000,700000000, 1000000000,2000000000,300000000,400000000,500000000,600000000,700000000, 1000000000,2000000000,300000000,400000000,500000000,600000000,700000000, 1000000000,2000000000,300000000,400000000,500000000,600000000,700000000, 1000000000,2000000000,300000000,400000000,500000000,600000000,700000000, 1000000000,2000000000,300000000,400000000,500000000,600000000,700000000, 1000000000,2000000000,300000000,400000000,500000000,600000000,700000000, 1000000000,2000000000,300000000,400000000,500000000,600000000,700000000, 1000000000,2000000000,300000000,400000000,500000000,600000000,700000000, 1000000000,2000000000,300000000,400000000,500000000,600000000,700000000, 1000000000,2000000000,300000000,400000000,500000000,600000000,700000000, 1000000000,2000000000,300000000,400000000,500000000,600000000,700000000, 1000000000,2000000000,300000000,400000000,500000000,600000000,700000000, 1000000000,2000000000,300000000,400000000,500000000,600000000,700000000};
  // set_buffer(taskB(), get_buffer(taskA()));
  thread_pool tp;
  vec | taskA()
#ifdef MULTI_THREADED___
    >> start_async(&tp)
#endif // MULTI_THREADED___
    >> count_to()
#ifdef MULTI_THREADED___
    >> de_randomize(&tp)
#endif//MULTI_THREADED___
    >> Display(std::string("   after"));
  // auto t1 = tuple_cat(t, stop());
  //runTask(b);
 // runTask(t2);
  
  tp.join();
  std::cout << clock() - start<< " ms"  << endl; //linear  (D 13022/ R 3237) ms // multi ( D 13022 / R 23) ms
  return 0;
}

