#include <iostream>
#include <random>
#include <future>
#include <algorithm>
#include <thread>
#include <mutex>
#include "TrafficLight.h"


/* Implementation of class "MessageQueue" */


template <typename T>
T MessageQueue<T>::receive()
{
    //std::cout<<"start locking" << std::endl;
    // FP.5a : The method receive should use std::unique_lock<std::mutex> and _condition.wait() 
    // to wait for and receive new messages and pull them from the queue using move semantics. 
    // The received object should then be returned by the receive function. 
    std::unique_lock<std::mutex> ulock(q_mutex);
    //std::cout<<"Done locking" << std::endl;
    q_cond.wait(ulock, [this] { return !q_queue.empty(); });
    T msg = std::move(q_queue.front());
    q_queue.pop_front();

    return msg;
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    // FP.4a : The method send should use the mechanisms std::lock_guard<std::mutex> 
    // as well as _condition.notify_one() to add a new message to the queue and afterwards send a notification.
    std::lock_guard<std::mutex> lcg(q_mutex);
    q_queue.push_back(std::move(msg));

    q_cond.notify_one();
}


/* Implementation of class "TrafficLight" */


TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
    _queue = std::make_shared<MessageQueue<TrafficLightPhase>>();
    //std::shared_ptr<MessageQueue<TrafficLightPhase>> _queue(new MessageQueue<TrafficLightPhase>;//!!! not work
}

void TrafficLight::waitForGreen()
{  
    // FP.5b : add the implementation of the method waitForGreen, in which an infinite while-loop 
    // runs and repeatedly calls the receive function on the message queue. 
    // Once it receives TrafficLightPhase::green, the method returns.
    while(true) {
        if(_queue->receive() == TrafficLightPhase::green)
            return;
    }
    //return;
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::simulate()
{
    // FP.2b : Finally, the private method „cycleThroughPhases“ should be started in a thread when the public method „simulate“ is called. To do this, use the thread queue in the base class. 
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases,this));

}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{   
    //std::shared_ptr<MessageQueue<TrafficLightPhase>> _queue(new MessageQueue<TrafficLightPhase>);
    // FP.2a : Implement the function with an infinite loop that measures the time between two loop cycles 
    // and toggles the current phase of the traffic light between red and green and sends an update method 
    // to the message queue using move semantics. The cycle duration should be a random value between 4 and 6 seconds. 
    // Also, the while-loop should use std::this_thread::sleep_for to wait 1ms between two cycles. 
    std::vector<std::future<void>> futures; 
    srand(time(NULL)) ;

    std::chrono::time_point<std::chrono::system_clock> lastUpdate = std::chrono::system_clock::now();
    auto cycleDuration = std::chrono::milliseconds((rand() % 2001) + 4000);

    while (true) {
        int lastUpdatedTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - lastUpdate).count();
        //int randNum = (rand() * 1.0 / RAND_MAX * 3) + 4;
        //std::chrono::seconds _interval( randNum );
        //std::this_thread::sleep_for(_interval);
        if(lastUpdatedTime >= cycleDuration.count()) {
            if (_currentPhase == TrafficLightPhase::green) {
                _currentPhase = TrafficLightPhase::red;
            }else {
                _currentPhase = TrafficLightPhase::green;
            }
            futures.emplace_back(std::async(std::launch::async, &MessageQueue<TrafficLightPhase>::send, _queue, std::move(_currentPhase)));
            //_queue.send(std::move(_currentPhase)) if not shared ptr used, or 
            //auto sentFuture = std::async(std::launch::async, &MessageQueue<TrafficLightPhase>::send,&_queue,std::move(_currentPhase));
            //sentFuture.wait();
            cycleDuration = std::chrono::milliseconds((rand() % 2001) + 4000);
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            lastUpdate = std::chrono::system_clock::now();            
        }
    }
    std::for_each(futures.begin(), futures.begin(), [](std::future<void> &ftr) { ftr.wait(); });
}
