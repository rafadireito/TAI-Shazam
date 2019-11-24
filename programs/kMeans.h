#include "cluster.h"
#include <algorithm>
#include <thread>
#include <pthread.h>
#include <mutex>

class KMeans{


    private:
        std::vector<Cluster> clusters;
        std::vector<Point> points; 
        size_t blockSize, k;
        int iterations;
        bool done;
        std::mutex m;
    
    public:
        void updateNearestCluster(size_t start, size_t end){

            for(size_t point = start; point < end; point++){

                double min_dist, sum = 0.0, dist;
                
                for(size_t position = 0; position < blockSize; position++){

                    sum += pow(clusters[0].getCentroidByPos(position) - points[point].getBlockByValue(position), 2.0);
                
                }
                min_dist = sqrt(sum);
                size_t nearestClusterId = clusters[0].getId();

                for(size_t cluster = 1; cluster < k; cluster ++){
                    
                    sum = 0.0;
                    
                    for(size_t position = 0; position < blockSize; position++){
                        
                        sum += pow(clusters[cluster].getCentroidByPos(position) - points[point].getBlockByValue(position), 2);
                    }

                    dist = sqrt(sum);
                    
                    if(dist < min_dist){
                        min_dist = dist;
                        nearestClusterId = clusters[cluster].getId();
                    }
                }
                size_t previousClusterId = points[point].getClusterId();
                    
                if(nearestClusterId != previousClusterId){
                    if(previousClusterId != 0){
                        for(size_t cluster = 0; cluster < k; cluster++){
                            if(clusters[cluster].getId() == previousClusterId){
                                m.lock();
                                clusters[cluster].removePoint(point) ;
                                m.unlock();
                            }
                        }
                    }

                    for(size_t cluster = 0; cluster < k; cluster++){
                        if(clusters[cluster].getId() == nearestClusterId){
                            m.lock();
                            clusters[cluster].addPoint(point);
                            m.unlock();
                            points[point].setClusterId(nearestClusterId);
                            
                        }
                    } 
                    this->done = false;
                }
            };
        }

        void updateCentroids(size_t start, size_t end){

            for(size_t cluster = start; cluster < end; cluster++){

                size_t clusterNBlocks = clusters[cluster].getNBlocks();
                
                for(size_t value = 0; value < blockSize; value++){
                    
                    double sum = 0.0;

                    if(clusterNBlocks > 0){
                        
                        for(size_t block = 0; block < clusterNBlocks; block++){

                            sum += points[clusters[cluster].getBlock(block)].getBlockByValue(value);
                            
                        }

                        clusters[cluster].setCentroidByPos(value, sum / clusterNBlocks);
                    }
                }
            }
        }

        KMeans(size_t k, int iterations){
            this->k = k;
            this->iterations = iterations;
        }       

        std::vector<std::vector<short>> getClusters(std::vector<std::vector<short>> blocks, int nThreads){
            
            std::thread threads[nThreads];    

            blockSize = blocks[0].size();

            for(size_t i = 0; i < blocks.size(); i++){
                Point p(blocks[i], i);
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
            int pointsStep = points.size() / nThreads;
            int clustersStep = k / nThreads;

            while(true){
                done = true;

                /*
                  Atualiza as atribuições dos pontos aos clusters
                */
                std::cout << "Atribute Clusters" << std::endl;

                for(int i = 0; i < nThreads; i++){

                    if(i == nThreads -1){
                        size_t start = i*pointsStep;
                        size_t end = points.size();
                        threads[i] = std::thread(&KMeans::updateNearestCluster, this, start, end);
                    }
                    else{
                        size_t start = i*pointsStep;
                        size_t end = i*pointsStep + pointsStep;
                        threads[i] = std::thread(&KMeans::updateNearestCluster, this, start, end);
                    }
                }
                for(int i = 0; i < nThreads; i++){
                    threads[i].join();
                }


                /*
                  Atualizar os centroids de acordo com as novas atribuições.
                */
                std::cout << "Update Centroids" << std::endl;
                for(int i = 0; i < nThreads; i++){
                    if(i == nThreads -1){
                        size_t start = i*clustersStep;
                        size_t end = clusters.size();
                        threads[i] = std::thread(&KMeans::updateCentroids, this, start, end);
                    }
                    else{
                        size_t start = i*clustersStep;
                        size_t end = i*clustersStep + clustersStep;
                        threads[i] = std::thread(&KMeans::updateCentroids, this, start, end);
                    }
                }
                for(int i = 0; i < nThreads; i++){
                    threads[i].join();
                }

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

