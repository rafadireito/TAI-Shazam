
#include <vector>



class Point {

    private:
        std::vector<short> block;

        size_t clusterId, pointId;

    public:
        
        Point(std::vector<short> block, size_t id){
            this->block = block;
            pointId = id;
            clusterId = 0;

        }

        size_t getClusterId(){
            return clusterId;
        }

        void setClusterId(size_t index){
            clusterId = index;
        }

        size_t getId(){
            return pointId;
        }

        size_t getBlockSize(){
            return block.size();
        }

        short getBlockByValue(size_t pos){
            return block[pos];
        }

        std::vector<short> getBlock(){
            return block;
        }
};
