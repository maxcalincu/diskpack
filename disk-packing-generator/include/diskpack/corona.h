#include <diskpack/geometry.h>
#include <queue>
#include <stack>
#include <set>
#include <list>
#include <optional>

#pragma once

namespace diskpack {

  class Corona;
  class CoronaSignature;
  class ConnectivityGraph;

  using CoronaSignaturePointer = std::shared_ptr<CoronaSignature>;
  using SignatureList = std::list<CoronaSignaturePointer>;
  bool CoronaSignaturePointerCompare(const CoronaSignaturePointer &a, const CoronaSignaturePointer &b);
  bool operator< (const CoronaSignaturePointer &a, const CoronaSignaturePointer &b);

  /// ConnectivityGraph checks some graph invariants that must hold in order
  /// for compact packings to exist. Underlying region can be narrowed (via Refine()).
  /// The refine operations can be undone via Restore().

  /// The class is used in Searcher to test regions.

  class ConnectivityGraph {
  public:
    const size_t MAX_CORONA_SIGNATURES = 5'000;
    BaseType PRECISION_THRESHOLD;
    using VersionDiffStack = std::stack<std::shared_ptr<SignatureList>>;
  private:
    bool is_broken = false;
    std::queue<std::tuple<size_t, size_t, size_t>> redundant_triangles;
    std::vector<SignatureList> signatures;
    std::vector<VersionDiffStack> diffs;
    std::vector<std::vector<size_t>> transitions;
    std::vector<std::vector<bool>> edges;
    void Push(const CoronaSignature& signature);
    void Pop(const CoronaSignature& signature);
    void RemoveRedundantTriangles(std::shared_ptr<std::vector<std::shared_ptr<SignatureList>>> diff = std::shared_ptr<std::vector<std::shared_ptr<SignatureList>>>());

    void UpdateEdges();
    size_t GetTransitionsConst(size_t base, size_t i, size_t j) const;
    size_t& GetTransitions(size_t base, size_t i, size_t j);
    void CoronaFill(Corona& corona, std::list<DiskPointer> &packing, size_t starting_index, std::set<CoronaSignaturePointer, decltype(&CoronaSignaturePointerCompare)> &unique_signatures);

  public:
    size_t Size() const;
    void DisplaySignatures() const;
    ConnectivityGraph(OperatorLookupTable &lookup_table);
    bool HasOverflow() const;

    void Refine(OperatorLookupTable &lookup_table);
    bool Restore();

    bool IsViable() const;
    bool HasTriangle(size_t i, size_t j, size_t k) const;
  };

  /// Corona class
  class Corona {
    const size_t DEFAULT_OPERATORS_SIZE = 12;
    friend class CoronaSignature;
    friend class ConnectivityGraph;

    const Disk &base;                 /// The central disk
    std::deque<DiskPointer> corona;   /// Peripheral disks. At any point two consequitive disks are
                                      /// tangent (i. e. disks with indexes i and i + 1 where i + 1 <
                                      /// corona.size()).

    std::vector<SSORef> operators_front;  /// Helper objects for compuitng new disks' centers
    std::vector<SSORef> operators_back;   ///
    IntervalPair leaf_front;              ///
    IntervalPair leaf_back;               ///
    OperatorLookupTable &lookup_table;    ///
    std::stack<bool> push_history;        ///

    SpiralSimilarityOperator
    GetOperatorsProduct(
        const size_t &begin, const size_t &end,
        const std::vector<SSORef> &operators) const;

    void GetSortedCorona(const std::list<DiskPointer> &packing);    /// Constructs a anti-clockwise sorted corona from scratch.

  public:
    Corona(const Disk &b, const std::list<DiskPointer> &packing,
          OperatorLookupTable &lookup_table);
    bool IsCompleted();                                   /// Checks whether corona is complete
    bool IsContinuous() const;                            /// Checks the continuity assumption
    bool PeekNewDisk(Disk &new_disk, size_t index, const std::optional<ConnectivityGraph> &graph = std::nullopt);       
                                                          /// Constructs a new disk without changing the
                                                          /// state (eventually)
    void Push(const DiskPointer &disk, size_t index);     /// Pushes new disk into corona
    void Pop();                                           /// Pops a disk in LIFO order
    const Disk &GetBase();
    void DisplaySignature();
  };

  ///CoronaSignature
  /// Two coronas are considered equivalent if, for all pairs (i, j), 
  /// the count of adjacent positions occupied by disks of type i and j 
  /// is identical. The signature of a corona is uniquely determined by 
  /// this adjacency count, and two coronas have the same signature if 
  /// and only if they are equivalent.

  class CoronaSignature {
    const size_t radii_size;
    const size_t base;
    std::vector<size_t> transitions;
    size_t& GetTransitions(size_t i, size_t j);
    size_t GetTransitionsConst(size_t i, size_t j) const;

    friend class ConnectivityGraph;
    friend Corona;

  public:
    std::vector<bool> disks;
    std::vector<size_t> specimen_indexes;
    CoronaSignature(Corona &specimen);
    size_t GetBase() const;
    bool TestRadii(OperatorLookupTable& lookup_table) const;
    bool operator<(const CoronaSignature &other) const;
    bool operator==(const CoronaSignature &other) const;
  };

} // namespace diskpack