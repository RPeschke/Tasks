#include <future>







class batch {};
class returntype {};


template<typename Task>
int is_task(Task&&); // only compiles if Task is actually a task

template<typename returntype,typename... Args>
returntype _check(Args&&...);



template<typename TaskA,typename TaskB>
auto operator>> (TaskA&& a, TaskB&& b)-> decltype(_check<batch>(is_task(a), is_task(b))); 

template<typename TaskB>
auto operator>> (batch&& batch_, TaskB&&)-> decltype(_check<batch>(is_task(B)));


class task_I {

};

template<typename next, typename... Args>
returntype do_task(task_I&, next&& next_, Args&&... args) {

  // do whatever 

  return do_task(next_, args...);
}

class task_II {

};

template<typename next, typename... Args>
returntype do_task(task_II&, next&& next_, Args&&... args) {

  // do whatever 

  return do_task(next_, args...);
}

class task_III {

};

template<typename next, typename... Args>
returntype do_task(task_III&, next&& next_, Args&&... args) {

  // do whatever 

  return do_task(next_, args...);
}

returntype runBatch(batch&);

int main() {


  auto batch_ = task_I() >> task_II() >> task_III();


  auto batch_copy = batch_; // deep copy of all task objects 


  auto a = std::async([&] { runBatch(batch_);});

  auto b = std::async([&] { runBatch(batch_copy);}); // completely independent entity



}