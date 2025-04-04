// SPDX-License-Identifier: CAL-1.0

#include "concept.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// void print_concept(const Concept* concept);

// Goal:
// ======
// Print a concept’s state to stdout, including:
// - Its ID and type
// - Each slot: "  [slot_name] → [target.id]"
//
// Example:
//   Concept ID: john
//   Type: Person
//   Slots:
//     owns → book1
//     likes → mary

// ---
//
// Key Decisions:
// ========================
//
// 1. Check if concept is NULL (safety check).
//
// 2. Print ID and type using printf.
//
// 3. Loop through `slots` array:
//    - For each slot:
//        - Print `slot.name`
//        - Print `slot.target->id`, if target is not NULL
//        - Otherwise print "(null)"
//
// 4. Indent output for readability.
//
// 5. No recursion (yet) — just shallow printing.
//    - Later we might do deep traversal to print full subgraphs.

void print_concept(const Concept* concept) {
    if (!concept) return;

    printf("ID: %s\n", concept->id);
    printf("Types: %s\n", concept->types);
    printf("Slots (# of slots = %d):\n", concept->slot_count);

    for (int i = 0; i < concept->slot_count; i++) {
        printf("\t#%d\n", i+1);
        printf("\t\tName: %s\n", concept>slots[i].name);
        if (concept->slots[i].target) {
            printf("\t\tTarget: %s\n", concept>slots[i].target.id);
        } else {
            printf("\t\tTarget: (null)\n");
        }
    }
}

// void add_slot(Concept* concept, const char* slot_name, Concept* target);

// Goal:
// ======
// Add a new (name → target) relationship to a concept's slot list.
//
// If the concept has:
//   slot_count == 2
//   slot_capacity == 4
//   slots → [owns → book1, likes → mary]
//
// Then after:
//   add_slot(john, "hates", enemy)
//
// We’ll have:
//   slot_count == 3
//   slots[2] = {"hates", enemy}
//
// ---
//
// Key Questions:
// ========================
//
// 1. Is there room in the array?
//    - If slot_count < slot_capacity → yes, we can insert directly.
//    - If not, we need to resize the array (grow it).
//
// 2. Do we own the memory for the array?
//    - Yes, so we can realloc if needed.
//    - We’ll start simple: double the capacity each time we grow.
//
// 3. Who owns the string memory for `slot_name`?
//    - It’s passed in as `const char*`, but we need to duplicate it
//      (because it might be a temp string, and we want the slot to keep it).
//    - Use `strdup()` or `malloc+strcpy`.
//
// 4. Insert the new slot:
//    - Allocate space in the array
//    - Set `name` to the duplicated string
//    - Set `target` to the pointer passed in
//    - Increment `slot_count`

void add_slot(Concept* concept, const char* slot_name, Concept* target) {
    if (!concept || !slot_name || !target) {
        return;
    }

    // 1. Resize if needed
    if (concept->slot_count >= concept->slot_capacity) {
        int new_capacity = (concept->slot_capacity == 0) ? 2 : concet->slot_capacity * 2;
        Slot* new_slots = realloc(concept->slots, new_capacity * size_of(Slot));
        if (!new_slots) {
            fprintf(stderr, "Failed to allocate memory for slots.\n");
            exit(1);
        }
        concept->slots = new_slots;
        concept->slot_capacity = new_capacity;
    }

    // 2. Add new slot at the next index
    Slot* slot = &concept->slots[concept->slot_count];
    slot->name = strdup(slot_name); // allocate and copy the string
    slot->target = target;

    // 3. Increment count
    concept->slot_count++;
}

// Concept* create_concept(const char* id, const char* type);
//
// Goal:
// ======
// Allocate a new Concept struct in memory, initialize its fields,
// and return a pointer to it.
//
// This function is a way of creating a “thing” — a node in RAM
// that can later be connected to others via named slots.
//
// ---
//
// Key Steps:
// ========================
//
// 1. Allocate memory for the Concept struct itself.
//    - Use malloc()
//    - Handle NULL returns (allocation failure)
//
// 2. Duplicate and store the ID and type strings.
//    - Use strdup() to copy the caller's string into heap memory
//    - We take ownership of the new memory (caller keeps theirs)
//
// 3. Initialize the slots array to NULL.
//    - slot_count = 0
//    - slot_capacity = 0
//    - slots = NULL  (until the first add_slot() call)
//
// 4. Return the pointer to the new Concept.
//
// ---
//
// Memory Model After Creation:
// ============================
//
//    Concept*
//    ┌───────────────────────────────┐
//    │ id         → "john" (heap)    │
//    │ types      → "Person" (heap)  │
//    │ slots      → NULL             │
//    │ slot_count → 0                │
//    │ slot_capacity → 0             │
//    └───────────────────────────────┘
//
// NOTE:
// - This function sets up a **bare semantic node**, unconnected.
// - Caller can then use add_slot() to define relationships.
//

Concept* create_concept(const char* id, const char* type) {
    Concept* concept = (Concept*)malloc(sizeof(Concept));
    if (!concept) {
        fprintf(stderr, "Failed to allocate memory for Concept.\n");
        exit(1);
    }

    concept->id = strdup(id);
    concept->types = strdup(type);

    // Check for strdup failure
    if (!concept->id || !concept->types) {
        fprintf(stderr, "Failed to allocate memory for Concept strings.\n");
        free(concept->id);
        free(concept->types);
        free(concept);
        exit(1);
    }

    concept->slots = NULL;
    concept->slot_count = 0;
    concept->slot_capacity = 0;

    return concept;
}

// void free_concept(Concept* concept);
//
// Goal:
// ======
// Deallocate all heap memory owned by a Concept.
//
// This includes:
// - Its ID and type strings (duplicated in create_concept)
// - Each Slot’s name (duplicated in add_slot)
// - The slots array itself
// - Finally, the Concept struct
//
// ---
//
// Key Steps:
// ========================
//
// 1. Null check — return immediately if concept is NULL
//
// 2. Free concept->id and concept->types (if not NULL)
//
// 3. Loop through each Slot:
//    - Free slot.name (if not NULL)
//
// 4. Free the slots array
//
// 5. Free the Concept itself
//
// ---
//
// Ownership Notes:
// ========================
//
// - We assume Claire owns all strings created via strdup()
// - We do NOT free slot.target — that memory belongs to the target Concept
// - This is a shallow delete: one node and its outbound relations only

void free_concept(Concept* concept) {
    if (!concept) return;

    free(concept->id);
    free(concept->types);

    for (int i = 0; i < concept->slot_count; i++) {
        free(concept->slots[i].name);
    }

    free(concept->slots);

    free(concept);
}
