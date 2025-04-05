# Toward a Hybrid Neural-Symbolic Cognitive Architecture  
**Merging Llama2.c with Claire's Concept-Slot Memory for Persistent, Editable Knowledge**

Date: *April 2025*  
Author(s): *Zach McManus / Dynamic Systems, Inc.*

---

## 1. Introduction

### 1.1 Motivation  
Language models (LMs) based on the Transformer architecture have taken the world by storm, showcasing high proficiency in a vast array of tasks, from text generation to code completion and beyond. Yet, these powerful models—trained on massive text corpora—face fundamental challenges when it comes to **long-term, editable memory**. Once a model is trained, its "knowledge" is effectively baked into its parameters, and it cannot be straightforwardly or surgically updated.

Simultaneously, classical **symbolic** approaches excel at structuring knowledge in explicit data structures that can be inspected, edited, or expanded at will—think of knowledge graphs, frames, or concept-slot models. The trade-off? Symbolic systems alone typically lack the fluid, generalized language capabilities of large neural networks.

This white paper outlines a **hybrid neural-symbolic** architecture that integrates:

- **Claire's Concept-Slot Memory:** A minimal but powerful pointer-based semantic network in C.  
- **Llama2.c:** A local, minimal C implementation of a Transformer-based language model (LLaMA-2).

By directly connecting these two systems at the **memory level**, we can exploit the best of both paradigms: the robust linguistic and pattern-matching abilities of a local LLM, and the structured, dynamically editable knowledge graph that Claire's engine provides.

### 1.2 Vision  
Our goal is an **AI with a "brain" and a "mind"** in the same address space:

1. A **Neural Substrate** (llama2.c) handles text understanding and generation. It organizes internal states—key/value caches, attention distributions, hidden activations—within contiguous blocks of C arrays.  
2. A **Symbolic Memory** (Claire) provides explicit "concept" nodes and labeled "slot" relationships, effectively forming a knowledge graph in RAM. Pointers link concepts together, and these pointers can grow, be pruned, or reorganized as new information arises.

We envision a system where text tokens and concepts can **cross-reference each other** in real time. Attention patterns may trigger the creation of new slot links; established symbolic relations can be fed back into the LLM to guide or constrain its next output. The end result is an AI that, for the first time, can **retain knowledge outside of a limited token window**, correct or update that knowledge as necessary, and reflect the changes in subsequent neural inferences.

---

## 2. Background and Prior Work

### 2.1 Transformers and Local LLMs  
The **Transformer** architecture, first introduced by Vaswani et al. ("Attention is All You Need"), revolutionized NLP by enabling parallelizable attention-based processing of sequence data. Systems such as GPT-3.5, GPT-4, and LLaMA-2 demonstrate remarkable zero-shot and few-shot performance on tasks requiring reasoning, summarization, translation, and more.

**Llama2.c**, recently popularized by open-source advocates, is a minimal reimplementation of the LLaMA-2 model in pure C. It forgoes large frameworks (like PyTorch or TensorFlow) and instead loads pre-trained weights from a single binary file into contiguous arrays. This design:

- Makes it easy to **inspect** (and potentially modify) memory layouts.  
- Avoids overhead from library abstractions.  
- Allows fine-grained pointer arithmetic for weight slicing and activation caching.

### 2.2 Symbolic Knowledge Representation  
In symbolic AI, **frames** or **slot-based** representations are used to capture semantic relationships. Claire's approach:

- Treats each **Concept** as a node in RAM, with a string ID (e.g., `"john"`) and a type (e.g., `"Person"`).  
- Maintains a flexible array of **Slots** (name → pointer-to-Concept).  
- Allows incremental construction of a graph of meaning: you can add `owns -> book1`, `likes -> jane`, or any other named relationship on the fly.

This system is extremely simple yet powerful: by storing everything as pointers to dynamically allocated `Concept` structs, it bypasses complex database overhead and can be manipulated in real time.

### 2.3 Hybrid Neural-Symbolic Approaches  
Academic interest in **neuro-symbolic** integration goes back decades, from rule-based and connectionist expert systems to more recent "knowledge-aware Transformers." However, these systems are often large, complex, or not open to direct memory manipulation. That is what makes the union of llama2.c and Claire so compelling: **both** are small, hackable, and fully transparent at the pointer level, facilitating deep integration.

---

## 3. System Architecture

### 3.1 Two-Tier Memory Model  
We propose a **two-tier** memory approach:

1. **Short-Term Neural Memory:** Stored in llama2.c's key-value (KV) caches and hidden states. This ephemeral memory is critical for immediate token-by-token generation and self-attention patterns.  
2. **Long-Term Symbolic Memory:** Managed by Claire. Concepts and slots persist across conversation turns, sessions, or user-provided updates. They do not vanish when the context window or token limit is exceeded.

The synergy arises from bridging these tiers so that the neural side can **consult** or **alter** the symbolic memory, and the symbolic side can be informed by or reflect the model's dynamic attention relationships.

### 3.1.1 Memory Addressing Model

The bridging mechanism requires a bijective mapping between neural activations and symbolic pointers:

1. **Concept Addressing Scheme**
   - Each Concept struct (~64-128 bytes) maintains a unique 32-bit identifier in the global concept table
   - Memory layout: |id_ptr(8B)|type_ptr(8B)|slots_ptr(8B)|count(4B)|capacity(4B)|hash(4B)|
   - Hash table with O(1) lookup maps string IDs to concept pointers with collision resolution via linear probing

2. **Neural-Symbolic Reference Bridge**
   - Token embeddings map to concept addresses via locality-sensitive hashing (LSH)
   - Formal mapping function: C(t) = argmin_i{d(E(t), E(concept_i))} where:
     - E(x) is embedding vector
     - d(a,b) is cosine distance
     - The resulting i is the concept_id with minimum distance

### 3.2 Bridging Mechanisms  
1. **Prompt Injection:** When the LLM needs knowledge about a concept (e.g., "john's job"), we retrieve the relevant slot from Claire's graph and insert it into the LLM prompt. This is the simplest approach: the symbolic store becomes a repository for "facts" that we feed in textually when relevant.

### 3.2.1 Attention Projection Algorithm

The attention-to-slot mapping requires explicit thresholding and pruning:

1. **Slot Creation Criteria**
   - For each token t_i and each attention head h:
     - Extract attention distribution A_h(t_i, t_j) for all j < i
     - Apply Sparse Projection: S_h(t_i) = {t_j | A_h(t_i, t_j) > max(A_h) * τ}
       where τ is tunable threshold (default: 0.75)
   - Create slot only if |S_h(t_i)| ≤ k (default: k=3) to prevent explosion
   
2. **Attention Memory Impact Analysis**
   - Space complexity: O(H * k * N) where:
     - H = number of attention heads tracked
     - k = max slots per attention-concept pair
     - N = total concepts in memory
   - For typical parameters (H=4, k=3, N=10⁴), expected overhead: ~480KB

### 3.2.2 Vector/Embedding Alignment

With deeper modifications, we can store each concept's **embedding** in Claire, letting the LLM do direct vector lookups. If the concept ID "john" has an embedding vector, the LLM can do a similarity search to find and retrieve that concept from symbolic memory, merging the best of text-based prompts and vector searches.

### 3.2.3 Concept Embedding Storage

Vector embeddings require efficient storage and similarity calculation:

1. **Memory-Efficient Embedding Model**
   - Each concept maintains a dimension-reduced embedding vector (128 dimensions, 2 bytes per dimension)
   - Memory impact: 256 bytes per concept, allocated on demand using mmap() for vectors
   - Quantization: 16-bit fixed-point representation with 12.4 format (12 bits integer, 4 bits fraction)
   
2. **Fast Similarity Search**
   - Hierarchical Navigable Small World (HNSW) graph structure for O(log N) similarity lookups
   - Implementation: 
     ```c
     // Pseudocode for fast vector lookup
     typedef struct {
         float* vectors;     // Flattened 2D array of concept vectors
         int n_concepts;     // Number of concepts with embeddings
         int n_dimensions;   // Embedding dimensions (fixed at 128)
         hnsw_index_t* index; // HNSW graph for fast retrieval
     } ConceptVectorStore;
     ```

### 3.3 Data Flow Summary  
1. **User Input:** A user prompt enters the system (e.g., "What does John own?").  
2. **Symbolic Check:** We search Claire's graph for concept "john" and a slot labeled "owns." If found, we retrieve the target concept(s) (e.g., "book1").  
3. **Contextualization:** This fact is injected into the LLM input prompt (e.g., "John owns a book named 'book1'.") before we run inference.  
4. **LLM Generation:** llama2.c produces a response (e.g., "John owns a book, possibly the same one Mary mentioned earlier.").  
5. **Symbolic Update:** If new entities or relationships surface in the output, we add or update the corresponding nodes/slots in Claire.  

This cycle repeats, gradually building a **persistent knowledge base** while allowing the LLM to reason with more context than fits in its internal window alone.

---

## 4. Implementation and Memory Model

### 4.1 Llama2.c Recap  
- **Weights Layout:** Stored in a flat array, segmented by pointer arithmetic for each layer (Query, Key, Value, etc.).  
- **Runtime State:**  
  - KV Cache: 3D array `[layer, position, dimension]` flattened into a single contiguous block.  
  - Activation Buffers: Temporary arrays for each token's forward pass.  
  - Logits: Output dimension `(vocab_size)` for next-token probabilities.  

### 4.2 Claire's Concept-Slot Model  
- **Concept Struct:**  

  ```c
  typedef struct Concept {
      char* id;
      char* types;
      Slot* slots;
      int slot_count;
      int slot_capacity;
  } Concept;
  ```

- **Slots:** Each slot is `(char* name, Concept* target)`. The concept has a dynamic array of these slots that can be expanded via `realloc()`.  
- **Graph:** In memory, we have a pointer table or a lookup function for known concepts by ID. Each concept's adjacency is effectively a small array of `(slot_name, pointer_to_other_concept)` pairs.

### 4.3 Bridging at a Low Level
**Attention Mapping to Claire Slots**

- During each `llama2.c` forward pass for token `t`, we retrieve attention weights `att[l, h]` for layer `l` and head `h`.
- Identify the top-n previous tokens receiving the largest attention scores from token `t`.
- If tokens `T_i` and `T_j` correspond to known or newly created concepts in Claire, we create a slot:

  \[
  T_i \xrightarrow{\text{head } h} T_j
  \]

  (Or using a naming scheme like `att_h_2 -> [target concept]`).

**Symbolic Summaries to Prompt**

- If a conversation includes "john," we check if `john` is in Claire and gather the relationships: `owns -> book1`, `likes -> mary`, etc.
- We insert a short summary text into the LLM prompt: "John is known to own 'book1' and likes 'mary'."
- The LLM can now incorporate that knowledge in generating the next tokens.

### 4.4 Persistent Storage and Updates  
- **Serialization:** We can write each concept to disk (ID, type, slots). On restart, the system reloads them, effectively giving the system a persistent memory that outlives the LLM's ephemeral state.
- **Real-Time Updates:** If user input reveals a contradiction ("Wait, John doesn't own that book anymore!"), we can remove or modify a slot in the Claire graph. On the next inference, the LLM has up-to-date knowledge. This is impossible by adjusting the LLM parameters alone.

### 4.4.1 Binary Concept Serialization Format

1. **File Format Specification**
   ```
   HEADER (16 bytes):
     - Magic number (4 bytes): "CLAI"
     - Version (2 bytes): 0x0001
     - Concept count (4 bytes): uint32_t
     - Flags (2 bytes): bit flags for features
     - Reserved (4 bytes): zeros
     
   CONCEPT TABLE:
     - Each concept: variable length
       - ID length (2 bytes): uint16_t
       - ID string (variable): UTF-8 bytes
       - Type length (2 bytes): uint16_t
       - Type string (variable): UTF-8 bytes
       - Slot count (2 bytes): uint16_t
       - For each slot:
         - Name length (2 bytes): uint16_t
         - Name string (variable): UTF-8 bytes
         - Target concept ID (4 bytes): uint32_t index
   ```

2. **Cycle Handling Algorithm**
   - During serialization, construct a DAG representation
   - Detect cycles using depth-first traversal with visited-node marking
   - For cyclic references, store a special "CYCLE" flag with target index

---

## 5. Discussion of Opportunities and Challenges

### 5.1 Advantages

1. **Infinite Memory Beyond Context Window:** Instead of being limited to a few thousand tokens, the system can track arbitrary amounts of knowledge in Claire, referencing it instantly.  

2. **Editable Knowledge:** Users or other processes can correct or refine the knowledge graph, something not trivially done with purely parametric LLMs.  

3. **Interpretability and Debugging:** We can partially demystify the LLM's ephemeral attention patterns by capturing them as edges in Claire. This yields a *graph of references* that can be inspected after generation.  

4. **Local and Lightweight:** Both llama2.c and Claire are minimal, single-process C programs that can run on standard operating systems without massive frameworks.

### 5.1.1 Token-to-Concept Resolution Algorithm

The mapping between tokens and concepts requires precise disambiguation:

1. **Entity Recognition Pipeline**
   ```
   function mapTokensToConcepts(tokens[]):
     spans = []
     # Phase 1: Identify potential entity spans
     for i = 0 to tokens.length:
       for j = i to min(i+MAX_SPAN_LENGTH, tokens.length):
         candidate = join(tokens[i:j])
         if existsInConceptStore(candidate):
           spans.append((i, j, confidence(candidate)))
     
     # Phase 2: Resolve overlapping spans
     return greedyNonOverlappingSelection(spans)
   ```

2. **Confidence Scoring Function**
   - c(span) = P(concept) * ContextSimilarity(span, context)
   - Where P(concept) is prior probability from usage frequency
   - ContextSimilarity compares surrounding token embeddings to concept's expected context

### 5.2 Pitfalls and Open Questions

1. **Representation Explosion:** If you store *all* attention links, you'll quickly accumulate a sprawling, unmanageable graph. Pruning or summarizing those links is crucial.  

2. **Semantic Drift:** The LLM might interpret or generate ambiguous references ("John" vs. "John Smith" vs. "Jon"), complicating the merges in the concept graph.  

3. **Performance Overheads:** Repeated bridging calls—especially mid-inference—could degrade throughput unless carefully optimized.  

4. **Consistency Guarantee:** The symbolic memory might store contradictory facts (e.g., "John is in Paris" vs. "John is in Tokyo") if the system does not handle logic constraints. The LLM may not always reconcile these contradictions in its output.

5. **Cross-Platform Nuances:** While both are pure C, differences in memory-mapped IO and pointer arithmetic across Linux, macOS, and Windows must be handled carefully to ensure stable deployment.

### 5.2.4 Logical Consistency Maintenance

To handle contradictory information in the concept graph:

1. **Contradiction Detection**
   - Define slot types including functional slots (at most one value)
   - Implement slot constraints as C structure:
     ```c
     typedef struct {
         SlotConstraintType type;  // FUNCTIONAL, TRANSITIVE, etc.
         bool (*validator)(Concept* src, char* slot_name, Concept* target);
     } SlotConstraint;
     ```

2. **Belief Maintenance System**
   - Track assertion provenance (source of slot addition)
   - Implement Truth Maintenance System (TMS) with justified beliefs
   - On contradiction:
     1. Compute dependency graph for conflicting assertions
     2. Identify minimal set of assumptions to retract
     3. Maintain justification chains for all derived slots

---

## 6. Use Cases and Evaluation

### 6.1 Digital Twins and Personal Knowledge Graphs  
A user can maintain a personal concept graph with entries about themselves, their family, preferences, or projects. The LLM, augmented by Claire, can then provide contextually rich, **long-term personalized** assistance. You can correct the system at any time by editing the graph: "No, my friend's name is spelled 'Jon' not 'John'," and the system will not repeat that mistake in the future.

### 6.1.1 Memory Management and Reference Counting

The concept memory lifecycle must prevent leaks while enabling safe referencing:

1. **Reference Counting Implementation**
   ```c
   typedef struct Concept {
       // Existing fields...
       atomic_int ref_count;  // Thread-safe reference counter
   } Concept;
   
   // Acquire/release functions
   Concept* concept_acquire(Concept* c) {
       if (c) atomic_fetch_add(&c->ref_count, 1);
       return c;
   }
   
   void concept_release(Concept* c) {
       if (c && atomic_fetch_sub(&c->ref_count, 1) == 1) {
           // Last reference, free the concept
           free_concept_internal(c);
       }
   }
   ```

2. **Garbage Collection Policy**
   - Concepts with only weak references become eligible for collection
   - Mark-and-sweep collector runs when memory exceeds threshold (adjustable)
   - Collection frequency: O(log n) where n is concept count

### 6.2 Multi-Agent Collaboration  
Multiple instances of llama2.c could be assigned different roles (e.g., a "planner," a "critic," and a "summarizer") and operate over the same concept graph. Each agent's output updates the memory, and each agent can see the others' changes. This is reminiscent of "blackboard architectures" in classical AI, but now powered by advanced LLMs.

### 6.3 Evaluation Metrics  
- **Context Consistency:** How often does the system refer to the correct facts over lengthy dialogues that exceed the model's native context window?  
- **Response Correctness:** When knowledge changes (e.g., a slot is updated), how quickly does the system reflect the new information in subsequent replies?  
- **Graph Growth and Maintenance:** The rate at which the concept graph expands, how often old or unused concepts are pruned, and the overhead of searching for relevant slots.

### 7.3.1 Quantitative Evaluation Framework

Specific benchmarks to evaluate the hybrid architecture:

1. **Memory-Augmented QA Metrics**
   - Context Extension Score (CES): % correct answers on queries separated by N tokens
     - Formula: CES(N) = correct_answers(N) / total_questions(N)
     - Measured at N = {1K, 5K, 10K, 50K} tokens
   - Knowledge Editing Latency (KEL): Time from slot update to reflection in output
     - Measured in ms: average time until model reflects updated information

2. **Memory Efficiency Metrics**
   - Concepts per KB: Number of distinct concepts storable per 1KB memory
   - Slot Access Latency: Microseconds required for slot retrieval
     - Direct slot lookup
     - Pattern-based retrieval

---

## 7. Future Work

1. **Embedding-Based Retrieval:** Mapping each concept's ID to a stable embedding. The LLM could directly perform a vector similarity search to find relevant concepts, merging the best aspects of retrieval-augmented generation with a fully local system.  

2. **Symbol-Guided Attention Steering:** Instead of only capturing attention from the LLM, we can feed symbolic constraints *into* the LLM. For example, if the user has declared "John does *not* like Mary," we could reduce or zero out attention connections between "John" and "Mary" in certain heads, reinforcing the contradictory notion.  

3. **Deep Interpretability Tools:** Building visualization platforms to watch how ephemeral attention edges correlate with stable concept-slot relationships in real time.  

4. **Extended Reasoning and Logic Layers:** Incorporating classical logic or constraint solvers on top of Claire, so the system can do explicit deductive reasoning, and then feed the results back to the LLM.  

5. **Continual Fine-Tuning / Parameter Updating:** Investigate ways to *optionally* nudge the LLM's internal weights based on frequent concept-slot usage, bridging the gap between dynamic symbolic memory and static model parameters.

---

## 8. Conclusion

We are on the **frontier** of a new generation of AI systems that seamlessly fuse the **discrete explicitness** of symbolic knowledge graphs with the **continuous, distributed** representations of large language models. Our proposed integration of **llama2.c** (a minimal local LLaMA-2 implementation) with **Claire's** concept-slot memory system demonstrates that:

- **Direct memory manipulation in C** allows a level of transparency and control seldom seen in mainstream neural frameworks.  
- **Persistent, editable knowledge** can complement a model's fleeting token context, granting the system a form of "long-term memory" that users can inspect or modify.  
- **Attentional data** from the LLM can be reified into symbolic links, helping us interpret and refine how the model connects concepts.  

Despite challenges in performance, representation, and consistency, the potential payoffs are enormous: truly **interactive, dynamic AI** that can maintain, evolve, and explain a persistent knowledge base—**all on a personal device**, with no black-box cloud dependencies. As these ideas mature, we anticipate a growing ecosystem of local, memory-augmented AI agents that combine the best of neural and symbolic paradigms. The result could transform how we build personal assistants, digital twins, and knowledge systems, ushering in a new era of flexible, introspectable intelligence.

---

## References (Illustrative)

- Touvron, H. et al. (2023). *LLaMA: Open and Efficient Foundation Language Models.*  
- Karpathy, A. (2023). *llama2.c – Minimal LLM Implementation.* [GitHub Repository](https://github.com/karpathy/llama2.c)  
- Vaswani, A. et al. (2017). *Attention Is All You Need.* Advances in Neural Information Processing Systems.  
- Minsky, M. (1974). *A Framework for Representing Knowledge.* MIT-AI Laboratory Memo 306.  
- Sun, R. (1994). *Integrating Rules and Connectionism for Robust Commonsense Reasoning.* Wiley.  
- Claire Project (2025). *Concept-Slot Implementation in C.* (Internal documentation / GitHub link)

## License

This project is licensed under the terms of the Cryptographic Autonomy License 1.0. See the [LICENSE](LICENSE) file for details.

The CAL-1.0 ensures that users have control over their data and the software they use. For more information, visit the [Open Source Initiative's page on CAL-1.0](https://opensource.org/license/cal-1-0/).

---

**Contact / Further Information**  
If you are interested in contributing or exploring this hybrid approach further, please reach out or visit our shared repository. Together, we can push the boundaries of **local, transparent, and deeply conceptual** AI systems.
