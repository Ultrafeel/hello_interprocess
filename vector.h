/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   vector.h
 * Author: mml
 *
 * Created on серада, 12, красавіка 2017, 19.33
 */

#ifndef VECTOR_H
#define VECTOR_H

#ifdef __cplusplus
extern "C" {
#endif

// vector.h

#define VECTOR_INITIAL_CAPACITY 100

// Define a vector type
typedef struct {
  int size;      // slots used so far
  int capacity;  // total available slots
  int *data;     // array of integers we're storing
} Vector;

void vector_init(Vector *vector);

void vector_append(Vector *vector, int value);

int vector_get(Vector *vector, int index);

void vector_set(Vector *vector, int index, int value);

void vector_double_capacity_if_full(Vector *vector);

void vector_free(Vector *vector);


#ifdef __cplusplus
}
#endif

#endif /* VECTOR_H */

