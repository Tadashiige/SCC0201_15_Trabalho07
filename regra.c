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
 * Arquivo regulador de regra - compor funções reguladores de regras para conferência de jogadas
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "regra.h"
#include "FEN.h"
#include "objeto.h"

//função definidora de pontuação
int point (char type)
{
	int point = 0;
	//as letras possuem pelo ASCII sequencias iguais, mas inícios diferentes, e esta diferenção é de 32 ('a'=97 e 'A'=65)
	if(type >= 'a')
		type -= 32;

	switch (type)
	{
	case 'P':
		point = 100;
		break;
	case 'N':
		point = 325;
		break;
	case 'B':
		point = 325;
		break;
	case 'R':
		point = 550;
		break;
	case 'Q':
		point = 1000;
		break;
	case 'K':
		point = 50000;
		break;
	default:
		break;
	}
	return point;
}

/**
 * Função comparação para ordenação segundo a cor - branco primeiro
 *
 * DESCRIÇÃO:
 * 		Função irá comparar dois objetos segundo sua cor, isto é,
 * 		segundo o valor da tabela ASCII, uma vez que o tamanho da
 * 		letra representam a cor. A minúscula começam de 97 e maiúsculas
 * 		em 65.
 *
 * 	PARAMETROS:
 * 		const void* avoid - ponteiro para o primeiro termo
 * 		const void* bvoid - ponteiro para o segundo termo
 *
 * 	RETORNO:
 * 		int difference - valor da diferença (magnetude) entre os dois
 */
int separaCor (const void* avoid, const void* bvoid)
{
	OBJETO **a = (OBJETO**)avoid, **b = (OBJETO**)bvoid;
	return getType(*a) - getType(*b);
}

/**
 *	Função comparação para ordenação segundo critério de desempate.
 *
 *	DESCRIÇÃO:
 *		A função irá comparar entre dois termos segundo os critérios de desempates sequentes,
 *		sendo eles: valor, posição na coluna e posição na linha. O valor
 *		de retorno representa se o primeiro termo deve ser trocado ou não pelo segundo.
 *
 *	PARAMETROS:
 *		const void * avoid - ponteiro para primeiro termo
 *		const void * bvoid - ponteiro para segundo termo
 *
 *	RETORNO:
 *		int difference - diferença entre os dois termos, se <=0 não se troca, >0 troca-se.
 */
int desempate (const void* avoid, const void* bvoid)
{
	OBJETO **a = (OBJETO**)avoid, **b = (OBJETO**)bvoid;
	if (getValue(*a) == getValue(*b))
	{
		if((getType(*a) != 'p' && getType(*a) != 'P') && getType(*a) != getType(*b))
			return -(getType(*a) - getType(*b));
		if(getObjectColumn(*a) == getObjectColumn(*b))
		{
			//a posição de armazenamento do tabuleiro é invertido na vertical, mas a notação já corrigiu o erro
			return (int)(getObjectRow(*a) - getObjectRow(*b));
		}
		return (int)(getObjectColumn(*a) - getObjectColumn(*b));
	}
	return getValue(*a) - getValue(*b);
}

/**
 *	Função comparação para ordenação segundo critério de desempate.
 *
 *	DESCRIÇÃO:
 *		A função irá comparar entre dois termos segundo os critérios de desempates sequentes,
 *		sendo eles: posição na coluna e posição na linha. O valor
 *		de retorno representa se o primeiro termo deve ser trocado ou não pelo segundo.
 *
 *	PARAMETROS:
 *		const void * a - ponteiro para primeiro termo
 *		const void * b - ponteiro para segundo termo
 *
 *	RETORNO:
 *		int difference - diferença entre os dois termos, se <=0 não se troca, >0 troca-se.
 */
int sortPlay (const void* a, const void* b)
{
	char **A = (char**)a, **B = (char**)b;

	if(getNotationColumn(*A) == getNotationColumn(*B))
	{
		//a posição de armazenamento do tabuleiro é invertido na vertical, mas a notação já corrigiu o erro
		if(getNotationRow(*A) == getNotationRow(*B))
		{
			return point((*A)[strlen(*A) - 1]) - point((*B)[strlen(*B) - 1]) - ((*A)[strlen(*A)-1] - (*B)[strlen(*B)-1]); //resolução para promoão de peões
		}
		return  getNotationRow(*A) - getNotationRow(*B);
	}
	return getNotationColumn(*A) - getNotationColumn(*B);
}

/**
 *	Função comparação para ordenação segundo critério de desempate para os tratamentos de colisões de jogadas para mesma casa.
 *
 *	DESCRIÇÃO:
 *		A função irá comparar entre dois termos segundo os critérios de desempates sequentes,
 *		sendo eles: posição na coluna e posição na linha. O valor
 *		de retorno representa se o primeiro termo deve ser trocado ou não pelo segundo.
 *		A função é igual ao sortPlay, mas trata para ordenação de tipos CONFLICT usado
 *		na fase de tratamento de colisões sobre jogadas iguais em condições iguais de peças.
 *
 *	PARAMETROS:
 *		const void * a - ponteiro para primeiro termo
 *		const void * b - ponteiro para segundo termo
 *
 *	RETORNO:
 *		int difference - diferença entre os dois termos, se <=0 não se troca, >0 troca-se.
 */
int sortPlayConflict (const void* a, const void* b)
{
	CONFLICT *A = (CONFLICT*)a, *B = (CONFLICT*)b;
	if(getNotationColumn(*(A->play)) == getNotationColumn(*(B->play)))
	{
		//a posição de armazenamento do tabuleiro é invertido na vertical, mas a notação já corrigiu o erro
		if(getNotationRow(*(A->play)) == getNotationRow(*(B->play)))
		{
			return point((*(A->play))[strlen(*(A->play)) - 1]) - point((*(B->play))[strlen(*(B->play)) - 1])
					- ((*(A->play))[strlen(*(A->play))-1] - (*(B->play))[strlen(*(B->play))-1]); //resolução para promoão de peões
		}
		return  getNotationRow(*(A->play)) - getNotationRow(*(B->play));
	}
	return getNotationColumn(*(A->play)) - getNotationColumn(*(B->play));
}

/**
 * 	Função irá preencher o tabuleiro com as peças dadas por entrada
 *
 * 	DESCRIÇÃO:
 * 		Função irá percorrer a coleção de peças e colocá-las em suas posições dentro do tabuleiro.
 *
 * 		IMPORTANTE:	O preenchimento segue a regra de localização de FEN, assim a linha 1 é o mais baixo,
 * 		a coluna 'a' é o da mais esquerda, no entanto o ARMAZENAMENTO faz com que o tabuleiro, portanto,
 * 		seja invertida verticalmente.
 *
 * 	PARAMETROS:
 * 		OBJETO *** table - matriz de ponteiro para objetos, representando o tabuleiro
 * 		const OBJETO ** collection - coleção de peças no jogo
 * 		const int pieces_num - número de peças da coleção
 *
 * 	RETORNO:
 * 		void
 */
void fillTable (OBJETO *** table, OBJETO ** const collection, const int pieces_num)
{
	if(table != NULL && collection != NULL && pieces_num > 0)
	{
		int i;

		//resetar o tabuleiro inteiro
		int j;
		for(i = 0; i < TABLE_ROWS; i++)
			for (j = 0; j < TABLE_COLS; j++)
				table[i][j] = NULL;

		int row = 0, col = 0;
		for(i = 0; i < pieces_num; i++)
		{
			OBJETO *obj = collection[i];

			col = getObjectColumn(obj);
			row = 7 - getObjectRow(obj);

			table[row][col] = obj;
		}
	}
}

/**
 * Função imprimi tod.o o tabuleiro na tela
 *
 * DESCRIÇÃO:
 *		Função irá imprimir o tabuleiro segundo representação da matriz extraída
 *		da notação FEN.
 *
 * 	PARAMETROS:
 * 		const OBJETO ***table - matriz de ponteiro para objetos
 *
 * 	RETORNO:
 * 		void
 */
void printTable (OBJETO *** const table)
{
	int i, j;
	for (i = 0; i < TABLE_ROWS; i++)
	{
		for(j = 0; j < TABLE_COLS; j++)
		{
			if(table[i][j] == NULL)
				fprintf(stdout, " - ");
			else
			{
				fprintf(stdout, " %c ", getType(table[i][j]));
			}
		}
		fprintf(stdout, "\n");
	}
}

//conjunto de regras condicionais spara loopings
int ortSup (int i, int j)
{
	return i >= 0;
}
int ortInf (int i, int j)
{
	return i < TABLE_ROWS;
}
int ortEsq (int i, int j)
{
	return j >= 0;
}
int ortDir (int i, int j)
{
	return j < TABLE_COLS;
}

/**
 * Função verificadora de perigo ao rei nas ortogonais
 * DESCRIÇÂO:
 * 		Função irá verificar se a peça encontrada é inimiga e se ela oferece risco
 * 		ao rei pela sua distancia.
 *
 * 	PARAMETROS:
 * 		char piece - tipo da peça
 * 		char ~ - conjunto de tipos inimigos segundo o turno
 * 		int kingCoord - coordenada matricial do rei em um dos eixos ortonormais
 * 		int coord - coordenada matricial da peça inimiga em um dos eixos ortonormais
 *
 * 	RETORNO:
 * 		int resultado - resultado da verificação (1 - cheque, 0 - seguro)
 */
int _verifyDangerOrto (char piece,
		char queen,
		char rook,
		char king,
		int turn,
		int kingCoord,
		int coord)
{
	if((piece < 'a') != turn)
	{
		if(piece == queen || piece == rook)
			return 1;
		if(abs(kingCoord - coord) == 1 && piece == king)
			return 1;
	}
	return 0;
}

//conjunto de regras condicionais para loopings
int diagSupEsq (int i, int j)
{
	return (i >= 0 && j >= 0);
}
int diagSupDir (int i, int j)
{
	return (i >= 0 && j < TABLE_COLS);
}
int diagInfEsq (int i, int j)
{
	return (i < TABLE_ROWS && j >= 0);
}
int diagInfDir (int i, int j)
{
	return (i < TABLE_ROWS && j < TABLE_COLS);
}

/**
 * Função verificadora de perigo ao rei nas diagonais
 * DESCRIÇÂO:
 * 		Função irá verificar se a peça encontrada é inimiga e se ela oferece risco
 * 		ao rei pela sua distancia.
 *
 * 	PARAMETROS:
 * 		char piece - tipo da peça
 * 		char ~ - conjunto de tipos inimigos segundo o turno
 * 		int kingCoord - coordenada matricial do rei em um dos eixos diagonais
 * 		int coord - coordenada matricial da peça inimiga em um dos eixos diagonais
 *
 * 	RETORNO:
 * 		int resultado - resultado da verificação (1 - cheque, 0 - seguro)
 */
int _verifyDangerDiag(char piece,
		char queen ,
		char bishop,
		char peasant,
		char king,
		int turn,
		int dist,
		int kingRow,
		int row)
{
	if((piece < 'a') != turn)
	{
		if(piece == queen || piece == bishop)
			return 1;

		if(dist == 2)
		{
			if(piece == peasant)
			{
				//para oferecer perido o peão deve estar na linha de avanço contra o rei
				if(row < kingRow == (piece >= 'a'))
					return 1;
				return 0;
			}
			if(piece == king)
				return 1;
		}
	}
	return 0;
}

/**
 *	Função irá avaliar se há risco para o rei na jogada pretendida
 *
 *	DESCRIÇÃO:
 *		Função irá realizar uma busca pelas direções a partir da posição do rei em busca
 *		por peças que ofereçam risco ao rei segundo a movimentação pretendida de uma peça.
 *		São realizadas verificações para as ortogonais e as diagonais até que se encontre
 *		peças que ofereçam perigo ao rei. Adicionamente são verificadas as posições segundo
 *		movimento de um cavalo para verificar se há um cavalo nestas posições. Caso seja
 *		encontrada uma ameaça ao rei é retornado 1, caso contrário 0.
 *
 *	PARAMETROS:
 *		OBJETO *** const table - ponteiro para matriz do tabuleiro
 *		OBJETO * const obj - ponteiro para a peça em movimento
 *		int row - valor do índice da matriz na linha
 *		int col - valor do índice da matriz na coluna
 *		int turn - valor representativo de turno
 *
 *	RETORNO:
 *		risco - valor de risco para o rei, 0 significa o não risco e 1 significa a invalidade por risco
 *
 */
int __riscoRei (OBJETO *** const table, OBJETO * const obj, int row, int col, int turn)
{
	if(table != NULL && obj != NULL && row >= 0 && col >= 0 && row < TABLE_ROWS && col < TABLE_COLS)
	{
		int i, j;
		int resultado = 0;

		//estabelecer as peças inimigas
		char KING = (turn == 1)? 'K' : 'k';
		char king = (turn == 0)? 'K' : 'k';
		char knight = (turn == 0)? 'N' : 'n';
		char rook = (turn == 0)? 'R' : 'r';
		char bishop = (turn == 0)? 'B' : 'b';
		char queen = (turn == 0)? 'Q' : 'q';
		char peasant = (turn == 0)? 'P' : 'p';

		//mudar o tabuleiro para a posição pretendida, assim guardar as variações para poder reverter
		OBJETO *removido = table[row][col];
		table[row][col] = obj;
		int rowVeridico = 7 - getObjectRow(obj);
		int colVeridico = getObjectColumn(obj);
		if(row != rowVeridico || col != colVeridico)
			table[rowVeridico][colVeridico] = NULL;

		//procurar pelo rei
		for (i = 0; i < TABLE_ROWS; i++)
		{
			for( j = 0; j < TABLE_COLS; j++)
			{
				if(getType(table[i][j]) == KING)
					goto out; //sair do looping mais externo do percurso de matriz
			}
		}out:; //saida do goto dos loopings pela busca do rei

		//identificar as coordenadas do rei
		int kingRow = 8;
		int kingCol = 8;
		if(j < TABLE_COLS)
		{
			kingRow = i;
			kingCol = j;
		}
		else //algo deu errado, rei nao encontrado
		{
			table[rowVeridico][colVeridico] = table[row][col];
			table[row][col] = removido;
			return 1;
		}

		int acrsI = -1; //valor de acrescimo de i (linha)
		int acrsJ = -1; //valor de acrescimo de j (coluna)

		// ************************************** fazer busca pelas direções em eixos ortonormais
		int (*funcVOrt[4]) (int i, int j); //vetor de funções condicionais
		funcVOrt[0] = &ortSup;
		funcVOrt[1] = &ortInf;
		funcVOrt[2] = &ortEsq;
		funcVOrt[3] = &ortDir;

		int count, auxRow, auxCol;
		int kingCoord, coord;

		//buscar para as 4 direções ortonormais
		for(count = 0; count < 4; count ++)
		{
			acrsI *= -1;
			acrsJ *= -1;

			kingCoord = ((count < 2)*kingRow + (count >= 2)*kingCol);
			for(i = kingRow - acrsI, j = kingCol - acrsJ; (*funcVOrt[count])(i, j); i -= acrsI, j -= acrsJ)
			{
				//para count 0 e 1, auxRow == i, e para count 2 e 3, auxRow == kingRow
				auxRow = ((count < 2)*i + (count >= 2)*kingRow);
				auxCol = ((count < 2)*kingCol + (count >= 2)*j);

				if(table[auxRow][auxCol] != NULL)
				{
					//encontrado uma casa não vazia na ortogonal, verificar se ela oferece risco ao rei
					coord = ((count < 2)*i + (count >= 2)*j);
					if(_verifyDangerOrto(getType(table[auxRow][auxCol]), queen , rook, king, turn, kingCoord, coord))
						resultado = 1;
					break;
				}
			}
		}
		// ************************************** fazer busca pelas direções das diagonais
		int (*funcVDiag[4]) (int i, int j); //vetor de funções condicionais
		funcVDiag[0] = &diagSupEsq;
		funcVDiag[1] = &diagSupDir;
		funcVDiag[2] = &diagInfEsq;
		funcVDiag[3] = &diagInfDir;
		//int count;
		/*int */acrsI = 1; //valor de acrescimo de i (linha)
		/*int */acrsJ = -1; //valor de acrescimo de j (coluna)
		//procurar pelas 4 direções diagonais
		for(count = 0; count < 4; count ++)
		{
			acrsI *= pow(-1, (count == 2)); //valor deve ser i = 1, 2 vezes e i = -1, 2 vezes
			acrsJ *= -1; //valor deve ser i = 1 e i = -1 alternadamente

			for(i = kingRow - acrsI, j = kingCol - acrsJ; (*funcVDiag[count])(i, j); i -= acrsI, j -= acrsJ)
			{
				if(table[i][j] != NULL)
				{
					//encontrado uma casa não vazia na diagonal, verificar se a peça oferece risco ao rei
					if(_verifyDangerDiag(getType(table[i][j]), queen , bishop, peasant, king, turn,
							(abs(kingRow - i) + abs(kingCol - j)), kingRow, i))
						resultado = 1;
					break;
				}
			}
		}
		// ************************************** fazer busca pelas rotas do cavalo
		/*int*/auxRow = -1, auxCol = 1;
		for(count = 0; count < 4; count++)
		{
			auxRow *= pow(-1, (count == 2));
			auxCol *= -1;
			OBJETO *auxObj = NULL;
			//o movimento do cavalo é em L, assim sempre inversamente são avanços ou retrocessos de coordenada de 2 por 1.
			//verificar se na posição tal reside um cavalo inimigo
			if(TABLE_ROWS > kingRow + auxRow*2 && TABLE_COLS > kingCol + auxCol*1 &&
					kingRow + auxRow*2 >= 0 && kingCol + auxCol*1 >= 0 &&
					(auxObj = table[kingRow + auxRow*2][kingCol + auxCol*1]) != NULL &&
					(getType(auxObj) == knight))
				resultado = 1;

			if(TABLE_ROWS > kingRow + auxRow*1 && TABLE_COLS > kingCol + auxCol*2 &&
					kingRow + auxRow*1 >= 0 && kingCol + auxCol*2 >= 0 &&
					(auxObj = table[kingRow + auxRow*1][kingCol + auxCol*2]) != NULL &&
					(getType(auxObj) == knight))
				resultado = 1;
		}
		//reverter o tabuleiro para o estado anterior
		table[rowVeridico][colVeridico] = table[row][col];
		table[row][col] = removido;

		return resultado;
	}
	return 1;
}

/**
 * Função irá chamar uma função que realiza a busca pela presença de risco ao rei para
 * uma data posição pretendida da peça.
 */
int riscoRei (OBJETO *** const table, OBJETO * const obj, int row, int col, int turn)
{
	return __riscoRei (table, obj, row, col, turn);
	//int result = __riscoRei (table, obj, row, col, turn);
	//if(result) printf("rei em perigo_[%d][%d]\n", row, col);
	//return result;
}
