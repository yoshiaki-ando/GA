#include <iostream>
#include <fstream>
#include <cmath>
#include <random>

#include "GA.h"
#include "agent.h"

int main(void){
    
      std::random_device rnd;
      std::mt19937 mt(rnd());
    //std::mt19937 rnd(1); 

    double max_beta_1, max_beta_2, max_h_prime_1, max_h_prime_2;  /*最終世代スコアの最大値を判断*/ 
    double MAX[Number_of_Generation + 1];
    double score_average[Number_of_Generation + 1];
      Agent agent[2][Number_of_Individual];

    /*初期ランダム遺伝子の作成*/
    for(int i = 0; i < Number_of_Individual; i++){
        for(int n = 0; n < N_bit_total; n++){
            agent[0][i].Gene[n] = rnd() % 2;
            //std::cout << agent[0][i].Gene[n] << " "; 
        }
        //std::cout << std::endl;
    }
    
    

    for(int n_generation = 0; n_generation < Number_of_Generation - 1; n_generation++){
        const int PARENT { n_generation % 2 };
        const int CHILD { (n_generation + 1) % 2};
        for(int i = 0; i < Number_of_Individual; i++){
            
            agent[PARENT][i].set_parameter(agent[PARENT][i].Gene);  /* 2進数から10進数に変換*/

            agent[PARENT][i].score
                    = fitting( agent[PARENT][i].parameter_beta_1, agent[PARENT][i].parameter_beta_2,
                            agent[PARENT][i].parameter_h_prime_1, agent[PARENT][i].parameter_h_prime_2); 
                    /*FDTDの計算,返値がスコア*/
        }
        
        
        /*スコア順にソート*/
        sort_ind(agent[PARENT]);
        MAX[n_generation] = agent[PARENT][0].score;
        
        
        // std::cout << " beta_1 = " << max_beta_1 << " beta_2 = " << max_beta_2 
        //           << " h_prime_1 = " << max_h_prime_1 << " h_prime_2 = " << max_h_prime_2
        //           << " score= " << max_1 <<  std::endl; 

        /*ルーレット作成*/
        double roulette[Number_of_Individual];
        compose_roulette(Number_of_Individual, agent[PARENT], roulette, score_average, n_generation);    
        
        /*選択と交叉*/
        for(int i = 0; i < Number_of_Individual; i+=2){
            int sict[2];
            for(int j = 0; j <2 ; j++){
                double rnd_num = rnd() / i32;
                int k = 0;
                while( roulette[k] < rnd_num){  /*親を2体選ぶルーレット*/
                //std::cout << "k= " << k << " " << roulette[k] << " " << rnd_num << std::endl;
                    k++;
                }
                //std::cout << "k= " << k <<" " << roulette[k] << " " << rnd_num << std::endl; 
                sict[j] = k;
                //std::cout << "k= " << k << std::endl;
                //std::cout << "sict[" << j << "]= " << sict[j] << std::endl << std::endl; 
            }
            
            crossover(i, agent[PARENT], agent[CHILD], sict); /*交叉*/
        }
        
        
        

        /*突然変異*/
        for(int i = 0; i < Number_of_Individual; i++){
            for (int j = 0; j < N_bit_total; j++){
                if(rnd() / i32 < MUTATION ){
                    agent[CHILD][i].Gene[j] ^= 1;  /*0と1を反転*/
                }
            }
        }
        
    }
    
    

    /*最終世代の最もスコアが高いものを判断*/
    const int PARENT { (Number_of_Generation - 1) % 2 };
    double sum = 0.0;

    for(int i = 0; i < Number_of_Individual; i++){
            agent[PARENT][i].set_parameter(agent[PARENT][i].Gene);  /* 2進数から10進数に変換*/
            agent[PARENT][i].score
                    = fitting( agent[PARENT][i].parameter_beta_1, agent[PARENT][i].parameter_beta_2,
                            agent[PARENT][i].parameter_h_prime_1, agent[PARENT][i].parameter_h_prime_2); 
                    /*FDTDの計算,返値がスコア*/
                    sum += agent[PARENT][i].score;
                    sort_ind(agent[PARENT]);
    }
    MAX[Number_of_Generation - 1] = agent[PARENT][0].score;
    score_average[Number_of_Generation - 1] = sum / (Number_of_Individual - 1);
    max_beta_1 = agent[PARENT][0].parameter_beta_1;
    max_beta_2 = agent[PARENT][0].parameter_beta_2;
    max_h_prime_1 = agent[PARENT][0].parameter_h_prime_1;
    max_h_prime_2 = agent[PARENT][0].parameter_h_prime_2;

    std::ofstream ofs("../data/" "score_graph");
    for(int n_generation = 0; n_generation < Number_of_Generation; n_generation++){
     ofs << n_generation << " " << MAX[n_generation] << " " << score_average[n_generation] << std::endl;
    }

    std::cout << "beta_1= " << max_beta_1 << " beta_2= " << max_beta_2  << " h_prime_1= " << max_h_prime_1  
              << " h_prime_2= " << max_h_prime_2 << " max= " << MAX[Number_of_Generation - 1] << std::endl;

    return 0;

}
