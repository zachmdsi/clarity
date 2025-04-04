// SPDX-License-Identifier: CAL-1.0

#ifndef CONCEPT_H
#define CONCEPT_H

// -------------------------------------- NOTES ---------------------------------------

// A "Concept" is a piece of meaning represented in memory
// It has an ID ("john", "give", "book1")
// It has a type ("Person", "Action", "Object")
// It has "Slots" - named relationships to other Concepts

// For example:
//     Concept ID: "john"
//     Type: "Person"
//     Slots: owns -> pointer to book1
//            likes -> pointer to jane 

// What do we need?

// 1. A string for the concept's unique ID.
//    -> char* 
//
// 2. A string or enum for the concept's types
//    -> if types are limited, maybe an enum. Otherwise, char*
//
// 3. A list of slots:
//    -> Each slot has:
//          - A slot name (char*) like "owns", "likes", "agent"
//          - A pointer to another Concept
//
//    -> So we'll need a Slot struct:
//          struct Slot {
//              char* name;
//              Concept* target;
//          }
//
//    -> The Concept then has:
//          Slot* slots;            // pointer to the array of slots
//          int slot_count;         // how many slots are in the array
//          int slot_capacity;      // if we're growing the array later

// ---
// Memory Model:
// ==============
//
//    Concept
//    ┌───────────────────────┐
//    │ id         → "john"   │
//    │ type       → "Person" │
//    │ slots      → ──────┐  │
//    │ slot_count →    2  │  │
//    │ slot_capacity → 4  │  │
//    └───────────────────────┘
//                       ↓
//              ┌──────────────────────────────┐
//              │ Slot 0: "owns" → → book1     │
//              │ Slot 1: "likes" → → jane     │
//              └──────────────────────────────┘

// ---
// API Thoughts (to write later):
// - Concept* find_concept_by_id(const char* id);   ← only if we manage a table

// ----------------------------------------------------------------------------------------

typedef struct Concept Concept;

typedef struct Slot {
    char* name;
    Concept* target;
} Slot

struct Concept {
    char* id;
    char* types;
    Slot* slots;
    int slot_count;
    int slot_capacity;
}

void print_concept(const Concept* concept);
void add_slot(Concept* concept, const char* slot_name, Concept* target);
void create_concept(const char* id, const char* type);

#endif
