#include <random>

#include "GA.h"
#include "agent.h"

double fitting(double parameter_beta_1, double parameter_beta_2, 
            double parameter_h_prime_1, double parameter_h_prime_2){
    double v; /*score*/
    double beta[3], h_prime[3];
    double **s = new double*[3];
    for(int i=0; i < 3; i++){
        s[i] = new double[900];
    }
    double **S = new double*[3];
    for(int i=0; i < 3; i++){
        S[i] = new double[1900];
    }
    double time[3];
    time[0] = 6.1667;
    time[1] = 6.313;
    time[2] = 6.5; 
    beta[0] = 0.49366;
    h_prime[0] = 77.69128;

    for(int t = 1; t <= 2; t++){
        beta[t] = parameter_beta_2 * pow((time[t] - time[0]), 2) + parameter_beta_1 * (time[t] - time[0]) + beta[0];
        h_prime[t] = parameter_h_prime_2 * pow((time[t] - time[0]), 2) + parameter_h_prime_1 * (time[t] - time[0]) + h_prime[0];
        input(S,t);
        s = cal_fdtd(beta[t], h_prime[t], t); /*betaとh'を代入して電界を返す*/
    }
    
    // v = std::exp( - std::pow((parameter_beta_1 - u_beta_1), 2) - std::pow((parameter_beta_2 - u_beta_2), 2)
    //          - std::pow((parameter_h_prime_1 - u_h_prime_1), 2) - std::pow((parameter_h_prime_2 - u_h_prime_2), 2));

    for(int i = 0; i < Nr_1; i++){
        for(int m = 1; m < M; m++){
            v += 1/ (Nr_1 * (M - 1)) * std::pow(std::abs( S[i][m] - s[i][m] ), 2)
                 + p_beta * std::pow(std::abs( beta[m] - beta[0]), 2) 
                 + p_h_prime * std::pow(std::abs( h_prime[m] - beta[0]), 2);

        }
    }
    

    return v;
}

void cal_ind(Agent *p){
    for(int i = 0; i < Number_of_Individual; i++){
            p[i].set_parameter(p[i].Gene);  /* 2進数から10進数に変換*/
            p[i].score  /*FDTDの計算,返値がスコア*/
                    = fitting( p[i].parameter_beta_1, p[i].parameter_beta_2,
                               p[i].parameter_h_prime_1, p[i].parameter_h_prime_2); 
                    
        }
}

void create_ind(Agent *agent){
    std::random_device rnd;
    std::mt19937 mt(rnd());
    //std::mt19937 rnd(1); 
    for(int i = 0; i < Number_of_Individual; i++){
        for(int n = 0; n < N_bit_total; n++){
            agent[i].Gene[n] = rnd() % 2;
            //std::cout << agent[0][i].Gene[n] << " "; 
        }
        //std::cout << std::endl;
    }
}




void compose_roulette(const int N, Agent *agent, double *roulette, double *score_average, int n_generation){/*ルーレット作成*/
    double sum = 0.0; 
    for(int i = 0; i < Number_of_Individual; i++){
        sum += agent[i].score;
        //std::cout << "agent[" << i << "].score= " << agent[i].score << std::endl;
    }    
    score_average[n_generation] = sum / Number_of_Individual;
    //std::cout << "sum_average= " << sum_1 << std::endl;
    //std::cout << std::endl;
    //std::cout << "sum= " << sum << std::endl;
    roulette[0] = agent[0].score / sum;
    //std::cout << "roulette[0]= " << roulette[0] << std::endl;
    for(int i = 1; i < Number_of_Individual ; i++){
        roulette[i] = roulette[i-1] + agent[i].score / sum;
        // std::cout << "roulette[" << 3 << "]= " << roulette[3] << std::endl;
    }
}

void crossover(int head, Agent *p, Agent *c, int *s){ /*交叉*/
    //std::mt19937 rnd(1); 
    std::random_device rnd;
    std::mt19937 mt(rnd());
  
    for(int i = 0; i < N_bit_total; i++){
        if(rnd() / i32 < 0.5){  /*入れ替えなし*/  
            c[head].Gene[i] = p[s[0]].Gene[i];  
            c[head+1].Gene[i] = p[s[1]].Gene[i];
        }
        else{                   /*入れ替えあり*/
            c[head].Gene[i] = p[s[1]].Gene[i];
            c[head+1].Gene[i] = p[s[0]].Gene[i];
        }
    }
}

void selection_crossover(double *roulette, Agent *p, Agent *c){
    std::random_device rnd;
    std::mt19937 mt(rnd());
    //std::mt19937 rnd(1);
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
            crossover(i, p, c, sict); /*交叉*/
        }
}

void mutate_ind(Agent *c){
    std::random_device rnd;
    std::mt19937 mt(rnd());
    //std::mt19937 rnd(1);
    for(int i = 0; i < Number_of_Individual; i++){
        for (int j = 0; j < N_bit_total; j++){
            if(rnd() / i32 < MUTATION ){
                c[i].Gene[j] = !c[i].Gene[j];  /*0と1を反転*/
            }
        }
    }
}

void final_cal_ind(Agent *p, double *max_paramter, double *MAX, double *score_average){
    double sum = 0.0;
    for(int i = 0; i < Number_of_Individual; i++){
        p[i].set_parameter(p[i].Gene);  /* 2進数から10進数に変換*/
        p[i].score/*FDTDの計算,返値がスコア*/
            = fitting( p[i].parameter_beta_1, p[i].parameter_beta_2,
                       p[i].parameter_h_prime_1, p[i].parameter_h_prime_2); 
        sum += p[i].score;
        sort_ind(p);
    }
    MAX[Number_of_Generation - 1] = p[0].score;
    score_average[Number_of_Generation - 1] = sum / (Number_of_Individual - 1);
    max_paramter[0] = p[0].parameter_beta_1;
    max_paramter[1] = p[0].parameter_beta_2;
    max_paramter[2] = p[0].parameter_h_prime_1;
    max_paramter[3] = p[0].parameter_h_prime_2;
}

void sort_ind(Agent *p){
    Agent tmp;
    for(int i = 0; i < Number_of_Individual - 1; i++){
        for(int j = i + 1 ; j < Number_of_Individual; j++){
            if( p[i].score < p[j].score ){
                tmp = p[i];
                p[i] = p[j];
                p[j] = tmp;

            } 
        }
    }
}


/*2進数→10進数変換*/
int bin2dec(const int N_bit_initial, const int N_bit_end, bool *binary){
    int v { 0 };
    int base { 1 };

    for(int i = N_bit_end - 1; i >= N_bit_initial; i--){
        v += int(binary[i]) * base;
        base *= 2;
    }
    return v;
}

