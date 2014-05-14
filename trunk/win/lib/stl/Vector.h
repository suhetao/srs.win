#ifndef _VECTOR_H
#define _VECTOR_H

#define DEFALUT_VECTOR_SIZE 16

typedef void* Element;

typedef struct VECTOR_T
{
	Element* data;
	int length;
	int alloced;
}Vector;

Vector* Vector_New(int length);
void Vector_Free(Vector *v);
int Vector_Enlarge(Vector *v);
int Vector_Insert(Vector *v, int index, Element data);
int Vector_Append(Vector *v, Element data);
void Vector_Clear(Vector *v);
void Vector_Remove_Range(Vector *v, int index, int length);
void Vector_Remove(Vector *v, int index);

#endif
