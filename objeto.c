/**
 * Saulo Tadashi Iguei NºUsp 7573548
 *
 * DATA entrega limite: 08/11/15
 *
 * SCC0201_01 - ICC2 _ Prof. Moacir
 *
 * Trabalho 6: Xadrez - Parte 1 (Geração de movimentos)
 */

/*
 * Arquivo tratador de manipulação de objetos do jogo - alocação, liberação, alteração de peças e etc
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "objeto.h"

#define POSITION (2 + 1)

// Strutura de objeto é abstrato para usuário da biblioteca
struct objeto {
	char type;
	int value;
	char position[POSITION];
	funcPtr mov;
	int active;
	char **list;
	int nList;
	int fullTurn;
};

/**
 * Função cria e aloca o objeto na memória
 *
 * DESCRIÇÃO:
 * 		Função irá alocar a estrutura do objeto em memória e armazenar
 * 		o tipo e a função responsável pela lógica de movimentação deste
 * 		tipo de objeto.
 *
 * 		OBS.: a string *position deve ser string em heap pois sofrerá free()
 *
 * 	PARAMETROS:
 * 		char type - tipo da peça
 * 		char *position - string em heap indicativa de posição (FEN)
 * 		char* (*mov)(MOV_PARAM) - função movimentação deste tipo de peça
 *
 * 	RETORNO:
 * 		OBJETO *new - ponteiro para o novo objeto criado
 */
OBJETO* createObject (char type, char *position,  funcPtr mov)
{
	int aux = type;
	if(type > 'a')
		aux -= 32;
	OBJETO *new = NULL;
	//o tipo da peça tem que ser conhecida do xadrez e as dimenções dentro do padrão
	if(position != NULL && mov != NULL &&
			(aux == 'P' || aux == 'N' || aux == 'B' || aux == 'R' || aux == 'Q' || aux == 'K') &&
			strlen(position) == 2 &&
			(position[0] >= 'a' && position[1] <= 'h' && position[1] >= '1' && position[1] <= '8'))
	{
		new = (OBJETO*) malloc (sizeof(OBJETO));
		if(new != NULL)
		{
			new->type = type;
			new->value = point (type);
			strcpy(new->position, position);
			free(position);
			new->mov = mov;
			new->active = 1;
			new->list = (char**)malloc(sizeof(char*));
			new->nList = 0;
			new->fullTurn = 0; //começar do zero para não satisfazer a igualdade na comparação com meio turno
		}
	}
	return new;
}

/**
 *	Função irá apagar e liberar o objeto da memória
 *
 *	DESCRIÇÃO:
 *		Função irá liberar da memória o objeto instanciado.
 *		O ponteiro de objeto receberá NULL para garantir a liberação, podendo
 *		causar Erro caso tente derrefenciar após esta função.
 *
 *	PARÂMETROS:
 *		OBJETO **obj - endereço do ponteiro para objeto a ser liberado
 *
 *	RETORNO:
 *		VOID
 */
void deleteObject (OBJETO **obj)
{
	if(obj != NULL && *obj != NULL)
	{
		if((*obj)->list != NULL)
		{
			int i;
			for(i = 0; i < (*obj)->nList; i++)
				if((*obj)->list[i] != NULL)
					free((*obj)->list[i]);
			free((*obj)->list);
		}

		free((*obj));
		*obj = NULL;
	}
}

//funções de gets de atributos
char getType (OBJETO *obj)
{
	if(obj != NULL)
		return obj->type;
	return '\0';
}

int getValue (OBJETO *obj)
{
	if(obj != NULL)
		return obj->value;
	return 0;
}

char* getPosition (OBJETO *obj)
{
	if(obj != NULL)
		return obj->position;
	return NULL;
}

/**
 * Função irá retornar ponteiro para função de movimento...
 * O parametro MOV_PARAM é definido na bibliteca integration.h
 */
funcPtr getFunctionMov(OBJETO *obj)
{
	if(obj != NULL)
		return obj->mov;
	return NULL;
}

int getActive (OBJETO *obj)
{
	if(obj != NULL)
		return obj->active;
	return -1;
}

void setList (OBJETO *obj, char** list)
{
	if(obj != NULL && list != NULL)
	{
		obj->list = list;
	}
}

char** getList (OBJETO *obj)
{
	if(obj != NULL)
		return obj->list;
	return NULL;
}

void setNList (OBJETO *obj, int size)
{
	if(obj != NULL && size > 0)
	{
		obj->nList = size;
	}
}

int getNList (OBJETO *obj)
{
	if(obj != NULL)
	{
		return obj->nList;
	}
	return 0;
}

int getObjectTurn (OBJETO *obj)
{
	if(obj != NULL)
	{
		return obj->fullTurn;
	}
	return 0;
}

/**
 *	Função irá mudar o tipo e a função movimentação da peça
 *
 *	DESCRIÇÃO:
 *		Função irá substituir o valor de tipo e a função de movimentação
 *		da peça.
 *
 *	PARAMETROS:
 *		OBJETO *obj - ponteiro para objeto
 *		char type - novo tipo para mudança
 *		char* (*mov)(MOV_PARAM) - ponteiro para nova função de movimentação
 *
 *	RETORNO:
 *		void
 */
void changeType (OBJETO *obj, char type, funcPtr mov)
{
	int aux = type;
	if(type > 96)
		aux -= 32;

	if(obj != NULL && mov != NULL &&
			(aux == 'P' || aux == 'N' || aux == 'B' || aux == 'R' || aux == 'Q' || aux == 'K'))
	{
		obj->type = type;
		obj->value = point (type);
		obj->mov = mov;
	}
}

/**
 *	Função irá mudar a posição da peça
 *
 *	DESCRIÇÃO:
 *		Função irá substituir a posição atual da peça
 *
 *		OBJ.:	a nova posição deve ser string de heap
 *		pois sofrerá free()
 *
 *	PARÂMETROS:
 *		OBJETO *obj - ponteiro para objeto
 *		char *position - nova posição da peça
 *
 *	RETORNO:
 *		VOID
 */
void changePosition (OBJETO *obj, char *position, int fullTurn)
{
	if(obj != NULL && position != NULL &&
			strlen(position) == 2 &&
			(position[0] >= 'a' && position[0] <= 'h' && position[1] >= '1' && position[1] <= '8') &&
			fullTurn > 0)
	{
		strcpy(obj->position, position);
		free(position);
		obj->fullTurn = fullTurn;
	}
}

/**
 *	Função para registrar a peça capiturada
 *
 *	DESCRIÇÃO:
 *		Função irá marcar a peça como capturada.
 *
 *	PARAMETROS:
 *		OBJETO *obj - ponteiro para objeto
 *
 *	RETORNO:
 *		VOID
 */
void captured (OBJETO *obj)
{
	if(obj != NULL)
	{
		obj->active = 0;
	}
}

/**
 * Função para impressão do objeto para depuração
 */
void printObject (OBJETO *const obj)
{
	if(obj != NULL)
	{
		printf("%c _ %s _ %d\n", obj->type, obj->position, obj->value);
	}
}

/**
 * Função para impressão da coleção de objetos para depuração
 */
void printCollectionObject (OBJETO **const collectionObj, int size)
{
	if(collectionObj != NULL)
	{
		int i;
		for(i = 0; i < size; i++)
		{
			if(collectionObj[i] != NULL)
			{
				printObject (collectionObj[i]);
			}
			else
				printf("problema com o objeto [%d] da colecao\n", i);
		}
	}
}

/**
 * Função para impressão de jogadas de uma peça
 */
void printPlay (OBJETO *obj)
{
	if(obj != NULL)
	{
		printListMov( obj->list, obj->nList);
	}
}

/**
 * Função para impressão de jogadas de uma coleção de peças
 */
void printCollectionPlay (OBJETO **collection, int size)
{
	if(collection != NULL && size > 0)
	{
		int i;
		for(i = 0; i < size; i++)
		{
			printPlay(collection[i]);
		}
	}
}
