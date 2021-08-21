#include <stdlib.h>
#include "config.h"
#include "chieftain.h"
#include "valhalla.h"
#include <semaphore.h>

sem_t sem_r_table, sem_a_table;

void chieftain_init(chieftain_t *self, valhalla_t *valhalla)
{
    self->chairsList = (int*) malloc(sizeof(int) * (config.table_size+1));

    sem_init(&sem_r_table, 0, config.table_size+1);

    sem_init(&sem_a_table, 0, config.table_size+1);

    self->valhalla = valhalla;

    for (int i = 0; i <= config.table_size; i++)
    {
        self->chairsList[i] = 0;
    }

    plog("[chieftain] Initialized\n");
}

int chieftain_acquire_seat_plates(chieftain_t *self, int berserker)
{
    /* TODO: Implementar! */
    int vikingChair = 0;
    int vikingType = berserker + 1;
    int table_max_index = config.table_size;

    sem_wait(&sem_a_table);

    if (self->chairsList[0] == 0 && (self->chairsList[1] == vikingType || self->chairsList[1] == 0))
    {
        vikingChair = 0;
        self->chairsList[0] = vikingType;
        printf("entrou na primeira cadeira\n");
        sem_post(&sem_a_table);
        return 1;
    }

    for (int i = 1; i < table_max_index; i++)
    {
        if (self->chairsList[i] == 0)
        {
            if ((self->chairsList[i + 1] == vikingType || self->chairsList[i + 1] == 0) && (self->chairsList[i - 1] == vikingType || self->chairsList[i - 1] == 0))
            {
                vikingChair = i;
                self->chairsList[i] = vikingType;
                printf("entrou nas cadeiras posteriores %d, viking: %d\n", i+1, vikingType);
                sem_post(&sem_a_table);
                return vikingChair;
            }
        }
    }

    if (self->chairsList[table_max_index] == 0 && (self->chairsList[table_max_index - 1] == vikingType || self->chairsList[table_max_index - 1] == 0))
    { 
        vikingChair = table_max_index;
        self->chairsList[table_max_index] = vikingType;
        printf("entrou na última cadeira\n");
        sem_post(&sem_a_table);
        return vikingChair;
    }
    sem_post(&sem_a_table);
    return vikingChair;
}

void chieftain_release_seat_plates(chieftain_t *self, int pos)
{
    sem_wait(&sem_r_table);
    printf("saiu das cadeira %d\n", pos);
    self->chairsList[pos] = 0;
    sem_post(&sem_r_table);
}

god_t chieftain_get_god(chieftain_t *self)
{
    /* TODO: Implementar! O código abaixo deve ser modificado! */
    god_t god = THOR;

    return god;
}

void chieftain_finalize(chieftain_t *self)
{
    free(self->chairsList);
    plog("[chieftain] Finalized\n");
}


