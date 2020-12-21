#include <string>
#include "line.h"

template <typename T>
struct Node {
  public:
    T *data;
    Node *next;
    Node *prev;

    Node () {
      next = NULL;
      prev = NULL;
    }
};

template <typename T>
class LinkedList {
  private:
    Node<T> head;
    Node<T> tail;
    Node<T> *cursor;  /* the currently selected line */

  public:
    LinkedList ();
    int append (T*);
    void print (void);
    void seek_forward (unsigned);
    void seek_backward (unsigned);
    T* get_at_cursor ();
    T* next ();
    T* prev ();
    void reset_cursor ();
    T* go_to (unsigned);
};

template <typename T>
LinkedList<T>::LinkedList () {
  this->head.prev = NULL;
  this->head.next = &this->tail;

  this->tail.prev = &this->head;
  this->tail.next = NULL;

  this->cursor = &this->head;
}

template <typename T>
int LinkedList<T>::append (T *data) {
  Node<T> *current;
  current = this->tail.prev;

  Node<T> *new_node = new Node<T>;
  if (!new_node) {
    return 0;
  }

  new_node->data = data;

  current->next = new_node;
  new_node->prev = current;

  new_node->next = &this->tail;
  this->tail.prev = new_node;
}

template <typename T>
void LinkedList<T>::print () {
  Node<T> *current = this->head.next;
  while (current != &this->tail) {
    std::cout << current->data->get_str() << std::endl;
    current = current->next;
  }
}

template <typename T>
void LinkedList<T>::seek_forward (unsigned steps) {
  while (steps-- && this->cursor.next != &this->tail)
    this->cursor = &this->cursor.next;
}

template <typename T>
void LinkedList<T>::seek_backward (unsigned steps) {
  while (steps-- && this->cursor.next != &this->head)
    this->cursor = &this->cursor.prev;
}

template <typename T>
T* LinkedList<T>::next () {
  if (this->cursor->next == &this->tail)
    return NULL;

  this->cursor = this->cursor->next;
  return this->cursor->data;
}

template <typename T>
T* LinkedList<T>::prev () {
  if (this->cursor->prev == &this->head)
    return NULL;

  this->cursor = this->cursor->prev;
  return this->cursor->data;
}


template <typename T>
T* LinkedList<T>::get_at_cursor () {
  return this->cursosr->data;
}

template <typename T>
void LinkedList<T>::reset_cursor () {
  this->cursor = &this->head;
}

template <typename T>
T* LinkedList<T>::go_to (unsigned num) {
  this->reset_cursor ();

  while (this->cursor->next != &this->tail && num) {
    num--;
    this->cursor = this->cursor->next;
  }

  return this->cursor->data;
}
