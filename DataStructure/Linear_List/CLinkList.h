include<iostream>
using namespace std;

template <class T>
struct Node
{
    T data;
    Node* next;
};

template <class T>
class CLinkList
{
    private:
        Node<T>* Head;
    public:
        CLinkList();
        ~CLinkList();
        void CreateList(int n);
		void Insert(int i, T e);
		T Delete(int i);
		T GetElem(int i);
		int Locate(T e);
        T prior(T e);
		int Empty();
		int Length();
		void ListDisp();
};

template <class T>
CLinkList<T>::CLinkList(int m)
{
    Head = new Node<T>;
    Head->next = Head;
} 

template <class T>
CLinkList<T>::~CLinkList()
{
    Node<T> *P, *q;
    while(Head)
    {
        q = Head;
        while(q->next!=Head) q = q->next;
        while(Head!=Head->next)
        {
            p = Head;
            Head = Head->next;
            q->next = Head;
            delete p;
        }
        Head = NULL;
    }
}

template <class T>
void CLinkList<T>::CreateList(int n)
{
    Node<T> *p, *s;
    p = Head;
    cout << "input " << n << " value(s)" << endl;
    for(int i=1; i<=n; i++)
    {
        s = new Node<T>;
        cin >> s->data;
        s->next = p->next;
        p->next = s;
        p = s;
    }
}

template <class T>
void CLinkList<T>::Insert(int i, T e)
{
    int j = 0;
    Node<T> *p;
    p = Head;
    while(p && j<i-1)
    {
        p = p->next;
        j++;
    }
    if(!p || j>i-1) throw "abnormal address";
    else
    {
        Node<T> *s;
        s = new Node<T>;
        s->data = e;
        s->next = p->next;
        p->next = s;
    }
}

template <class T>
T CLinkList<T>::Delete(int i)
{
    T x;
    Node<T> *p, *q;
    p = Head;
    int j = 0;
    while(p->next && j<i-1)
    {
        p = p->next;
        j++;
    }
    if(!p->next || j>i-1) throw "abnormal address";
    else
    {
        q = p->next;
        p->next = q->next;
        x = q->data;
        delete p;
        return x;
    }
}

template <class T>
int CLinkList<T>::Locate(T e)
{
    int j = 1;
    Node<T> *p;
    p = Head->next;
    while(p && p->data!=e)
    {
        p = p->next;
        j++;
    }
    if(p==NULL) return 0;
    else return j;
}

template <class T>
T CLinkList<T>::GetElem(int i)
{
    Node<T> *p;
    p = Head->next;
    int j = 1;
    while(p && j<i)
    {
        p = p->next;
        j++;
    }
    if(!p || j>i) throw "abnormal address";
    else return p->data;
}


template<class T>
T CLinkList<T>::prior(T e)
{
    Node<T> *p, *q;
    p = Head;
    q = Head->next;
    while(p!=Head && q->data!=e)
    {
        p = q;
        q = q->next;
    }
    if(p==Head) throw "the first one, no prior";
    else if(q==Head) throw "element no exist";
    else return p->data;
}


template <class T>
int CLinkList<T>::Empty()
{
    if(Head->next == Head) return 1;
    else return 0;
}

template <class T>
int CLinkList<T>::Length()
{
    int len = 0;
    Node<T>* p;
    p = Head;
    while(p->next!=Head) 
    {
        len++;
        p = p->next;
    }
    return len;
}

template <class T>
void CLinkList<T>::ListDisp()
{
    Node<T>* p;
    p = Head->next;
    int i = 1;
    While(p!=Head)
    {
        cout << i << "\t" << p->data << endl;
        p = p->next;
        i++;
    }
}