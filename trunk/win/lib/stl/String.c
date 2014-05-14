#include "String.h"
#include "Util.h"
#include <assert.h>


String* String_New(char *string, int length)
{
	String* str = NULL;
	str = xmalloc(sizeof(String));
	if (NULL == str){
		return NULL;
	}
	str->length = length;
	str->data = xmalloc(sizeof(length + 1));
	if (!str->data) {
		free(str);
		return NULL;
	}
	memcpy(str->data, string, length);
	return str;
}

int String_Find(String *b, String *a)
{
	char *tmp0, *tmp1;
	int index = 0;
	int len = 0;
	int left = 0;

	if (!b || !a || b->length < a->length){
		return -1;
	}

	left = b->length - a->length;

	do{
		tmp0 = a->data;
		len = a->length;
		tmp1 = b->data + index;

		for(;*tmp0 == *tmp1 && 0 < len; len--,tmp0++,tmp1++);
		if (!len){
			goto FIND;
		}
		index++;
		left--;

	}while(left >= 0);
	return -1;
FIND:
	return index;
}

int StringCmp(String *a, String *b)
{
	int len;
	char *tmp0, *tmp1;

	assert(NULL != a);
	assert(NULL != b);
	
	if (a == b || a->data == b->data){
		return 0;
	}
	len = (a->length < b->length)?a->length:b->length;
	tmp0 = a->data;
	tmp1 = b->data;
	do{
		if(*tmp0 > *tmp1) return 1;
		if(*tmp0 < *tmp1) return -1;
		tmp0++;
		tmp1++;
		len--;
	}while(len >= 0);

	if ((a->length - b->length) > 0){
		return 1;
	}
	else if(a->length == b->length){
		return 0;
	}
	else{
		return -1;
	}
}

void String_Destroy(String* s)
{
	if (s != NULL){
		if (s->data != NULL){
			freep(s->data);
		}
		freep(s);
	}
}

BOOL String_Empty(String* s)
{
	if (s->length > 0 && NULL != s->data ){
		return FALSE;
	}
	return TRUE;
}

BOOL String_Equal(String *a, String *b)
{
	return (StringCmp(a,b) == 0);
}

int StringNCmp(String *a, char*b ,int len)
{
	char *tmp0, *tmp1;

	assert(NULL != a);
	assert(NULL != b);

	if (a->data == b){
		return 0;
	}
	len = (a->length < len)?a->length:len;
	tmp0 = a->data;
	tmp1 = b;
	do{
		if(*tmp0 > *tmp1) return 1;
		if(*tmp0 < *tmp1) return -1;
		tmp0++;
		tmp1++;
		len--;
	}while(len >= 0);

	if ((a->length - len) > 0){
		return 1;
	}
	else if(a->length == len){
		return 0;
	}
	else{
		return -1;
	}
}