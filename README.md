## Sample

    ```cpp
    #include <iostream>
    class Job {
    public:
        bool operator () (int v, std::string s) {
            std::cout << "hello job one" << " v: " << v << " s:" << s << std::endl;
    
            return 1;
        }
    };
    
    int main() {
        Job job;
    
        jdai::ThreadPool threadPool(2);
        auto res = threadPool.addTask(job, 1, "a");
        std::cout << res.get() << std::endl;
        return 0;
    }
    ```
