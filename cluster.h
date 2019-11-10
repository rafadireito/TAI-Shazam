#include "point.h"

class Cluster{

    private:
        std::vector<short> centroid;

        std::vector<size_t> points;

        size_t clusterId;
        
    public:
        
        Cluster( Point centroid, size_t clusterId){
            this->clusterId = clusterId;

            for(size_t i=0; i < centroid.getSampleSize(); i++){

                this->centroid.push_back(centroid.getSampleValue(i));

            }

            this->addPoint(centroid.getId());
        }

        void addPoint(size_t pointId){
            points.push_back(pointId);
        }

        bool removePoint(size_t pointId){

            for(size_t i = 0; i < points.size(); i++){

                if(points[i] == pointId){
                    points.erase(points.begin() + i);
                    return true;
                }

            }

            return false;
        }
        
        size_t getId(){
            return clusterId;
        }

        size_t getPoint(size_t pos){
            return points[pos];
        }
        
        short getCentroidByPos(size_t pos){
            return centroid[pos];
        }

        std::vector<short> getCentroid(){
            return centroid;
        }

        void setCentroidByPos(int pos, short val){
            this->centroid[pos] = val;
        }

        size_t getClusterSize(){
            return points.size();
        }
};
