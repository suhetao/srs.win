#include "Vector.h"
#include "Util.h"

Vector* Vector_New(int length)
{
	Vector* newVector;
	if (length <= 0){
		length = DEFALUT_VECTOR_SIZE;
	}
	newVector = (Vector*)xMalloc(length * sizeof(Vector));
	if (NULL == newVector){
		return NULL;
	}
	newVector->alloced = length;
	newVector->length = 0;

	newVector->data = xMalloc(length * sizeof(Element));

	if (newVector->data == NULL) {
		xFree(newVector);
		return NULL;
	}

	return newVector;
}

void Vector_Free(Vector *v)
{
	if (v != NULL) {
		xFree(v->data);
		xFree(v);
	}
}

int Vector_Enlarge(Vector *v)
{
	Element* data;
	int newsize;

	newsize = v->alloced * 2;

	data = (Element*)xRealloc(v->data, sizeof(Element) * newsize);

	if (data == NULL) {
		return 0;
	} else {
		v->data = data;
		v->alloced = newsize;
		return 1;
	}
}

int Vector_Insert(Vector *v, int index, Element data)
{
	if (index < 0 || index > v->length) {
		return 0;
	}
	
	if (v->length + 1 > v->alloced) {
		if (!Vector_Enlarge(v)) {
			return 0;
		}
	}

	xMemMove(&v->data[index + 1], 
	        &v->data[index],
	        (v->length - index) * sizeof(Element));

	v->data[index] = data;
	++v->length;

	return 1;
}

int Vector_Append(Vector *v, Element data)
{
	return Vector_Insert(v, v->length, data);
}

void Vector_Clear(Vector *v)
{
	v->length = 0;
}

void Vector_Remove_Range(Vector *v, int index, int length)
{
	if (index < 0 || length < 0) {
		return;
	}
	if(index + length > v->length){
		v->length = 0;
		return;
	}

	xMemMove(&v->data[index],
		&v->data[index + length],
		(v->length - (index + length)) * sizeof(Element));

	v->length -= length;
}

void Vector_Remove(Vector *v, int index)
{
	Vector_Remove_Range(v, index, 1);
}