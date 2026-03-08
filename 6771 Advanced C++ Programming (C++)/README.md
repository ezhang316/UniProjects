# 6771: Advanced C++ Programming

Intensively covers modern C++ techniques for building efficient, well-designed software. Progression from standard library mastery through custom data structures to advanced template programming and metaprogramming.

## Assignments

### Assignment 1: Standard Containers & Iterators
**Word Ladder Algorithm**

Implements a word ladder solver using BFS and C++ standard library containers. Demonstrates efficient use of STL algorithms and custom iterator patterns.

**Techniques:**
- `std::unordered_set` for O(1) lexicon lookups
- Standard algorithms (`std::find`, `std::transform`)
- Iterator usage patterns
- Performance optimization of graph search
- Lexicon file I/O and parsing

**Key Concepts:**
- Standard container efficiency analysis
- Algorithm complexity and practical optimization
- BFS pathfinding with STL

### Assignment 2: Custom Iterators & Resource Management
**Filtered String View**

Implements a non-owning filtered string view with custom iterator support. Demonstrates RAII principles, const-correctness, and iterator patterns.

**Techniques:**
- Custom iterator implementation
- String view abstraction without data copying
- Operator overloading for view semantics
- Resource management and const-correctness
- Template specialization for generic programming

**Key Concepts:**
- Iterator traits and iterator categories
- RAII (Resource Acquisition Is Initialization)
- View/proxy pattern for efficient data handling

### Assignment 3: Polymorphism & Metaprogramming
**Data Processing Pipeline**

Dynamic and static polymorphism for composable data transformations. Demonstrates both runtime polymorphism (virtual functions) and compile-time polymorphism (templates).

**Techniques:**
- Virtual functions and dynamic polymorphism
- Template programming and generic containers
- Concepts for template constraints
- Exception handling and error propagation
- Pipeline pattern for data transformation

**Advanced Concepts:**
- Static vs. dynamic polymorphism tradeoffs
- Template metaprogramming patterns
- Compile-time type checking with concepts
- Exception safety guarantees

## Technical Progression

1. **STL Mastery:** Effective use of standard library containers and algorithms
2. **Custom Data Structures:** Building safe, efficient abstractions
3. **Advanced Templates:** Generic programming and metaprogramming techniques
4. **Performance:** Profiling, optimization, and modern C++ best practices