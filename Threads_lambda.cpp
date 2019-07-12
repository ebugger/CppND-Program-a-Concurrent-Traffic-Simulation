#include<iostream>
#include<thread>
#include<vector>
#include <future>
#include <cmath>
#include <memory>
class Vehicle {
    public:
        Vehicle(int idx, std::string name): id(idx), _myname(new std::string(name)) {}
        Vehicle(): id(0), _myname(new std::string("Default initial constructor")) {}
        Vehicle(Vehicle const &src) {
        // QUIZ: Student code STARTS here
        id = src.id;
        if (src._myname != nullptr)
        {
            _myname = new std::string;
            *_myname = *src._myname;
        }
        // QUIZ: Student code ENDS here
        std::cout << "Vehicle #" << id << " copy constructor called" << std::endl;
        }

        Vehicle(Vehicle&& src) {
            id = src.getID();
            _myname = src.getName();

            id = src.setID(0);
            *_myname = *src._myname("Deafult Name");
            std::cout << "Vehicle #" << id << " move constructor called" << std::endl;
        }

        void operator () () {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            std::cout<< "Overload Id is: " << id << std::endl;
        }
        void addID (int idx) { id = idx; }
        std::string getName() { return *_myname; }
        int getID() { return id; }
        void setName(std::string name) { *_myname = name; }
        void setID(int idx) { id = idx; }
        void printID() {
            std::cout<< "Member func to print ID :" << id << std::endl;
        }
    private:
        int id;
        std::string* _myname;
};

void printID(int &id)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    std::cout << "printID:ID = " << id + 996 << std::endl;
    
}

void printIDAndName(int id, std::string name)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::cout << "printIDAndName ID = " << id << ", name = " << name << std::endl;
}

int main() {
    int idx = 0;
    Vehicle v1;
    Vehicle v2;
    std::shared_ptr<Vehicle> ptr_v2(new Vehicle);
    std::string str1 = "my string";
    std::thread t(Vehicle()); //C++'s most vexing parse
    std::thread t1( (Vehicle(1, "Number 1")) );
    std::thread t2 = std::thread( Vehicle(2, "Number 2") ); //Use copy initialization
    std::thread t3{ Vehicle(3, "Number 3") }; //Use uniform initialization with braces
    std::thread t4(printIDAndName, 888, str1); //call by variadic template NOT call by directly
    std::thread t5(printIDAndName, 888, std::move(str1)); //str1 will not be accessable after join().
    std::thread t6(printID, std::ref(idx)); //str1 will not be accessable after join().
    std::thread t7 = std::thread(&Vehicle::addID, &v1, 666); //!for class member func call
    std::thread t8 = std::thread(&Vehicle::addID, ptr_v2, 777);
    std::cout<<"Finished work in main \n"<< std::endl;

    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    t6.join();
    t7.join();
    t8.join();

    v1.printID();
    ptr_v2->printID();
    idx++;
    auto lambda_1 = []() {std::cout << "[] with empty capture list "<< std::endl;};
    auto lambda_2 = [&idx]() {std::cout << "[] ref ID is "<< idx << std::endl;};
    auto lambda_3 = [idx]() mutable {std::cout << "[] Mutable new ID is "<< ++idx << std::endl;};//not modify idx ,just a copy. &idx will modify
    auto lamdba_4 = [](const int &id) {std::cout << "[] the passed ID parameter is "<< id << std::endl;};

    lambda_1();
    lambda_2();
    lambda_3();
    lamdba_4(idx);

    std::future<void> ftr = std::async([](Vehicle v) {v.setName("My New Set Name");}, std::move(v2));
    std::vector<std::thread> threads;
    for (size_t i = 0; i < 10; ++i)
    {
        // create new thread from a Lambda
        threads.emplace_back([i]() {

            // wait for certain amount of time
            std::this_thread::sleep_for(std::chrono::milliseconds(10 * i));

            // perform work
            std::cout << "Hello from Worker thread #" << i << std::endl;
        });
    }

    // do something in main()
    std::cout << "Hello from Main thrqead" << std::endl;

    // call join on all thread objects using a range-based loop
    for (auto &t : threads)
        t.join();
    ftr.wait();
    v2.printID();

        
    return 0;
    

}