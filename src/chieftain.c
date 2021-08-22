#include <stdlib.h>
#include "config.h"
#include "chieftain.h"
#include "valhalla.h"
#include <semaphore.h>

sem_t sem_r_table, sem_a_table;

void chieftain_init(chieftain_t *self, valhalla_t *valhalla)
{
    self->chairsList = (int *)malloc(sizeof(int) * (config.table_size));

    self->valhalla = valhalla;

    for (int i = 0; i < config.table_size; i++)
    {
        self->chairsList[i] = 3; // inicialmente, todas as cadeiras são lugares vazios com um prato (3)
        // OBS: Cadeiras vazias sem prato podem ser elementos com o número 4 ou 5 (dependendo em qual sentido seu pratos foram pegos pelos vinkings)
    }

    plog("[chieftain] Initialized\n");
}

int chieftain_acquire_seat_plates(chieftain_t *self, int berserker)
{
    int vikingChair = 0;                    // OBS: -1 para o viking chair fará o programa dar ruim caso a variável mantenha esse valor após passar pela lógica abaixo (o que não deve ocorrer)
    int vikingType = berserker + 1;          // o tipo do meu viking: 2 para berserker e 1 para um viking comum
    int otherVikingType = berserker ? 1 : 2; // o tipo do viking que não é o mesmo que o meu: se eu for berserker, o outro tipo é 1, caso eu não seja, então o outro tipo é berserker (2)
    int table_max_index = config.table_size - 1;


    if (self->chairsList[0] == 3) // a primeira posição está vazia e com um prato?
    {
        if (self->chairsList[1] == 3) // a próxima cadeira está vazia e com um prato?
        {
            // então posso me sentar na primeira posição e pegar mais um prato na segunda posição.
            vikingChair = 0;
            self->chairsList[0] = vikingType; // a primeira posição fica ocupada "comigo"
            self->chairsList[1] = 5;          // a segunda posição (da qual eu peguei meu segundo prato) é colocada como uma cadeira vazia sem prato [com um número para indicar quem pegou o prato dela (algúem em sentido horário = 5)]
            return vikingChair;
        }
        // caso a próxima cadeira não esteja vazia E COM UM PRATO.
        else if (self->chairsList[table_max_index] == 3 && self->chairsList[1] != otherVikingType) // a última cadeira (separada de mim pelo vão) deve estar vazia e com um prato. E a próxima cadeira (depois de mim) deve ter um viking do meu tipo, ou estar vazia (sabidamente sem prato).
        {
            vikingChair = 0;
            self->chairsList[0] = vikingType;      // a primeira posição fica ocupada "comigo"
            self->chairsList[table_max_index] = 4; // a última posição é colocada como uma cadeira vazia sem prato [com um número para indicar quem pegou o prato dela (algúem em sentido anti-horário = 4)]
            return vikingChair;
        }
    }
    // caso a primeira cadeira não esteja vazia E COM UM PRATO ou não ser possível respeitar alguma das outras condições para que eu pudesse sentar nela (ter um outro prato por perto, não ter vikigns de outro tipo na segunda posição).
    for (int i = 1; i < table_max_index; i++) // para cada cadeira na mesa, tirando a primeira e a última (as quais são casos especiais)
    {
        if (self->chairsList[i] == 3) // a cadeira que eu estou tentando sentar está vazia E COM UM PRATO?
        {
            if (self->chairsList[i + 1] == 3 && self->chairsList[i - 1] != otherVikingType) // a cadeira depois da que eu estou tentando sentar está vazia e com um prato? E a cadeira antes da que eu estou tentando sentar ou está vazia (podendo ou não conter um prato) ou está ocupada por algum viking do meu tipo?
            {
                // então posso me sentar na posição i e pegar mais um prato na posição i+1
                vikingChair = i;
                self->chairsList[i] = vikingType; // a posicão i fica ocupada "comigo"
                self->chairsList[i + 1] = 5;      // a cadeira depois da que eu estou (da qual eu peguei meu segundo prato) é colocada como uma cadeira vazia sem prato [com um número para indicar quem pegou o prato dela (algúem em sentido horário = 5)]
                return vikingChair;
            }
            // caso a cadeira i+1 não esteja vazia E COM UM PRATO.
            else if (self->chairsList[i - 1] == 3 && self->chairsList[i + 1] != otherVikingType) // a cadeira ANTES da que eu estou tentando sentar está vazia e com um prato? E a cadeira DEPOIS da que eu estou tentando sentar ou está vazia (sabidamente sem prato) ou está ocupada por algum viking do meu tipo?
            {
                vikingChair = i;
                self->chairsList[i] = vikingType; // a posicão i fica ocupada "comigo"
                self->chairsList[i - 1] = 4;      // a cadeira antes da que eu estou (da qual eu peguei meu segundo prato) é colocada como uma cadeira vazia sem prato [com um número para indicar quem pegou o prato dela (algúem em sentido anti-horário = 4)]
                return vikingChair;
            }
        }
    }

    // caso não tenha sido possível pegar qualquer cadeira até a penúltima.
    if (self->chairsList[table_max_index] == 3) // a última posição está vazia e com um prato?
    {
        if (self->chairsList[table_max_index - 1] == 3) // e a cadeira anterior (penúltima) está vazia e com um prato?
        {
            // então posso me sentar na última posição e pegar mais um prato na penúltima posição.
            vikingChair = table_max_index;
            self->chairsList[table_max_index] = vikingType; // a última posição fica ocupada "comigo"
            self->chairsList[table_max_index - 1] = 4;      // a penúltima posição (da qual eu peguei meu segundo prato) é colocada como uma cadeira vazia sem prato [com um número para indicar quem pegou o prato dela (algúem em sentido anti-horário = 4)]
            return vikingChair;
        }
        // caso a penúltima cadeira não esteja vazia E COM UM PRATO.
        else if (self->chairsList[0] == 3 && self->chairsList[table_max_index - 1] != otherVikingType) // a primeira cadeira (separada de mim pelo vão) deve estar vazia e com um prato. E a penúltima cadeira deve ter um viking do meu tipo, ou estar vazia (sabidamente sem prato).
        {
            vikingChair = table_max_index;
            self->chairsList[table_max_index] = vikingType; // a última posição fica ocupada "comigo"
            self->chairsList[0] = 5;                        // a primeira posição (da qual eu peguei meu segundo prato) é colocada como uma cadeira vazia sem prato [com um número para indicar quem pegou o prato dela (algúem em sentido horário = 5)]
            return vikingChair;
        }
    }

    return vikingChair;
}

void chieftain_release_seat_plates(chieftain_t *self, int pos)
{
    int table_max_index = config.table_size - 1;

    if (pos > 0 && pos < table_max_index) // para garantir que não tentemos acessar os indexes "0 - 1" e "table_max_index + 1"
    {
        if (self->chairsList[pos - 1] == 4) // o index antes da minha posição teve o prato pego no sentido anti horário?
        {
            self->chairsList[pos - 1] = 3; // então quer dizer que eu que peguei o prato dele e eu devolvo para aquela cadeira
        }
        else if (self->chairsList[pos + 1] == 5) // o index depois da minha posição teve o prato pego no sentido horário?
        {
            self->chairsList[pos + 1] = 3; // então quer dizer que eu que peguei o prato dele e eu devolvo para aquela cadeira
        }
    }
    else if (pos == 0)
    {
        if (self->chairsList[table_max_index] == 4) // OBS: o index considerado anterior a posição 0 é o último
        {
            self->chairsList[table_max_index] = 3;
        }
        else if (self->chairsList[1] == 5)
        {
            self->chairsList[1] = 3;
        }
    }
    else
    {
        if (self->chairsList[table_max_index - 1] == 4)
        {
            self->chairsList[table_max_index - 1] = 3;
        }
        else if (self->chairsList[0] == 5) // OBS: o index considerado posterior a última posição é o index 0
        {
            self->chairsList[0] = 3;
        }
    }
    printf("POSIÇÃO POS %d \n", pos);
    printf("TABLE SIZE %d \n", config.table_size);
    self->chairsList[pos] = 3;

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
