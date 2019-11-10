#include "cluster.h"
#include <algorithm>

class KMeans{


    private:
        std::vector<Cluster> clusters;
        std::vector<Point> points; 
        size_t sampleSize, k;
        int iterations;

        size_t getNearestClusterId(Point p){

            double min_dist, sum = 0.0, dist;
            
            
            for(size_t i = 0; i < sampleSize; i++){

                sum += pow(clusters[0].getCentroidByPos(i) - p.getSampleValue(i), 2.0);
            
            }
            
            min_dist = sqrt(sum);
            size_t nearestClusterId = clusters[0].getId();

            
            for(size_t i = 1; i < k; i ++){
                
                sum = 0.0;
                
                for(size_t j = 0; j < sampleSize; j++){
                    
                    sum += pow(clusters[i].getCentroidByPos(i) - p.getSampleValue(i), 2);
                }

                dist = sqrt(sum);
                
                if(dist < min_dist){
                    min_dist = dist;
                    nearestClusterId = clusters[i].getId();
                }
            }

            return nearestClusterId;
        }

        void updateCentroids(){

            for(size_t i = 0; i < k; i++){
                size_t ClusterSize = clusters[i].getClusterSize();
                
                for(size_t j = 0; j < sampleSize; j++){
                    
                    double sum = 0.0;

                    if(ClusterSize > 0){
                        
                        for(size_t p = 0; p < ClusterSize; p++){

                            sum += points[clusters[i].getPoint(p)].getSampleValue(j);
                            
                        }

                        clusters[i].setCentroidByPos(j, sum / ClusterSize);
                    }
                }
            }
        }

    public:
        KMeans(size_t k, int iterations){
            this->k = k;
            this->iterations = iterations;
        }       

        std::vector<std::vector<short>> getKMeansClustering(std::vector<std::vector<short>> samples){

            sampleSize = samples[0].size();

            for(size_t i = 0; i < samples.size(); i++){
                Point p(samples[i], i);
                points.push_back(p);
            }

            /*
              Initializing Clusters.
            */

            std::cout << "Initializing Clusters" << std::endl;
            
            srand(time(NULL));

            std::vector<size_t> usedPoints;
            

            for(size_t i = 1; i <= k; i++){
                while(true){
                    size_t randInd = rand() % points.size();
                    
                    if(std::find(usedPoints.begin(), usedPoints.end(), randInd) == usedPoints.end()){

                        usedPoints.push_back(randInd);
                    
                        points[randInd].setClusterId(i);

                        Cluster c(points[randInd], i);
                        
                        points[randInd].setClusterId(i);

                        clusters.push_back(c);

                        break;
                    }

                }
            }

            std::cout << "Clusters Initialized" << std::endl;
            
            int iter = 0;

            while(true){
                bool done = true;

                /*
                  Atualiza as atribuições dos pontos aos clusters
                */
                std::cout << "Atribute Clusters" << std::endl;

                for(size_t i = 0; i < points.size(); i++){

                    size_t newClusterId = this->getNearestClusterId(points[i]);
                    size_t previousClusterId = points[i].getClusterId();
                    
                    if(newClusterId != previousClusterId){
                        if(previousClusterId != 0){
                            for(size_t j = 0; j < k; j++){

                                if(clusters[j].getId() == previousClusterId){
                                    clusters[j].removePoint(i) ;
                                }
                            }
                        }

                        for(size_t j = 0; j < k; j++){
                            if(clusters[j].getId() == newClusterId){

                                clusters[j].addPoint(i);
                                points[i].setClusterId(newClusterId);
                            }
                        } 

                        done = false;
                    }
                }


                /*
                  Atualizar os centroids de acordo com as novas atribuições.
                */
                std::cout << "Update Centroids" << std::endl;
                updateCentroids();

                if(done || iter > iterations){
                    std::cout << "Finished KMeans" << std::endl;
                    break;
                }
                iter ++;
            }
            std::vector< std::vector<short>> centroids;
            for(size_t i = 0; i < k; i++){
                centroids.push_back(clusters[i].getCentroid());
            }
            
            return centroids;
        }
};

