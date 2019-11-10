#include "point.h"

class Cluster{

    private:
        std::vector<short> centroid;

        std::vector<Point> points;

        size_t clusterId;
        
    public:
        
        Cluster( Point centroid, size_t clusterId){
            this->clusterId = clusterId;

            for(size_t i=0; i < centroid.getSampleSize(); i++){

                this->centroid.push_back(centroid.getSampleValue(i));

            }

            this->addPoint(centroid);
        }

        void addPoint(Point p){
            p.setClusterId(this->clusterId);

            points.push_back(p);
        }

        bool removePoint(Point p){
            size_t nPoints = points.size();

            for(size_t i = 0; i < nPoints; i++){
                if(points[i].getPointId() == p.getPointId()){
                    points.erase(points.begin() + i);
                    return true;
                }
            }

            return false;
        }
        
        size_t getId(){
            return clusterId;
        }

        Point getPoint(size_t pos){
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

        size_t getSize(){
            return points.size();
        }
};
