// SPDX-License-Identifier: CAL-1.0

int main() {
    Concept* john = create_concept("john", "Person");
    Concept* book = create_concept("book", "Object");

    add_slot(john, "owns", book);
    print_concept(john);

    return 0;
}
