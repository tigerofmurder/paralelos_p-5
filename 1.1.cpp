#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <pthread.h>
#include <iostream>
#include <time.h>

#define MAX_RANDOM 65535

using namespace std;

struct list_node_s {
   int    data;
   struct list_node_s* next;
};

int  Member(int value, struct list_node_s* head_p) {
	struct list_node_s* curr_p = head_p;

	while(curr_p != NULL && curr_p->data < value)
		curr_p = curr_p->next;

	if(curr_p == NULL || curr_p->data > value) {
		return 0;
	} else {
		return 1;
	}
}

int Insert(int value, struct list_node_s** head_p) {
	struct list_node_s* curr_p = *head_p;
	struct list_node_s* pred_p = NULL; //Puntero a anterior
	struct list_node_s* temp_p;

	while(curr_p != NULL && curr_p->data < value) {
		pred_p = curr_p;
		curr_p = curr_p->next;
	}

	if(curr_p == NULL || curr_p->data > value) {
		temp_p = (struct list_node_s*)malloc(sizeof(struct list_node_s));
		temp_p->data = value;
		temp_p->next = curr_p;
		if(pred_p == NULL){
			*head_p = temp_p;
		}else{
			pred_p->next = temp_p;
		}
		return 1;
	}else{
		return 0;
	}
}

int Delete(int value, struct list_node_s** head_p) {
	struct list_node_s* curr_p = *head_p;
	struct list_node_s* pred_p = NULL;

	while(curr_p != NULL && curr_p->data < value) {
		pred_p = curr_p;
		curr_p = curr_p->next;
	}

	if (curr_p != NULL && curr_p->data == value) {
		if (pred_p == NULL){
			*head_p = curr_p->next;
			free(curr_p);
		}else{
			pred_p->next = curr_p->next;
			free(curr_p);
		}
		return 1;
	}else{
		return 0;
	}
}



int n = 1000; //Numero de Nodos
int m = 100000; //Numero de operaciones
int thread_count = 1; //Numero de threads en ejecucion

float mMember = 0.999;
float mInsert = 0.0005;
float mDelete = 0.0005;

//Operaciones Total
int m_member = mMember*m;
int m_insert = mInsert*m;
int m_delete = mDelete*m;

//Contador de operaciones por thread
int count_member = 0;
int count_insert = 0;
int count_delete = 0;

struct list_node_s* head = NULL;
pthread_rwlock_t rwlock;


void *Thread_Operation(void *thread_id) {
	int count_tot = 0;

    int finished_member = 0;
    int finished_insert = 0;
    int delete_finished = 0;

    while (count_tot < m) {
        int random_value = rand() % MAX_RANDOM;

        int random_select = rand() % 3;

		//cout<<random_select<<" "<<count_member<<" "<<count_insert<<" "<<count_delete<<endl;
        if (random_select == 0 && finished_member == 0) {
            pthread_rwlock_rdlock(&rwlock);
            if (count_member < m_member) {
                Member(random_value, head);
                count_member++;
            }else
                finished_member =1;
            pthread_rwlock_unlock(&rwlock);
        }
        if (random_select == 1 && finished_insert == 0) {
            pthread_rwlock_rdlock(&rwlock);
            if (count_insert < m_insert) {
                Insert(random_value, &head);
                count_insert++;
            }else
                finished_insert =1;
            pthread_rwlock_unlock(&rwlock);
        }
        else if (random_select == 2 && delete_finished == 0) {
            pthread_rwlock_wrlock(&rwlock);
            if (count_delete < m_delete) {
                Delete(random_value, &head);
                count_delete++;
            }else
                delete_finished =1;
            pthread_rwlock_unlock(&rwlock);
        }
        count_tot++;
    }
	/*
    int my_rank = *(int *) thread_id;
    int value = 0;
    double tmp_thr;
    unsigned seed  = my_rank+1;
    int count_member = 0;
    int count_insert = 0;
    int count_delete = 0;
    int opt_thr = n/thread_count;//Operacion por Thread
    for (int i = 0;i<opt_thr;i++){
    	tmp_thr = rand() % 100;
    	value = rand() % MAX_RANDOM % m;
    	//cout<<tmp_thr<<" "<<mMember<<" "<<(mMember  + mInsert)*100<<endl;
    	if (tmp_thr< mMember*100) {
			pthread_rwlock_rdlock(&rwlock);
            Member(value, head);
            pthread_rwlock_unlock(&rwlock);
            count_member++;
		} else if (tmp_thr < (mMember  + mInsert)*100) {
			pthread_rwlock_rdlock(&rwlock);
            Insert(value, &head);
            pthread_rwlock_unlock(&rwlock);
            count_insert++;
		} else {
			pthread_rwlock_wrlock(&rwlock);
            Delete(value, &head);
            pthread_rwlock_unlock(&rwlock);
            count_delete++;
		}
		m_member += count_member;
		m_insert += count_insert;
		m_delete += count_delete;
    }
    return NULL;*/
}


int main(int argc, char *argv[]) {
    double start_time, finish_time, time_elapsed;

    int i = 0;
    while (i < n) {
        if (Insert(rand() % MAX_RANDOM, &head) == 1)
            i++;
    }

    pthread_rwlock_init(&rwlock, NULL);

    pthread_t ids[thread_count];
	start_time = clock();

    for (int i=0; i < thread_count; i++) {
        pthread_create(&ids[i], NULL, Thread_Operation, &i);
    }
    for (int i=0; i < thread_count; i++) {
        pthread_join(ids[i], NULL);
    }
    finish_time = clock();
	time_elapsed = (finish_time - start_time)/CLOCKS_PER_SEC;
	printf("%.10f\n", time_elapsed);
    pthread_rwlock_destroy(&rwlock);
	cout<<m_member<<" "<<m_insert<<" "<<m_delete<<endl;

    return 0;
}
