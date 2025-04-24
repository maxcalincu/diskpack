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

  class ConnectivityGraph {
  public:
    const size_t MAX_CORONA_SIGNATURES = 5'000;
    using VersionDiffStack = std::stack<std::shared_ptr<SignatureList>>;
  private:
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
    std::deque<DiskPointer> corona;   /// Peripheral disks. At any moment two consequitive disks are
                                      /// tangent. (i. e. disks with indexes i and i + 1 where i + 1 <
                                      /// corona.size())

    std::vector<SSORef> operators_front;  /// Helper objects for compuitng new disks' centers
    std::vector<SSORef> operators_back;   ///
    IntervalPair leaf_front;              ///
    IntervalPair leaf_back;               ///
    OperatorLookupTable &lookup_table;    ///
    std::stack<bool> push_history;        ///

    SpiralSimilarityOperator
    GetOperatorsProduct(                 /// Helper function for compuitng new disks' centers
        const size_t &begin, const size_t &end,
        const std::vector<SSORef> &operators) const;

    void GetSortedCorona(
        const std::list<DiskPointer> &packing);   /// Collects disks tangent to the base
                                                /// and sorts them anti-clockwise

  public:
    Corona(const Disk &b, const std::list<DiskPointer> &packing,
          OperatorLookupTable &lookup_table);
    bool IsCompleted();                                   /// Checks whether corona is completed
    bool IsContinuous() const;                            /// Checks the assumption from row 7 and 8
    bool PeekNewDisk(Disk &new_disk, size_t index, const std::optional<ConnectivityGraph> &graph = std::nullopt);       
                                                          /// Constructs a new disk without changing the
                                                          /// state (eventually)
    void Push(const DiskPointer &disk, size_t index);     /// Pushes new disk into corona
    void Pop();                                           /// Pops a disk in LIFO order
    const Disk &GetBase();
    void DisplaySignature();
  };

  ///CoronaSignature

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

} // namespace CDP