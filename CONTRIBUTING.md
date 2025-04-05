# CONTRIBUTING.md

## Toward a Hybrid Neural-Symbolic Cognitive Architecture

> Where brain meets mind in the same address space

Thank you for your interest in contributing to our hybrid neural-symbolic cognitive architecture. This project merges Llama2.c's neural capabilities with Claire's concept-slot memory system to create an AI with both "brain" (neural processing) and "mind" (symbolic representation) that can maintain editable, persistent knowledge.

## Memory-Centric Vision

Our architecture functions through a unique **two-tier memory model**:

1. **Symbolic Memory (Claire)**: Long-term, persistent, explicit "concept" nodes connected by labeled "slot" relationships
2. **Neural Memory (Llama2.c)**: Short-term, ephemeral states in key/value caches and attention patterns
3. **Bridge Mechanisms**: Systems that allow neural patterns to create symbolic structures and symbolic knowledge to guide neural generation

This memory-centric approach gives our system capabilities beyond standard LLMs: editable knowledge, persistence beyond context windows, and transparent inspection of internal relationships.

## Repository Structure

```
/
├── include/          # Header files defining core structures
│   └── concept.h     # Concept and Slot structure definitions
├── src/              # Implementation files
│   ├── concept.c     # Core concept-slot memory operations
│   ├── main.c        # Entry point and examples
│   └── bridge/       # Neural-symbolic integration points
├── tests/            # Test suite
└── docs/             # Documentation
```

## Contribution Workflow

1. **Find or create an issue** describing the feature/fix you want to implement
2. **Fork the repository** and create a branch with a descriptive name
3. **Implement your changes** following our memory-safe coding guidelines
4. **Add tests** that verify your implementation works as expected
5. **Submit a pull request** with a proper commit message following our protocol
6. **Respond to review feedback**

## Memory Management Guidelines

When working with our codebase, keep these crucial memory principles in mind:

1. **Ownership Rules**:
   - Functions creating concepts/slots own the allocation of memory
   - Caller owns the responsibility to free concepts when done
   - String memory should be duplicated (using `strdup()`) rather than borrowed

2. **Pointer Safety**:
   - Always check for NULL pointers before dereferencing
   - Initialize pointers to NULL when declared but not yet assigned
   - Free memory in the reverse order of allocation

3. **Memory Efficiency**:
   - Consider using string interning for repeated concept IDs
   - Use appropriate initial capacity for slots to minimize reallocations
   - Profile memory usage for operations on large concept graphs

4. **Neural-Symbolic Bridge**:
   - Document memory flow between neural and symbolic components
   - Be explicit about whether operations are synchronous or asynchronous
   - Specify which tier owns what memory at each processing stage

## Commit Message Protocol

To maintain a coherent project history that reflects our memory architecture, use the following commit message format:

```
[memory_tier:component] <action> <entity> (<50 chars)

* Purpose: <why this change matters in the neural-symbolic vision>
* Implementation: <technical details of the change>
* Memory impact: <effect on pointers/allocation/graph structure>

@concepts: #concept1 #concept2
@relations: concept1->relation->concept2
```

### Memory Tiers

- `symbolic`: Long-term concept-slot memory (Claire's domain)
- `neural`: Short-term neural activation memory (Llama's domain)  
- `bridge`: Integration between symbolic and neural memory
- `meta`: Build system, documentation, tooling

### Components

- `concept`: Core concept structure/handling
- `slot`: Relationship management between concepts
- `graph`: Overall semantic network operations
- `attention`: Neural attention mechanisms
- `kv_cache`: Key-value cache for neural memory
- `serialize`: Persistence of memory structures
- `prompt`: Neural-to-symbolic prompt engineering
- `embedding`: Vector representations of concepts

### Actions

- `allocate`: Create new memory structures
- `link`: Connect existing memory structures
- `optimize`: Improve memory efficiency
- `persist`: Save/load memory structures
- `free`: Release memory
- `trace`: Debugging/tracing memory operations
- `repair`: Fix memory-related issues

### Examples

```
[symbolic:concept] Optimize string memory in Concept struct

* Purpose: Support large concept graphs without memory bloat
* Implementation: Added string interning for concept IDs and types
* Memory impact: Reduces heap allocations by ~40% for repeated strings

@concepts: #memory_efficiency #string_interning
@relations: concept->contains->strings
```

```
[bridge:attention] Link attention patterns to concept slots

* Purpose: Enable neural-discovered relationships to persist in symbolic memory
* Implementation: Extract top-k attention weights and create corresponding slots
* Memory impact: Dynamic slot creation based on attention threshold (configurable)

@concepts: #attention_mechanism #slot_creation
@relations: attention->generates->slot, neural->informs->symbolic
```

## Code Style Guidelines

1. **Memory Safety First**: All functions should have appropriate NULL checks and error handling
2. **Documentation**: Include detailed comments explaining memory ownership and structure relationships
3. **Naming Conventions**:
   - Functions: `verb_noun` format (e.g., `create_concept`, `add_slot`)
   - Variables: Clear, descriptive names (e.g., `concept_count` not `cc`)
   - Structs: PascalCase (e.g., `Concept`, `Slot`)
4. **Function Structure**:
   - Document memory ownership transfer clearly
   - Check input validity before allocation
   - Return NULL or appropriate error codes on failure

## Testing Guidelines

1. **Memory Tests**: Verify there are no leaks using tools like Valgrind
2. **Functionality Tests**: Ensure concepts and slots behave as expected
3. **Edge Cases**: Test NULL inputs, empty strings, and boundary conditions
4. **Integration Tests**: Verify neural-symbolic bridge functions correctly
5. **Performance Tests**: Evaluate memory usage and speed for large concept graphs

## Review Process

Pull requests will be reviewed based on:

1. **Memory Safety**: No leaks, dangling pointers, or risky pointer operations
2. **Functionality**: Code works as described and integrates properly with existing systems
3. **Test Coverage**: All new code has appropriate tests
4. **Documentation**: Code is well-commented and includes updated documentation if needed
5. **Performance**: Changes don't significantly degrade memory efficiency or speed

## Contribution Ideas

If you're looking for ways to contribute, consider these areas:

1. **Memory Optimization**: Improve concept/slot memory efficiency
2. **Persistence Layer**: Enhance serialization of concept graphs
3. **Neural-Symbolic Bridge**: Develop new mechanisms for neural ↔ symbolic interaction
4. **Attention Mapping**: Create tools to visualize attention → symbolic slot mapping
5. **Benchmark Suite**: Build performance tests for memory usage and speed
6. **Documentation**: Improve explanation of memory model and architecture

## License Compliance

This project is licensed under the Cryptographic Autonomy License (CAL-1.0). Ensure your contributions comply with this license, which emphasizes user data autonomy.

## Communication

- **Issues**: For bug reports and feature requests
- **Discussions**: For architectural questions and ideas
- **Pull Requests**: For code contributions

## Example First Contribution

A good first contribution might be fixing the error in `concept.c` where the field access is using `>` instead of `->`:

```c
// Incorrect:
printf("\t\tName: %s\n", concept>slots[i].name);

// Correct:
printf("\t\tName: %s\n", concept->slots[i].name);
```

This demonstrates understanding of the C pointer structure that forms the foundation of our symbolic memory system.

---

By contributing to this project, you're helping build a new generation of AI systems that fuse the discrete explicitness of symbolic knowledge with the continuous, distributed representations of large language models—all in a single, transparent memory space.

Welcome to the intersection of brain and mind!
