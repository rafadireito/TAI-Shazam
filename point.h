
#include <vector>



class Point {

    private:
        std::vector<short> sample;

        size_t clusterId, pointId;

    public:
        
        Point(std::vector<short> sample, size_t id){
            this->sample = sample;
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

        size_t getSampleSize(){
            return sample.size();
        }

        short getSampleValue(size_t pos){
            return sample[pos];
        }

        std::vector<short> getSample(){
            return sample;
        }
};
