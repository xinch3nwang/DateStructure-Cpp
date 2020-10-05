#include <iostream>
using namespace std;

template <class T>
class SqList
{
	private:
		T* elem;
		int length;
		int listsize;

	public:
		SqList(int m);
		~SqList();
		void CreateList(int n);
		void Insert(int i, T e);
		T Delete(int i);
		T GetElem(int i);
		int Locate(T e);
		void Clean();
		int Empty();
		int Full();
		int Length();
		void ListDisp();
};

template <class T>
SqList<T>::SqList(int m)
{
	elem = new T[m];
	length = 0;
	listsize = m;
} 

template <class T>
SqList<T>::~SqList()
{
	delete []elem;
	length = 0;
	listsize = 0;
}

template <class T>
void SqList<T>::CreateList(int n)
{
	if(n>listsize) throw "error";
	cout<< "input " << n << " value(s)"<< endl;
	for(int i=1; i<n; i++)
	{
		cin >> elem[i-1];
	}
	length = n;
}

template <class T>
void SqList<T>::Insert(int i, T elem)
{
	if(length>=listsize) throw "overflow";
	if(i<1||i>length)
}

template <class T>
T SqList<T>::Delete(int i)
{
	T x;
	if(length==0) throw "underflow";
	if(i<1)
	x = elem[i-1]; 
	for(int j=i; j<length; j++)
	{
		elem[j-1] = elem[j];
	}
	length--;
	return x;
}

template <class T>
int SqList<T>::Locate(T e)
{
	for(int i=0; i<length; i++)
	{
		if(elem[i]==e) return i+1;
		return 0;
	}
}

template <class T>
void SqList<T>::Clean()
{
	length = 0;
}

template <class T>
T SqList<T>::GetElem(int i)
{
	T e;
	if(i<1||i>length) throw "illegal address";
	e = elem[i-1];
	return e;
}

template <class T>
int SqList<T>::Empty()
{
	if(length==0) return 1;
	else return 0;
}

template <class T>
int SqList<T>::Full()
{
	if(length==listsize) return 1;
	else return 0;
}

template <class T>
int SqList<T>::Length()
{
	return length;
}

template <class T>
void SqList<T>::ListDisp()
{
	for(int i=0; i<length; i++)
	{
		cout << i+1 << "\t" << elem[i] << endl;
	}
}